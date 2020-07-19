
# Function to source files to avoid using devtools
sourceFolder <- function(folder, recursive = FALSE, ...)
{
  files <- list.files(folder, pattern = "[.][rR]$",
                      full.names = TRUE, recursive = recursive)
  if (!length(files))
    stop(simpleError(sprintf('No R files in folder "%s"', folder)))
  src <- invisible(lapply(files, source, ...))
  message(sprintf('%s files sourced from folder "%s"', length(src), folder))
}
sourceFolder("INCLUDE DIRECTORY HERE")
#set.seed(45)


# Testing with native funtions
#####################################
n <- 100
p <- 5
data <- gen_data(n, p) # Data we would like to match
holdout <- gen_data(n, p) # Data we will train on, to compute PE
FLAME_out <- FLAME(data = data, holdout = holdout)
#print(result)
####################################




#Testing with custom functions
###################################
n <- 100
p <- 5
data <- gen_data(n, p) # Data we would like to match
holdout <- gen_data(n, p) # Data we will train on, to compute PE
my_fit <- dbarts::bart
my_fit_params <- list(ntree = 100, verbose = FALSE, keeptrees = TRUE)
my_predict <- function(bart_fit, new_data) {
  return(colMeans(predict(bart_fit, new_data)))
}
FLAME_out <-
  FLAME(data = data, holdout = holdout,
        user_PE_fit = my_fit, user_PE_fit_params = my_fit_params,user_PE_predict = my_predict)
###################################
