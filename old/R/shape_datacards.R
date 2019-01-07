## DETERMINE THE NUMBER OF DIFFERENT UNCERTAINTIES FOR BINS ##

read_datacards <- function(file_directory, file_suffs) {

  data <- vector(mode="list", length=length(file_suffs))
  names(data) <- file_suffs

  # Check that the files exist
  # If yes, dump into data

  ## READ THE FILES ##

  for (i_file in 1:length(file_suffs)) {

    file_name <- paste(file_directory, "/datacard_", file_suffs[i_file], ".csv", sep="")
    if (!file.exists(file_name)) {

      print(paste("File missing:", file_name))
      quit(save="no", status=5)

    }

    data[[i_file]] <- read.csv(file_name)
  }

  return(data)
}

count_uncertainties <- function(data) {

  binnames_split = strsplit(colnames(data[[1]]), '.', fixed=TRUE)

  num_split = as.data.frame(table(sapply(binnames_split, FUN=length)))$Freq

  # This should be an integer

  stopifnot(num_split[2] %% num_split[1] == 0)
  num_unc = num_split[2]/num_split[1]

  return(num_unc)

}

shape_datacards <- function (data) {

  num_unc <- count_uncertainties(data)

  ## GET VARIABLES FOR FUNCTION TO MINIMIZE

  # Get the parameters that will be used in the MLE.
  # Will maximize:
  # sum ( (x - theta)^2 / (2 theta) + 0.5 log (theta) + (theta - mu)^2 / (2 sig^2) )
  #

  # Data points for our bins
  # Collapse matrices into vectors like the following:
  #
  # xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  #  ^-- bins --^^-- bins --^^-- bins --^
  #  ^----------   regions   -----------^
  #

  # This function will transform our data
  # Each row will look like the mess above
  # Each column will be a different process

  make_matrix <- function (file, which_unc=0) {

    stopifnot(which_unc <= num_unc)

    mask = rep(FALSE, num_unc + 1)

    mask[1 + which_unc] = TRUE

    output <- melt((t(data[[file]]))^2,
                   varnames=c("bin", "reg_proc"))[mask,]

    # Add some columns

    output <- mutate(output, split_loc=regexpr("/", reg_proc))
    output <- mutate(output,
                     region=substr(reg_proc, 1, split_loc - 1),
                     process=substr(reg_proc, split_loc + 1, length(reg_proc))
                     )

    return(acast(output, region + bin ~ process))
  }

  return(make_matrix)
}
