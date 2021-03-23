//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#ifndef DUMBMUTATE_MUTATORCARET_H
#define DUMBMUTATE_MUTATORCARET_H
#include "MutatorBase.h"

class MutatorCaret :public MutatorBase{
public:
	size_t CheckMutationsPossible(const std::string &line) override;
	std::string MutateLine(const std::string &line, size_t mutationnr = 0) override;

};


#endif //DUMBMUTATE_MUTATORCARET_H
