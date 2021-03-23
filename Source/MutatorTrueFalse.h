//
// Created by Christoffer Wärnbring on 2019-03-05.
//

#ifndef DUMBMUTATE_MUTATORTRUEFALSE_H
#define DUMBMUTATE_MUTATORTRUEFALSE_H

#include "MutatorBase.h"

class MutatorTrueFalse : public MutatorBase{
public:
	size_t CheckMutationsPossible(const std::string &line) override;
	std::string MutateLine(const std::string &line, size_t mutationnr) override;
};


#endif //DUMBMUTATE_MUTATORTRUEFALSE_H
