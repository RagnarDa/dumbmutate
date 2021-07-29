//
// Created by Christoffer Wärnbring on 2021-07-13.
//

#ifndef DUMBMUTATE_MUTATORXYZ_H
#define DUMBMUTATE_MUTATORXYZ_H

#include "MutatorBase.h"

class MutatorXYZ : public MutatorBase{
public:
size_t CheckMutationsPossible(const std::string &line) override;
std::string MutateLine(const std::string &line, size_t mutationnr) override;
};


#endif //DUMBMUTATE_MUTATORXYZ_H
