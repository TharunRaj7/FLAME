#include <Rcpp.h>
using namespace Rcpp;

// matching <- function(cov_to_drop, data, replace, covs) {
//   if (replace) {
//     match_index <-
//       bit_match(data[!unlist(data$missing), ], setdiff(covs, cov_to_drop))$match_index
//     units_matched <- which(!data$missing)[match_index]
//   }
//   else {
//     data_step <- data[!unlist(data$matched),]
//     data_step <- data_step[!unlist(data_step$missing), ]
//     match_index <-
//       bit_match(data_step, setdiff(covs, cov_to_drop))$match_index
//     units_matched <- which(!data$matched & !data$missing)[match_index]
//   }
//   return (list(match_index = match_index, units_matched = units_matched))
// }

// [[Rcpp::export]]
List call_matching(int cov_to_drop, DataFrame data, bool replace, IntegerVector covs) {
  //Environment env("package:FLAME");
  Function matching("matching");
  //Function matching = env["matching"];
  List matching_return = as<List>(matching(cov_to_drop, data, replace, covs));
  return List::create(Named("match_index") = matching_return["match_index"],
                      Named("units_matched") = matching_return["units_matched"]);
}

// [[Rcpp::export]]
List get_BF(int cov_to_drop, DataFrame data, bool replace, IntegerVector covs) {
  // Calculate number of units eligible to be matched
  IntegerVector treatedVect = data["treated"];
  LogicalVector missingDataVect = data["missing"];
  LogicalVector matchedDataVect = data["matched"];
  int data_rows = data.nrows();
  int n_control = 0;
  int n_treated = 0;
  if (replace) {
    for(int i = 0; i != data_rows; ++i) {
      if(treatedVect[i] == 0 && !missingDataVect[i] ){
        n_control++;
      } else if (treatedVect[i] == 1 && !missingDataVect[i]) {
        n_treated++;
      }
    }
  }
  else {
    for(int i = 0; i != data_rows; ++i) {
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

  List matching_step = call_matching(cov_to_drop, data, replace, covs);

  LogicalVector match_index = as<LogicalVector>(matching_step["match_index"]);
  IntegerVector units_matched = as<IntegerVector>(matching_step["units_matched"]);

  // Newly matched
  int n_control_matched = 0;
  int n_treated_matched = 0;

  for(int i = 0; i != units_matched.length(); ++i) {
    if(treatedVect[units_matched[i] - 1] == 0){
      //minus one to correct from indexing from R to Rcpp
      n_control_matched ++;
    } else {
      n_treated_matched ++;
      //minus one to correct from indexing from R to Rcpp
    }
  }

  // All matched units; for stopping rule purposes

  // IntegerVector matched_indices;
  // for(int i = 0; i != data_rows; ++i) {
  //   if(matchedDataVect[i]){
  //     matched_indices.push_back(i + 1);
  //     //plus one to correct for indexing
  //   }
  // }
  //
  // IntegerVector all = seq_len(data_rows);
  // IntegerVector all_unmatched = setdiff(all, union_(units_matched, matched_indices));
  // int number_unmatched = all_unmatched.length();
  // int n_control_unmatched = 0;
  // int n_treated_unmatched = 0;
  //
  // for(int i = 0; i != number_unmatched; ++i) {
  //   if(treatedVect[all_unmatched[i] - 1]==0){
  //     n_control_unmatched ++;
  //   }
  //   else {
  //     n_treated_unmatched ++;
  //   }
  // }

  int n_control_unmatched = n_control - n_control_matched;
  int n_treated_unmatched = n_treated - n_treated_matched;

  double prop_control_unmatched = (double) n_control_unmatched / (double) n_control;
  double prop_treated_unmatched = (double) n_treated_unmatched / (double) n_treated;
  double BF;

  if((n_control == 0) | (n_treated == 0)) {
    BF = 0;
  }
  else {
    BF = (double) n_control_matched / (double) n_control + (double) n_treated_matched / (double) n_treated;
  }

  // Rcout << "n_control: " << n_control << std::endl;
  // Rcout << "n_treated: " << n_treated << std::endl;
  // Rcout << "n_control_matched: " << n_control_matched << std::endl;
  // Rcout << "n_control_unmatched: " << n_control_unmatched << std::endl;
  // Rcout << "n_treated_matched: " << n_treated_matched << std::endl;
  // Rcout << "n_treated_unmatched: " << n_treated_unmatched << std::endl;
  // Rcout << "match_index: " << match_index << std::endl;
  // Rcout << "units_matched: " << units_matched << std::endl;
  // Rcout << "prop_control_unmatched: " << prop_control_unmatched << std::endl;
  // Rcout << "prop_treated_unmatched: " << prop_treated_unmatched << std::endl;
  // Rcout << "BF: " << BF << std::endl;
  //Rcout << "all: " << all << std::endl;
  // Rcout << "all_unmatched: " << all_unmatched << std::endl;
  // Rcout << "data_rows: " << data_rows<< std::endl;
  // Rcout << "matched_indices: " << matched_indices << std::endl;
  // Rcout << "test_n_control_unmatched: " << test_n_control_unmatched << std::endl;
  // Rcout << "test_n_treated_unmatched: " << test_n_treated_unmatched << std::endl;

  return List::create(Named("BF") = BF,
                      Named("prop_unmatched") = List::create(
                        Named("control") = prop_control_unmatched,
                        Named("treated") = prop_treated_unmatched));
}
