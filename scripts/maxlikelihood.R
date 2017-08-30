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
signals <- unique(yields[yields$type == 'signal', ]$process)

for (i_signal in 1:length(signals)) {

  lambda_function <- get_lambda_function(signals[i_signal])
  log_likelihood <- function(theta) {

    return(
      nlog_likelihood_thetas(theta) -
      binned_poisson_likelihood(data, 0, theta, lambda_function)
    )

  }

  result <- optim(starting_theta, log_likelihood)

  # Write output file

#  if (sum(args == c("time")) == 1) {
#    out_name <- paste(file_directory, "/fit_result_", format(Sys.time(), "%y%m%d_%H%M"),
#                      ".txt", sep="")
#  } else {
#    out_name <- paste(file_directory, "/fit_result.txt", sep="")
#  }

#  sink(out_name)
#  for (i_proc in 1:ncol(theta_matrix)) {
#    cat(colnames(theta_matrix)[i_proc])
#    cat("    ")
#    cat(result[["par"]][i_proc])
#    cat("\n")
#  }
#  sink()

  print(result)
  print(data.frame(region=uc_yields$region,
                   bin=uc_yields$bin,
                   start=lambda_function(0, starting_theta),
                   end=lambda_function(0, result$par),
                   data=data))
}

#print(out_name)
