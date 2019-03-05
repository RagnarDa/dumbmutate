//
// Created by Christoffer Wärnbring on 2019-03-04.
//

#ifndef DUMBMUTATE_MUTATORANDOR_H
#define DUMBMUTATE_MUTATORANDOR_H

#include "MutatorBase.h"

class MutatorAndOr : public MutatorBase{
public:
	size_t CheckMutationsPossible(const std::string &line) override;
	std::string MutateLine(const std::string &line, size_t mutationnr) override;
};


#endif //DUMBMUTATE_MUTATORANDOR_H
