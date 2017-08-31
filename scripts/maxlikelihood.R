#! /usr/bin/env Rscript

# Get a debug flag
debug = !is.na(Sys.getenv("DEBUG", NA))
args <- commandArgs(trailingOnly=TRUE)

library(plyr)
library(reshape2)

load <- function(file_name, local=FALSE) {
  source(paste(Sys.getenv("CROMBIEPATH"), "/R/", file_name, ".R", sep=""), local=local)
}

input_file = args[1]

# Load R scripts
load("read_datacards")
load("shape_datacards")
load("likelihood_functions")

## FIT ##

stopifnot(exists('yields'))

data <- yields[yields$type == 'data', ]$contents
if (sum(args == c("bg")) == 1) {
  signals <- c("")
} else {
  signals <- unique(yields[yields$type == 'signal', ]$process)
}

for (i_signal in 1:length(signals)) {

  lambda_function <- get_lambda_function(signals[i_signal])
  log_likelihood <- function(theta) {

    return(
      nlog_likelihood_thetas(theta) -
      binned_poisson_likelihood(data, theta, lambda_function)
    )

  }

  result <- optim(starting_theta, log_likelihood,
                  control=list(maxit=1000000))

  # Write output file

  store_result <- data.frame(region=uc_yields$region,
                             bin=uc_yields$bin,
                             start=lambda_function(starting_theta),
                             end=lambda_function(result$par),
                             data=data)

  if (debug) {
    print(result)
    print(store_result)
  }

  regions <- unique(store_result$region)

  for (i_region in 1:length(regions)) {

    if (sum(args == c("stamp")) == 1) {
      stamp <- paste("_", format(Sys.time(), "%y%m%d_%H%M"), sep="")
    } else {
      stamp <- ""
    }

    sink(file.path(
           dirname(input_file),
           paste(signals[i_signal], "_", regions[i_region], stamp, ".txt", sep="")
         ))

    contents <- store_result[store_result$region == regions[i_region], ]$end
    for (i_bin in 1:length(contents)) {
      cat(contents[i_bin])
      cat("\n")
    }
    sink()
  }

}
