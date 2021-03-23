//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#ifndef DUMBMUTATE_MUTATORADDSUBTR_H
#define DUMBMUTATE_MUTATORADDSUBTR_H


#include "MutatorBase.h"

class MutatorAddSubtr : public MutatorBase{
public:
	size_t CheckMutationsPossible(const std::string &line) override;
	std::string MutateLine(const std::string &line, size_t mutationnr = 0) override;
};


#endif //DUMBMUTATE_MUTATORADDSUBTR_H
