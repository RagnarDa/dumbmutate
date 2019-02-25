//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#include "MutatorCaret.h"

std::string MutatorCaret::MutateLine(const std::string &line, size_t mutationnr) {
	return DuoMutation(line, mutationnr, "<", ">");
}

size_t MutatorCaret::CheckMutationsPossible(const std::string &line) {
	return CountOccurences(line, "<") + CountOccurences(line, ">");
}
