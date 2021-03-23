//
// Created by Christoffer Wärnbring on 2019-02-26.
//

#ifndef DUMBMUTATE_MUTATORNEQUAL_H
#define DUMBMUTATE_MUTATORNEQUAL_H

#include "MutatorBase.h"

class MutatorNEqual: public MutatorBase{
public:
	size_t CheckMutationsPossible(const std::string &line) override;
	std::string MutateLine(const std::string &line, size_t mutationnr = 0) override;
};


#endif //DUMBMUTATE_MUTATORNEQUAL_H
