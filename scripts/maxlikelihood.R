#! /usr/bin/env Rscript

# Get a debug flag
debug = !is.na(Sys.getenv("DEBUG", NA))
args <- commandArgs(trailingOnly=TRUE)

library(plyr)
library(reshape2)

load <- function(file_name, local=FALSE) {
  source(paste(Sys.getenv("CROMBIEPATH"), "/old/R/", file_name, ".R", sep=""), local=local)
}

input_file = args[1]

# Load R scripts
load("read_datacards")
load("shape_datacards")
load("likelihood_functions")
load("dump_fit_results")

## FIT ##

if (sum(args == c("fresh")) == 1) {
  read_sqlite3(file_name, "DROP TABLE IF EXISTS postfit_yields", stmt=TRUE)
  read_sqlite3(file_name, "DROP TABLE IF EXISTS fit_params", stmt=TRUE)
}

create_results_tables(input_file)

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
                             yield=lambda_function(result$par),
                             data=data)

  if (debug) {
    print(result)
    print(store_result)
  }

  dump_fit_result(input_file, result, store_result, signals[i_signal])

}
