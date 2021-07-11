//
// Created by Christoffer Wärnbring on 2021-04-27.
//

#include "MutatorIf.h"


std::string MutatorIf::MutateLine(const std::string &line, size_t mutationnr) {
    std::string str (line);
    str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
    return DuoMutation(str, mutationnr, "if(", "if(!");
}

size_t MutatorIf::CheckMutationsPossible(const std::string &line) {
    std::string str (line);
    str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
    return CountOccurences(str, "if(") + CountOccurences(str, "if(!");
}
