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

//holdout, covs, cov_to_drop,
//PE_fit, PE_predict, PE_fit_params, PE_predict_params
//Thing to talk: differences in using library() vs import/install in the DESCRIPTION
// /*** R
// printObj <- function(x){
//     print(x)
// }
// */

int lengthOfUnique (DataFrame data, String colName);

// [[Rcpp::export]]
double get_PE(int cov_to_drop, IntegerVector covs, List holdout, String PE_method,
               SEXP user_PE_fit, List user_PE_fit_params,
               SEXP user_PE_predict, List user_PE_predict_params){

  SEXP PE_fit = R_NilValue;
  List PE_fit_params = R_NilValue;
  SEXP PE_predict = R_NilValue;
  List PE_predict_params = R_NilValue;


  //Function print("printObj");
  // Rprintf("user_PE_fit: ");
  // print(user_PE_fit);
  // Rprintf("user_PE_predict: ");
  // print(user_PE_predict);

if(user_PE_fit != R_NilValue){
  //Rcout<<"user pe fit not null bro\n";
  PE_fit = user_PE_fit;
  PE_fit_params = user_PE_fit_params;
}
else{
  if (strcmp(PE_method.get_cstring(), "ridge") == 0){
    Function glmnet("cv.glmnet");
    PE_fit = glmnet;
    String family;
    DataFrame holdFrame = as<DataFrame>(holdout[0]);
    if (lengthOfUnique(holdFrame, "outcome") == 2){
      family = "binomial";
    }else{
      family = "gaussian";
    }
    PE_fit_params = List::create(Named("family") = family.get_cstring(), Named("nfolds") = 5);
    //print(PE_fit_params);

  }else if (strcmp(PE_method.get_cstring(), "xgb") == 0){
    Function cv("cv_xgboost");
    PE_fit = cv;
    PE_fit_params = List::create();
  }else{
    //Need to check error handling
    throw(Rcpp::exception("PE_method not recognized. To supply your own function, use user_PE_fit and user_PE_predict"));
  }
}

if(user_PE_predict != R_NilValue){
  //Rcout<<"user pe predict not null bro\n";
  PE_predict = user_PE_predict;
  PE_predict_params = user_PE_predict_params;
}else{
  Function predictFunc("predict");
  PE_predict = predictFunc;
  List l;
  PE_predict_params = l;
}

Function retFunc("predict_master");
double PE = as<double>(retFunc(holdout, covs, cov_to_drop, PE_fit, PE_predict, PE_fit_params, PE_predict_params));
//Rprintf("value = success, PE = %f \n", PE);
return PE;
}

//Function tested and verified to work correctly
int lengthOfUnique (DataFrame data, String colName){
  std::unordered_set<double>values;
  NumericVector v = data[colName];
  for (int i = 0; i < v.size(); i++){
    values.insert(v[i]);
  }
  //Rprintf("values.size = %d", values.size());
  return values.size();
}
