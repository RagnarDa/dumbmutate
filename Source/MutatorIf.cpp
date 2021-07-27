//
// Created by Christoffer Wärnbring on 2021-04-27.
//

#include "MutatorIf.h"
#include <algorithm>


std::string MutatorIf::MutateLine(const std::string &line, size_t mutationnr) {
    return DuoMutation(line, mutationnr, "if (", "if (!");
}

size_t MutatorIf::CheckMutationsPossible(const std::string &line) {
    return CountOccurences(line, "if (") + CountOccurences(line, "if (!");
}
