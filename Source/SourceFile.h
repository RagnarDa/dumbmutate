//
// Created by Christoffer Wärnbring on 2019-02-23.
//

#ifndef DUMBMUTATE_SOURCEFILE_H
#define DUMBMUTATE_SOURCEFILE_H
#include <string>
#include <vector>

class SourceFile {
public:
	enum MutationResult
	{
		NoMutation = 0,
		FailedBuild = 1,
		FailedTest = 2,
		Survived = 3
	};
	void Revert();
	void Modify(size_t LineNr, std::string NewLine);
	void SaveModification(MutationResult result);
	void SaveModification(size_t LineNr, MutationResult Result);
	void WriteModification();

	virtual ~SourceFile();

	void WriteOriginal();

	explicit SourceFile(std::string FilePathToLoad);
	size_t LineCount() {return linecount;};
	std::string GetLine(size_t linenr){return Original.at(linenr);};
	bool GetIsMultilineComment(size_t linenr){return IsMultiLineComment.at(linenr);};

	const std::vector<std::pair<std::string, MutationResult>> &GetSaved() const;

private:
	std::vector<std::string> Original;
	std::vector<std::string> Modified;
	std::vector<std::pair<std::string,MutationResult>> Saved;
	std::vector<bool> IsMultiLineComment;
	size_t LatestModifiedLine = 0;
	std::string FilePath;
	size_t linecount = 0;
	void WriteFile(std::vector<std::string> &vector) const;
};


#endif //DUMBMUTATE_SOURCEFILE_H
