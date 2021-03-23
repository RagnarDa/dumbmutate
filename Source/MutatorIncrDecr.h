//
// Created by Christoffer Wärnbring on 2019-03-01.
//

#ifndef DUMBMUTATE_MUTATORDOUBLEADDSUBTR_H
#define DUMBMUTATE_MUTATORDOUBLEADDSUBTR_H

#include "MutatorBase.h"

class MutatorIncrDecr : public MutatorBase{
public:
	size_t CheckMutationsPossible(const std::string &line) override;
	std::string MutateLine(const std::string &line, size_t mutationnr) override;
};


#endif //DUMBMUTATE_MUTATORDOUBLEADDSUBTR_H
