//
// Created by Christoffer Wärnbring on 2021-07-27.
//

#ifndef DUMBMUTATE_MUTATORANDBITWISEAND_H
#define DUMBMUTATE_MUTATORANDBITWISEAND_H

// Inspired by this:
// https://arstechnica.com/gadgets/2021/07/google-pushed-a-one-character-typo-to-production-bricking-chrome-os-devices/

#include "MutatorBase.h"

class MutatorAndBitwiseAnd : public MutatorBase{
public:
    size_t CheckMutationsPossible(const std::string &line) override;
    std::string MutateLine(const std::string &line, size_t mutationnr = 0) override;
};


#endif //DUMBMUTATE_MUTATORANDBITWISEAND_H
