//
// Created by Christoffer Wärnbring on 2021-07-27.
//

#include "MutatorAndBitwiseAnd.h"

std::string MutatorAndBitwiseAnd::MutateLine(const std::string &line, size_t mutationnr) {
    return DuoMutation(line, mutationnr, "&&", "&");
}

size_t MutatorAndBitwiseAnd::CheckMutationsPossible(const std::string &line) {
    return CountOccurences(line, "&&") + CountOccurences(line, "&");
}