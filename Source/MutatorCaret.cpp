//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#include "MutatorCaret.h"

std::string MutatorCaret::MutateLine(const std::string &line, size_t mutationnr) {
	return DuoMutation(line, mutationnr, "<", ">");
}

size_t MutatorCaret::CheckMutationsPossible(const std::string &line) {
	if (CountOccurences(line, "<") == CountOccurences(line, ">"))
        {
		return 0; // filter out templates, includes
	};
	return CountOccurences(line, "<") + CountOccurences(line, ">");
}
