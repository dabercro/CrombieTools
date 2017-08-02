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

# We make our own sorting function since R and SQL sort differently
# (R is not case sensisitive)

consistent_sort <- function(input) {

  return(input[order(input$region, input$process, input$bin), ])

}

if (exists('input_file')) {

  yields <- consistent_sort(get_yields(input_file))
  master <- get_master(input_file)
  links <- get_links(input_file)

  match_bins <- function(to_match) {
    return(
      yields[yields$process %in% to_match$process &
             yields$region %in% to_match$region, ]
    )
  }

  n_free <- nrow(match_bins(master))
  n_transfer <- nrow(match_bins(links))

  starting_theta <- append(rep(1, n_free), rep(0, n_transfer))

  # Add a column saying which column to merge to for each linked region and process
  # Then add columns with the contents and uncertainties for each linked area
  # Finally, add columns with transfer factors and uncertainties

  merged_links <- consistent_sort(
                    mutate(
                      merge(merge(yields, links),
                            yields[yields$region == master$region, ],
                            by.x = c('connect_to', 'bin'), by.y = c('process', 'bin'),
                            suffixes = c('', '.link')),
                      ratio = contents/contents.link,
                      ratio_unc = sqrt((stat_unc/contents.link)^2 + (stat_unc.link * contents/(contents.link)^2)^2)))

  transfer_factors <- merged_links$ratio
  transfer_uncs <- merged_links$ratio_unc

  # Get the uncontrolled yields

  uc_yields <- anti_join(yields[yields$type == "background", ], links)
  uc_yields <- aggregate(uc_yields$contents,
                         by=list(region=uc_yields$region,
                                 bin=uc_yields$bin),
                         FUN=sum)
  uc_yields <- uc_yields[order(uc_yields$region, uc_yields$bin), ]$x

  # Generate a function from the datacard
  # that will estimate lambdas with a given mu and theta

  get_lambda_function <- function(signal_process) {

    signal <- yields[yields$process == signal_process, ]$contents

    lambdas <- function(mu, theta) {
      return(mu * signal + uc_yields)
    }

    return(lambdas)

  }

}
