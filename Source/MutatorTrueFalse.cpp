//
// Created by Christoffer Wärnbring on 2019-03-05.
//

#include "MutatorTrueFalse.h"
std::string MutatorTrueFalse::MutateLine(const std::string &line, size_t mutationnr) {
	return DuoMutation(line, mutationnr, "true", "false");
}

size_t MutatorTrueFalse::CheckMutationsPossible(const std::string &line) {
	return (CountOccurences(line, "true") + CountOccurences(line, "false"));
}