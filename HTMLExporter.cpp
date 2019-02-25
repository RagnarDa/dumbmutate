//
// Created by Christoffer Wärnbring on 2019-02-24.
//

#include <iostream>
#include <fstream>
#include "HTMLExporter.h"

void HTMLExporter::WriteHTML(std::string HTMLFileName,
                            std::vector<std::pair<std::string, SourceFile::MutationResult>> Results) {
	std::ofstream out(HTMLFileName);
	out << "<html><body>" << std::endl;

	for (unsigned int linenr = 0; linenr < Results.size(); ++linenr)
	{
		out << "<br><pre>" << linenr+1 << "\t";
		switch (Results.at(linenr).second)
		{
			case SourceFile::NoMutation:
			{
				out << "" << Results.at(linenr).first << "";
			}
			break;
			case SourceFile::FailedCompile:
			{
				out << "<font color=\"yellow\">" << Results.at(linenr).first << "</font>";
			}
			break;
			case SourceFile::FailedTest:
			{
				out << "<font color=\"green\">" << Results.at(linenr).first << "</font>";
			}
			break;
			case SourceFile::Survived:
			{
				out << "<<font color=\"red\">>" << Results.at(linenr).first << "</font>";
			}
			break;
		}
		out << "</pre>" << std::endl;
	}
	out.close();
}
