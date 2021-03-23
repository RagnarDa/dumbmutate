//
// Created by Christoffer Wärnbring on 2019-03-01.
//

#include "MutatorIncrDecr.h"
std::string MutatorIncrDecr::MutateLine(const std::string &line, size_t mutationnr) {
	return DuoMutation(line, mutationnr, "++", "--");
}

size_t MutatorIncrDecr::CheckMutationsPossible(const std::string &line) {
	return (CountOccurences(line, "++") + CountOccurences(line, "--"));
}