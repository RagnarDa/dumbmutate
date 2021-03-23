//
// Created by Christoffer Wärnbring on 2019-03-04.
//

#include "MutatorAndOr.h"
std::string MutatorAndOr::MutateLine(const std::string &line, size_t mutationnr) {
	return DuoMutation(line, mutationnr, "&&", "||");
}

size_t MutatorAndOr::CheckMutationsPossible(const std::string &line) {
	return (CountOccurences(line, "&&") + CountOccurences(line, "||"));
}