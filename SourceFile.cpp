//
// Created by Christoffer Wärnbring on 2019-02-23.
//

#include "SourceFile.h"
#include <fstream>
#include <iostream>

SourceFile::SourceFile(std::string FilePathToLoad) : FilePath(FilePathToLoad) {
	std::ifstream in(FilePath);
	if (!in)
	{
		std::cerr << "Couldn't load the specified source file.";
		throw in.exceptions();
	}
	std::string line;
	while (getline(in, line))
	{
		this->Original.emplace_back(line);
		this->Saved.emplace_back(std::pair<std::string, MutationResult>(line, NoMutation));
		linecount++;
	}
	this->Revert();
}

void SourceFile::Revert() {
	this->Modified.clear();
	assert(this->Modified.empty());
	for (auto line : this->Original)
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

