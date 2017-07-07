#! /usr/bin/env Rscript

library(plyr)
library(reshape2)

args <- commandArgs(trailingOnly=TRUE)

# Get a debug flag
debug = !is.na(Sys.getenv("DEBUG", NA))

# Set the file suffixes to read
# These will also correspond to data keys

file_suffs <- c("data", "background", "signal")

data <- vector(mode="list", length=length(file_suffs))
names(data) <- file_suffs

# Check that the files exist
# If yes, dump into data

## READ THE FILES ##

for (i_file in 1:length(file_suffs)) {

  file_name <- paste("datacards/datacard_", file_suffs[i_file], ".csv", sep="")
  if (!file.exists(file_name)) {

    print(paste("File missing:", file_name))
    quit(save="no", status=5)

  }

  data[[i_file]] <- read.csv(file_name)
}

if (debug) {
  print("## DATA ##")
  print(data)
}

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
#  ^---------    regions     ---------^
#

# This function will transform our data
# Each row will look like the mess above
# Each column will be a different process

make_matrix <- function (file, uncertainties=FALSE) {

  if (uncertainties) {
    output <- melt((t(data[[file]])[c(FALSE, TRUE),])^2,
                   varnames=c("bin", "reg_proc"))
  } else {
    output <- melt(t(data[[file]])[c(TRUE, FALSE),],
                   varnames=c("bin", "reg_proc"))
  }

  # Add some columns

  output <- mutate(output, split_loc=regexpr("/", reg_proc))
  output <- mutate(output,
                   region=substr(reg_proc, 1, split_loc - 1),
                   process=substr(reg_proc, split_loc + 1, length(reg_proc))
                   )

  return(acast(output, region + bin ~ process))
}

# Get our one column matrix of data

x <- make_matrix("data")

# We will construct the matrix that will be used to calculate theta

theta_matrix <- make_matrix("background")

# Collapse into one column for mu

mu <- theta_matrix %*% rep.int(1, ncol(theta_matrix))

# Get the uncertainties squared

sig2 <- make_matrix("background", TRUE) %*% rep.int(1, ncol(theta_matrix))

if (debug) {
  print("## matrix ##")
  print(theta_matrix)
  print("## mu ##")
  print(mu)
  print("## sig2 ##")
  print(sig2)
  print("## X ##")
  print(x)
}

## FUNCTION TO MINIMIZE ##

log_likelihood <- function (factors) {
  # This is sort of the negative log likelihood function, which we want to minimize
  # I dropped the constants
  # Factors should start at all 1

  theta <- theta_matrix %*% factors

  return(sum( (x - theta)^2/(2*theta) + 0.5*log(theta) + (theta - mu)^2/(2*sig2)))

}

## FINAL ##

# Look how easy this is!

result <- optim(rep(1.0, ncol(theta_matrix)), log_likelihood)

# Write output file


if (sum(args == c("time")) == 1) {
  out_name <- paste("datacards/fit_result_", format(Sys.time(), "%y%m%d_%H%M"), ".txt", sep="")
} else {
  out_name <- "datacards/fit_result.txt"
}

sink(out_name)
for (i_proc in 1:ncol(theta_matrix)) {
  cat(colnames(theta_matrix)[i_proc])
  cat("    ")
  cat(result[["par"]][i_proc])
  cat("\n")
}
sink()

print(out_name)