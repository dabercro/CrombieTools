
# This is the first example that we used for the "What's in a Thesis?" presentation

first_example <- function(x, theta_matrix) {

  output <- function (factors) {
    # This is sort of the negative log likelihood function, which we want to minimize
    # I dropped the constants
    # Factors should start at all 1

    theta <- theta_matrix %*% factors

    return(sum( (x - theta)^2/(2*theta) + 0.5*log(theta) + (theta - mu)^2/(2*sig2)))

  }

  return(output)
}

# This is the log likelihood of binned data assuming poissonian distribution for each
# data is the data from the real or toy experiment
# mu scales the signal shape and theta does all the other shaping.

binned_poisson_likelihood <- function(data, mu, theta, lambda_function, log = TRUE) {

  # Get the lambdas from theta and mu

  lambdas = lambda_function(mu, theta)

  return(sum(dpois(data, lambdas, log)))

}
