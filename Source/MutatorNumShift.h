//
// Created by Christoffer Wärnbring on 2019-03-01.
//

#ifndef DUMBMUTATE_MUTATORNUMSHIFT_H
#define DUMBMUTATE_MUTATORNUMSHIFT_H

#include "MutatorBase.h"

class MutatorNumShift : public MutatorBase{
public:
	size_t CheckMutationsPossible(const std::string &line) override;
	std::string MutateLine(const std::string &line, size_t mutationnr) override;
};



#endif //DUMBMUTATE_MUTATORNUMSHIFT_H
