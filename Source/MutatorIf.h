//
// Created by Christoffer Wärnbring on 2021-04-27.
//

#ifndef DUMBMUTATE_MUTATORIF_H
#define DUMBMUTATE_MUTATORIF_H
#include "MutatorBase.h"

class MutatorIf :public MutatorBase{
public:
    size_t CheckMutationsPossible(const std::string &line) override;
    std::string MutateLine(const std::string &line, size_t mutationnr = 0) override;
};


#endif //DUMBMUTATE_MUTATORIF_H
