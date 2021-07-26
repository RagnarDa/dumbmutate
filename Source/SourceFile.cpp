#include <utility>

//
// Created by Christoffer Wärnbring on 2019-02-23.
//

#include "SourceFile.h"
#include <fstream>
#include <iostream>
#include <cassert>

SourceFile::SourceFile(std::string FilePathToLoad) : FilePath(std::move(FilePathToLoad)) {
	std::ifstream in(FilePath);
	if (!in)
	{
		std::cerr << "Couldn't load the specified source file: " << FilePath << std::endl;
		exit(1);
	}
	std::string line;
	linecount = 0;
	bool ismultilinecomment = false;
	while (getline(in, line))
	{
		this->Original.emplace_back(line);
		this->Saved.emplace_back(std::pair<std::string, MutationResult>(line, NoMutation));
		linecount++;
		if (line.find("/*") != std::string::npos &&
				((line.find("*/") == std::string::npos)
			|| (line.find("/*") > line.find("*/") && line.find("*/") != std::string::npos)))
		{
			ismultilinecomment = true;
			this->IsMultiLineComment.push_back(true);
		} else if (line.find("*/") != std::string::npos)
		{
			ismultilinecomment = false;
			this->IsMultiLineComment.push_back(true);
		} else {
			this->IsMultiLineComment.push_back(ismultilinecomment);
		}
	}
	this->Revert();
}

void SourceFile::Revert() {
	this->Modified.clear();
	assert(this->Modified.empty());
	for (const auto& line : this->Original)
	{
		this->Modified.emplace_back(line);
	}
}

void SourceFile::Modify(size_t LineNr, std::string NewLine) {
	this->Modified.at(LineNr) = std::move(NewLine);
	this->LatestModifiedLine = LineNr;
}

void SourceFile::SaveModification(size_t LineNr, MutationResult Result) {
	this->Saved.at(LineNr) = std::pair<std::string, MutationResult>(this->Modified.at(LineNr), Result);
}

void SourceFile::SaveModification(MutationResult result) {
	SaveModification(LatestModifiedLine, result);
}

void SourceFile::WriteOriginal() {
	WriteFile(Original);
}

void SourceFile::WriteFile(std::vector<std::string> &vector) const {
	std::ofstream out(FilePath);
	if (!out.is_open()) {
		std::cerr << "Can't write to file " << FilePath << std::endl;
		out.close();
		// Don't think we can recover from this one...
		exit(EXIT_FAILURE);
//		return;
//		throw FileError("Failed to open file \"" + name + "\"");
	}
	for (const auto &line : vector)
	{
		out << line << std::endl;
	}
	out.close();
}

void SourceFile::WriteModification() {
	WriteFile(Modified);
}

SourceFile::~SourceFile() {
	// Just to be safe
	WriteOriginal();
}

const std::vector<std::pair<std::string, SourceFile::MutationResult>> &SourceFile::GetSaved() const {
	return Saved;
}

SourceFile::SourceFile() {

}

