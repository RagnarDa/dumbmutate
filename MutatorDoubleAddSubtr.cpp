//
// Created by Christoffer Wärnbring on 2019-03-01.
//

#include "MutatorDoubleAddSubtr.h"
std::string MutatorDoubleAddSubtr::MutateLine(const std::string &line, size_t mutationnr) {
	return DuoMutation(line, mutationnr, "++", "--");
}

size_t MutatorDoubleAddSubtr::CheckMutationsPossible(const std::string &line) {
	return (CountOccurences(line, "++") + CountOccurences(line, "--"));
}