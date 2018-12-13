library(DBI)

if (!exists("read_sqlite3")) {
  source(paste(Sys.getenv("CROMBIEPATH"), "/old/R/read_datacards.R", sep=""))
}

create_results_tables <- function (file_name) {

  if (no_table(file_name, 'postfit_yields')) {

    read_sqlite3(file_name, "
                 CREATE TABLE postfit_yields(
                 region VARCHAR(64),
                 bin INT,
                 yield DOUBLE,
                 signal VARCHAR(64),
                 PRIMARY KEY (region, bin, signal)
                 )
                 ", stmt = TRUE)

  }
               
  if (no_table(file_name, 'fit_params')) {

    read_sqlite3(file_name, "
                 CREATE TABLE fit_params(
                 theta_index INT,
                 process VARCHAR(64),
                 region VARCHAR(64),
                 bin INT,
                 connect_to VARCHAR(64),
                 value DOUBLE,
                 signal VARCHAR(64),
                 PRIMARY KEY (theta_index, signal)
                 )
                 ", stmt = TRUE)
  }

}

dump_fit_result <- function (file_name, result, result_yields, signal) {

  conn <- dbConnect(RSQLite::SQLite(), file_name)

  dbWriteTable(conn, "postfit_yields",
               mutate(result_yields[c(T, T, F, T, F)], signal = signal),
               append = TRUE)

  dbDisconnect(conn)

}
