#include <Rcpp.h>
using namespace Rcpp;

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//

// [[Rcpp::export]]
NumericVector get_BF(int cov_to_drop, DataFrame data, bool replace, List covs) {
// Calculate number of units eligible to be matched
  IntegerVector treatedVect = data["treated"];
  LogicalVector missingDataVect = data["missing"];
  LogicalVector matchedDataVect = data["matched"];

  if (replace) {
    int n_control = 0;
    int n_treated = 0;

    /*
     * How to translate data frame?
     * Is
     * How to represent $missing (0/1)
     */
    for(int i = 0; i != data.nrows(); ++i) {
      if(treatedVect[i] == 0 && !missingDataVect[i] ){
        n_control++;
      } else if (treatedVect[i] == 1 && !missingDataVect[i]) {
        n_treated++;
      }
    }
  }
  else {
    //!data$matched & !data$missing
    //
    int n_control = 0;
    int n_treated = 0;

    for(int i = 0; i != data.nrows(); ++i) {
      if(treatedVect[i] == 0
           && !matchedDataVect[i]
           && !missingDataVect[i]) {
        n_control++;
      } else if (treatedVect[i] == 1
                   && !matchedDataVect[i]
                   && !missingDataVect[i]) {
        n_treated++;
      }
    }
  }

  //Find out deleting rows (manipulation in general of dataframes).

  if (replace) {

    for(DataFrame::iterator i = data.begin(); i != data.end(); ++i) {

    }
    data[!data$missing, ]   , setdiff(covs, cov_to_drop)

    Function f("bit_match");
    int match_index =
      bit_match()$match_index
    units_matched <- which(!data$missing)[match_index]
  }

  NumericVector v  = NumericVector::create(1,3,2);

  return v;
}


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

/*** R
timesTwo(42)
*/
