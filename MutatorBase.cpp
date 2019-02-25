//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#include "MutatorCaret.h"
#include "MutatorBase.h"

size_t MutatorBase::CountOccurences(std::string StringToSearch, std::string TargetString) {
	int occurrences = 0;
	std::string::size_type pos = 0;
	while ((pos = StringToSearch.find(TargetString, pos )) != std::string::npos) {
		++occurrences;
		pos += TargetString.length();
	}
	return occurrences;
}

size_t MutatorBase::GetOccurence(std::string StringToSearch, std::string TargetString, size_t Occurence) {
	int occurrences = 0;
	std::string::size_type pos = 0;
	while ((pos = StringToSearch.find(TargetString, pos )) != std::string::npos
	&& occurrences < Occurence) {
		++occurrences;
		pos += TargetString.length();
	}
	return pos;
}

std::string
MutatorBase::DuoMutation(const std::string &line, size_t mutationnr, const char *LeftSymbol, const char *RightSymbol) const {
	const size_t LeftOccurencies = CountOccurences(line, LeftSymbol);
	std::string rtrn = line;
	if (mutationnr < LeftOccurencies)
	{
		return rtrn.replace(GetOccurence(line, LeftSymbol, mutationnr), 1, RightSymbol);
	} else {
		return rtrn.replace(GetOccurence(line, RightSymbol, mutationnr - LeftOccurencies), 1, LeftSymbol);
	}
}