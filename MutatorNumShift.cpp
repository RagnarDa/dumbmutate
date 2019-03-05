//
// Created by Christoffer Wärnbring on 2019-03-01.
//

#include "MutatorNumShift.h"

// I think it will be enough to just shift once per number since this would take
// forever otherwise

std::string MutatorNumShift::MutateLine(const std::string &line, size_t mutationnr) {
	size_t occurencies = 0;
	for (const auto & nr : numbers)
	{
		if (line.find(nr) != std::string::npos)
		{
			if (occurencies == mutationnr)
			{
				std::string rtrn = line;
				return rtrn.replace(line.find(nr),1,std::to_string(NumShift(std::atoi(nr.c_str()))).c_str()); // NOLINT(cert-err34-c)
			}
			occurencies++;
		}
	}
	throw std::exception();
}

size_t MutatorNumShift::CheckMutationsPossible(const std::string &line) {
	size_t occurencies = 0;
	for (const auto & nr : numbers)
	{
		if (line.find(nr) != std::string::npos)
		{
			occurencies++;
		}
	}
	return occurencies;
}