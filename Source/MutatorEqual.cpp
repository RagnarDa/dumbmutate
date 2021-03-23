//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#include <string>
#include "MutatorEqual.h"

std::string MutatorEqual::MutateLine(const std::string &line, size_t mutationnr) {
	return DuoMutation(line, mutationnr, "=", "==");
}

size_t MutatorEqual::CheckMutationsPossible(const std::string &line) {
	return CountOccurences(line, "=") + CountOccurences(line, "==");
}
