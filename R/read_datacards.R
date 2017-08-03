library(plyr)
library(dplyr)
library(DBI)

# Define the way files are read

read_sqlite3 <- function(file_name, query) {

  conn <- dbConnect(RSQLite::SQLite(), file_name)

  output <- dbGetQuery(conn, query)
  dbDisconnect(conn)

  return(output)

}

get_yields <- function(file_name) {

  return(
    read_sqlite3(
      file_name,
      "
      SELECT region, process, bin, contents, stat_unc, name AS type
      FROM yields INNER JOIN types ON types.type = yields.type
      ")
    )

}

get_master <- function(file_name) {

  return(
    read_sqlite3(
      file_name,
      "
      SELECT process, region FROM links WHERE connect_to = ''
      ")
    )

}

get_links <- function(file_name) {

  return(
    read_sqlite3(
      file_name,
      "
      SELECT process, region, connect_to FROM links WHERE connect_to != ''
      ")
    )

}

if (exists('input_file')) {

  yields <- get_yields(input_file)
  master <- get_master(input_file)
  links <- get_links(input_file)

  regions <- unique(yields$regions)
  bins <- unique(yields$bin)

  # Get the uncontrolled yields

  uc_yields <- anti_join(yields[yields$type == "background", ],
                         rbind(links[1:2], master),
                         by = c("region", "process"))

  # Merge the bins together

  uc_yields <- aggregate(uc_yields$contents,
                         by = list(region = uc_yields$region,
                                   bin = uc_yields$bin),
                         FUN = sum)

  # Sort those yields back to preferred bin order

  uc_yields <- uc_yields[order(uc_yields$region, uc_yields$bin), ]

  # A function that returns the proper yield bin for given parallel lists of regions and bins

  which_bin <- function(region, bin) {
    output <- c()
    for (i in 1:length(region)) {
      output[i] <- which(uc_yields$region == region[i] & uc_yields$bin == bin[i])
    }
    return(output)
  }

  connect_control <- function(process, bin, match) {
    output <- c()
    for (i in 1:length(process)) {
      output[i] <- which(match$region == master$region & match$process == process & match$bin == bin[i])
    }
    return(output)
  }

  # Add a column saying which column to merge to for each linked region and process
  # Then add columns with the contents and uncertainties for each linked area
  # Finally, add columns with transfer factors and uncertainties

  merged_links <- mutate(
                    inner_join(inner_join(yields, links, by = c("region", "process")),
                               yields[yields$region %in% master$region, ],
                               by = c("connect_to" = "process", "bin"),
                               suffix = c("", ".link")),
                    ratio = contents/contents.link,
                    ratio_unc = sqrt((stat_unc/contents.link)^2 + (stat_unc.link * contents/(contents.link)^2)^2),
                    which_bin = which_bin(region, bin))

  #
  # We split up nuisance parameters into three kinds
  #
  #   1. Ones that float freely (master)
  #   2. Ones connected to that master region
  #   3. Ones not connected directly to the master region
  #

  yields_master <- mutate(yields[yields$process %in% master$process &
                                 yields$region %in% master$region, ],
                          which_bin = which_bin(region, bin))

  proc_list <- merged_links$connect_to == master$process

  yields_direct <- mutate(merged_links[proc_list, ],
                          connect_bin = connect_control(connect_to, bin, yields_master))
  yields_indir <- mutate(merged_links[!proc_list, ],
                         connect_bin = connect_control(connect_to, bin, yields_direct))

  starting_theta <- append(rep(1, nrow(yields_master)), rep(0, nrow(merged_links)))

  # Get the indices for splitting the theta parameters

  n_master <- nrow(yields_master)
  direct_start <- n_master + 1
  direct_end <- nrow(yields_direct) + n_master
  indirect_start <- direct_end + 1
  indirect_end <- nrow(merged_links) + n_master

  # Get the uncontrolled yields

  uncontrolled_yields <- uc_yields$x

  # Generate a function from the datacard
  # that will estimate lambdas with a given mu and theta

  get_lambda_function <- function(signal_process) {

    signal <- yields[yields$process == signal_process, ]$contents

    lambdas <- function(mu, theta) {

      output <- mu * signal + uncontrolled_yields

      theta_free <- theta[1:n_master]
      theta_direct <- theta[direct_start:direct_end]
      theta_indirect <- theta[indirect_start:indirect_end]

      final_free <- yields_master$contents * theta_free

      # Add the master yields

      for (i_bin in 1:n_master) {
        o_bin <- yields_master$which_bin[i_bin]
        output[o_bin] <- output[o_bin] + final_free[i_bin]
      }

      # Transfer factor

      R_direct <- theta_direct * yields_direct$ratio_unc + yields_direct$ratio

      for (i_bin in 1:length(R_direct)) {
        R_direct[i_bin] <- R_direct[i_bin] * final_free[yields_direct$connect_bin[i_bin]]
        o_bin <- yields_direct$which_bin[i_bin]
        output[o_bin] <- output[o_bin] + R_direct[i_bin]
      }

      R_indirect <- theta_indirect * yields_indir$ratio_unc + yields_indir$ratio

      for (i_bin in 1:length(R_indirect)) {
        o_bin <- yields_indir$which_bin[i_bin]
        output[o_bin] <- output[o_bin] + R_direct[yields_indir$connect_bin[i_bin]] * R_indirect[i_bin]
      }

      return(output)
    }

    return(lambdas)
  }
}
