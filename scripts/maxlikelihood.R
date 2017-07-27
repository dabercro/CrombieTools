#! /usr/bin/env Rscript

# Get a debug flag
debug = !is.na(Sys.getenv("DEBUG", NA))
args <- commandArgs(trailingOnly=TRUE)

library(plyr)
library(reshape2)

load <- function(file_name) {
  source(paste(Sys.getenv("CROMBIEPATH"), "/R/", file_name, ".R", sep=""))
}

# Load R scripts
load("read_datacards")
load("shape_datacards")
load("likelihood_functions")

# Set the file suffixes to read
# These will also correspond to data keys

file_directory = args[1]
file_suffs = c("data", "background", "signal")

data <- read_datacards(file_directory, file_suffs)
make_matrix <- shape_datacards(data)

# Get our one column matrix of data

x <- make_matrix("data")

# We will construct the matrix that will be used to calculate theta

theta_matrix <- make_matrix("background")

# Collapse into one column for mu

mu <- theta_matrix %*% rep.int(1, ncol(theta_matrix))

# Get the uncertainties squared

sig2 <- make_matrix("background", 1) %*% rep.int(1, ncol(theta_matrix))

## FUNCTION TO MINIMIZE ##

log_likelihood <- first_example(x, theta_matrix)

## FIT ##

result <- optim(rep(1.0, ncol(theta_matrix)), log_likelihood)

# Write output file

if (sum(args == c("time")) == 1) {
  out_name <- paste(file_directory, "/fit_result_", format(Sys.time(), "%y%m%d_%H%M"),
                    ".txt", sep="")
} else {
  out_name <- paste(file_directory, "/fit_result.txt", sep="")
}

sink(out_name)
for (i_proc in 1:ncol(theta_matrix)) {
  cat(colnames(theta_matrix)[i_proc])
  cat("    ")
  cat(result[["par"]][i_proc])
  cat("\n")
}
sink()

print(result)

print(out_name)
