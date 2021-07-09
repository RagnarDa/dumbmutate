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
		if (SearchString(line, nr, 0) != std::string::npos)
		{
			if (occurencies == mutationnr)
			{
				std::string rtrn = line;
                unsigned long pos = SearchString(line, nr, 0);
                const char *nrstring = nr.c_str();
                int i = std::atoi(nrstring); // NOLINT(cert-err34-c)
                int shift = NumShift(i);
                const std::string &toString = std::to_string(shift);
                auto shifted = toString.c_str();
                return rtrn.replace(pos, 1, shifted);
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
		if (SearchString(line,nr,0) != std::string::npos)
		{
			occurencies++;
		}
	}
	return occurencies;
}