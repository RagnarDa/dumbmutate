//
// Created by Christoffer Wärnbring on 2019-02-21.
//

#ifndef DUMBMUTATE_FILEMUTATOR_H
#define DUMBMUTATE_FILEMUTATOR_H
#include <string>

class FileMutator {
public:
	static int MutateFile(int Occurence, std::string InFile, std::string OutFile, std::string ToReplace, std::string ReplaceWith);
};


#endif //DUMBMUTATE_FILEMUTATOR_H
