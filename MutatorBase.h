//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#ifndef DUMBMUTATE_MUTATORBASE_H
#define DUMBMUTATE_MUTATORBASE_H
#include <string>

class MutatorBase {
public:
	virtual size_t CheckMutationsPossible(const std::string &line) = 0;
	virtual std::string MutateLine(const std::string &line, size_t mutationnr = 0) = 0;

protected:
	static size_t CountOccurences(std::string StringToSearch, std::string TargetString);
	static size_t GetOccurence(std::string StringToSearch, std::string TargetString, size_t Occurence);
	std::string DuoMutation(const std::string &line, size_t mutationnr, const char *LeftSymbol,
	                        const char *RightSymbol) const;

};


#endif //DUMBMUTATE_MUTATORBASE_H
