//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#include "MutatorAddSubtr.h"

std::string MutatorAddSubtr::MutateLine(const std::string &line, size_t mutationnr) {
	return DuoMutation(line, mutationnr, "+", "-");
}

size_t MutatorAddSubtr::CheckMutationsPossible(const std::string &line) {
	return CountOccurences(line, "+") + CountOccurences(line, "-");
}