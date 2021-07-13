//
// Created by Christoffer Wärnbring on 2021-07-13.
//

#include "MutatorXYZ.h"
// Just make one replacement per line, see if that works
std::string MutatorXYZ::MutateLine(const std::string &line, size_t mutationnr) {
    std::string rtrn = line;
    if (GetOccurence(line, "X", 0) != std::string::npos)
        rtrn.replace(GetOccurence(line, "X", 0), 1, "Y");
    if (GetOccurence(line, "x", 0) != std::string::npos)
        rtrn.replace(GetOccurence(line, "x", 0), 1, "y");
    if (GetOccurence(line, "Y", 0) != std::string::npos)
        rtrn.replace(GetOccurence(line, "Y", 0), 1, "X");
    if (GetOccurence(line, "y", 0) != std::string::npos)
        rtrn.replace(GetOccurence(line, "y", 0), 1, "x");
    if (GetOccurence(line, "Z", 0) != std::string::npos)
        rtrn.replace(GetOccurence(line, "Z", 0), 1, "X");
    if (GetOccurence(line, "z", 0) != std::string::npos)
        rtrn.replace(GetOccurence(line, "z", 0), 1, "x");
    return rtrn;
}

size_t MutatorXYZ::CheckMutationsPossible(const std::string &line) {
    // See if it ever occurs
    return (GetOccurence(line, "X", 0) != std::string::npos
        || GetOccurence(line, "x", 0) != std::string::npos
        || GetOccurence(line, "Y", 0) != std::string::npos
        || GetOccurence(line, "y", 0) != std::string::npos
        || GetOccurence(line, "Z", 0) != std::string::npos
        || GetOccurence(line, "z", 0) != std::string::npos)
     ? 1 : 0;
}