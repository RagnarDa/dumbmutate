//
// Created by Christoffer Wärnbring on 2019-02-26.
//

#include "MutatorNEqual.h"
#include <string>
#include "MutatorNEqual.h"

std::string MutatorNEqual::MutateLine(const std::string &line, size_t mutationnr) {
	return DuoMutation(line, mutationnr, "!=", "==");
}

size_t MutatorNEqual::CheckMutationsPossible(const std::string &line) {
	return CountOccurences(line, "!=") + CountOccurences(line, "==");
}