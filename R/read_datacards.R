library(DBI)

read_sqlite3 <- function(file_name) {

  conn <- dbConnect(RSQLite::SQLite(), file_name)

  output <- dbGetQuery(con, "
    SELECT region, process, bin, contents, stat_unc, name AS type
    FROM yields INNER JOIN types ON types.type = yields.type
    ")

  dbDisconnect(conn)

  return(output)

}
