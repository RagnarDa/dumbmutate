//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#include <iostream>
#include <fstream>
#include <string>
#include "HTMLExporter.h"


std::string ReplaceAll(std::string Original, std::string ToReplace, std::string ReplaceWith)
{
	std::string Copy = Original;
	auto pos = Copy.find(ToReplace);
	while (Copy.size() > pos) {
		Copy.replace(pos, ToReplace.size(), ReplaceWith);
		pos = Copy.find(ToReplace,pos+1);
	}
	return Copy;
}

std::string Sanitize(std::string Original)
{
	return ReplaceAll(ReplaceAll(ReplaceAll(ReplaceAll(Original,"&", "&#38;"), ">", "&#62;"), "\"", "&#34;"), "<","&#60;");
}

void HTMLExporter::WriteHTML(std::string HTMLFileName,
                            std::vector<std::pair<std::string, SourceFile::MutationResult>> Results) {
	std::ofstream out(HTMLFileName);
	out << "<html><body>" << std::endl;

	for (unsigned int linenr = 0; linenr < Results.size(); ++linenr)
	{
		//<span><p style="background-color: red; margin-top: 0.0em; margin-bottom: 0em;">code</p></span>

		out << "<pre><p style=\"background-color:  ";
		switch (Results.at(linenr).second)
		{
			case SourceFile::NoMutation:
			{
				out << "white";
			}
			break;
			case SourceFile::FailedCompile:
			{
				out << "yellow";
			}
			break;
			case SourceFile::FailedTest:
			{
				out << "green";
			}
			break;
			case SourceFile::Survived:
			{
				out << "red";
			}
			break;
		}
		out << "; margin-top: 0.0em; margin-bottom: 0em;\">" << linenr << "\t";
		out << Sanitize(Results.at(linenr).first);
		//out << Results.at(linenr).first;
		out << "</p></pre>" << std::endl;
	}
	out.close();
}
