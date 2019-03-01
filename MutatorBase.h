//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#ifndef DUMBMUTATE_MUTATORBASE_H
#define DUMBMUTATE_MUTATORBASE_H
#include <string>
#include <vector>

class MutatorBase {
public:
	virtual size_t CheckMutationsPossible(const std::string &line) = 0;
	virtual std::string MutateLine(const std::string &line, size_t mutationnr) = 0;

protected:
	static size_t CountOccurences(std::string StringToSearch, std::string TargetString);
	static size_t CountOccurences(std::string StringToSearch, std::vector<std::string> TargetStrings);
	static size_t GetOccurence(std::string StringToSearch, std::string TargetString, size_t Occurence);
	std::string DuoMutation(const std::string &line, size_t mutationnr, std::string LeftSymbol,
	                        std::string RightSymbol) const;
	static int NumShift(int nr);
	const std::vector<std::string> numbers = {"0","1","2","3","4","5","6","7","8","9"};

};


#endif //DUMBMUTATE_MUTATORBASE_H
