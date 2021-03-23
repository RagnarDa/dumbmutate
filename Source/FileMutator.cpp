//
// Created by Christoffer Wärnbring on 2019-02-21.
//

#include "FileMutator.h"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int FileMutator::MutateFile(int Occurence, std::string InFile, std::string OutFile, std::string ToReplace, std::string ReplaceWith)
{
	ifstream in(InFile);
	ofstream out(OutFile);

	if (!in)
	{
		cerr << "Could not open " << InFile << "\n";
		return -1;
	}

	if (!out)
	{
		cerr << "Could not open " << OutFile << "\n";
		return -1;
	}
	int Count = -1;
	int LineNr = -1;
	int LineReplacedOn = -1;
	string line;
	size_t len = ToReplace.length();
	while (getline(in, line))
	{
		LineNr++;

		size_t pos = line.find(ToReplace);
		if (pos != string::npos) {
			if (++Count == Occurence) {
				line.replace(pos, len, ReplaceWith);
				LineReplacedOn = LineNr;
			};
		}

		out << line << '\n';
	}
	return LineReplacedOn+1;
}