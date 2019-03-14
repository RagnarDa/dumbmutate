#include <iostream>
#include "cxxopts.hpp"
#include "SourceFile.h"
#include "Utility.h"
#include "HTMLExporter.h"
#include "MutatorAddSubtr.h"
#include "MutatorCaret.h"
#include "MutatorEqual.h"
#include "MutatorNEqual.h"
#include "MutatorIncrDecr.h"
#include "MutatorNumShift.h"
#include "MutatorAndOr.h"
#include "MutatorTrueFalse.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <cassert>

bool Compile();

bool Test();

std::stringstream
Summary(std::chrono::time_point<std::chrono::system_clock> timepoint_start,
        size_t linesdone, size_t linestotal, size_t mutations, size_t compilefailes, size_t testfailes,
        size_t survived);

size_t KillRatioPerc(size_t testfailes, size_t survived);

#include <cstdio>
#include <cstdlib>

cxxopts::ParseResult ParseCommandLine(int argc, char* argv[])
{
	try
	{
		cxxopts::Options options(argv[0], " - example command line options");
		options
				.positional_help("[optional args]")
				.show_positional_help();

		options
				.allow_unrecognised_options()
				.add_options()
						("f, file", "File to mutate", cxxopts::value<std::string>(), "FILE")
						("c, compile", "Compile command", cxxopts::value<std::string>()->default_value("make"))
						("t, test", "Test command", cxxopts::value<std::string>()
						        ->default_value("./test"))
						("k, threshold", "Killratio threshold (%)", cxxopts::value<size_t>()->default_value("0"), "N")
						/*("positional",
						 "Positional arguments: these are the arguments that are entered "
						 "without an option", cxxopts::value<std::vector<std::string>>())
						("long-description",
						 "thisisareallylongwordthattakesupthewholelineandcannotbebrokenataspace")
						("help", "Print help")
						("int", "An integer", cxxopts::value<int>(), "N")
						("float", "A floating point number", cxxopts::value<float>())
						("option_that_is_too_long_for_the_help", "A very long option")
#ifdef CXXOPTS_USE_UNICODE
			("unicode", u8"A help option with non-ascii: à. Here the size of the"
        " string should be correct")
#endif
				*/;


		auto result = options.parse(argc, argv);

		return result;

	} catch (const cxxopts::OptionException& e)
	{
		std::cout << "error parsing options: " << e.what() << std::endl;
		exit(1);
	}
}

std::string testcommand;
std::string compilecommand;
int main(int argc, char* argv[])
{
	std::chrono::duration<double, std::ratio<1,1>> compiletime = std::chrono::duration<double>(600*5);
	std::chrono::duration<double, std::ratio<1,1>> testtime = std::chrono::duration<double>(600*5);
	size_t threshold = 0;
	std::string filepath;
	try {
		auto result = ParseCommandLine(argc, argv);
		filepath = result["file"].as<std::string>();
		compilecommand = result["compile"].as<std::string>();
		testcommand = result["test"].as<std::string>();
		threshold = result["threshold"].as<size_t>();
	} catch (std::exception & e){
		std::cout << "How to use:" << std::endl;
		std::cout << R"(dumbmutate --file="filetotest.cpp" --compile="make" --test="./test")" << std::endl;
		std::cout << "Optionally for a 80% killratio threshold:" << std::endl;
		std::cout << R"(dumbmutate --file="filetotest.cpp" --compile="make" --test="./test --threshold=80")" << std::endl;

		return 1;
	}
	auto start = std::chrono::system_clock::now();
	SourceFile file(filepath);
	if (!Compile()) {
		std::cerr << "Unmodified compile failed. Fix your program.";
		exit(2);
	};
	auto end = std::chrono::system_clock::now();
	compiletime = end-start;
	std::cout << "Nominal compile-time: " << compiletime.count() << std::endl;
	start = std::chrono::system_clock::now();
	if (!Test()) {
		std::cerr << "Unmodified test failed. Fix your program.";
		exit(3);
	}
	end = std::chrono::system_clock::now();
	testtime = end-start;
	std::cout << "Nominal test-time: " << testtime.count() << std::endl;

	// TODO: Mutations as options.

	size_t mutations = 0, compilefailes = 0, testfailes = 0, survived = 0;

	std::vector<MutatorBase*> Mutations({
		  new MutatorEqual()
		, new MutatorAddSubtr()
		, new MutatorIncrDecr()
		, new MutatorCaret()
		, new MutatorNEqual()
		, new MutatorNumShift()
		, new MutatorAndOr()
		, new MutatorTrueFalse()});

	std::string HTMLFileName = std::string("./MutationResult_" + ReplaceAll(ReplaceAll(filepath,"/","_"),"\\","_") + ".html");
	for (size_t i = 0; i < file.LineCount(); ++i) {
		double percentagedone = (double)i/(double)file.LineCount();
		std::chrono::duration<double,std::ratio<1,1>> timeelapsed = std::chrono::system_clock::now() - end;
		std::chrono::duration<double,std::ratio<1,1>> timeremaining = (timeelapsed / percentagedone)-timeelapsed;
		std::cout << std::endl << std::endl << "**********************************************" << std::endl;
		std::cout << "Mutation progress:" << std::endl;
		std::cout << i << " of " << file.LineCount() << " lines processed in " << std::ceil(timeelapsed.count()) << " seconds." << std::endl;
		std::cout << "Approximately " << (int)(percentagedone*100) << "% done, " << std::ceil(timeremaining.count()/60) << " minutes left." << std::endl;
		std::cout << "**********************************************" << std::endl << std::endl;
		const std::string &line = file.GetLine(i);
		SourceFile::MutationResult result = SourceFile::NoMutation;
		if (!file.GetIsMultilineComment(i)) {
			for (auto &mutator : Mutations) {
				const size_t mutationsPossible = mutator->CheckMutationsPossible(line);
				for (size_t j = 0; j < mutationsPossible/* && result < SourceFile::MutationResult::Survived*/; ++j) {
					std::cout << "Original:" << std::endl;
					std::cout << i << ": " << line << std::endl;
					std::cout << "Mutation:" << std::endl;
					const std::string &mutatedline = mutator->MutateLine(line, j);
					std::cout << i << ": " << mutatedline << std::endl;
					assert(line != mutatedline);
					file.Modify(i, mutatedline);
					mutations++;
					file.WriteModification();
					if (Compile()) {
						if (Test()) {
							result = SourceFile::MutationResult::Survived;
							file.SaveModification(result);
							std::cout << "Survived." << std::endl;
							survived++;
						} else {
							testfailes++;
							std::cout << "Test failed." << std::endl;
							if (result < SourceFile::MutationResult::FailedTest) {
								result = SourceFile::MutationResult::FailedTest;
								file.SaveModification(result);
							}
						}
					} else {
						compilefailes++;
						std::cout << "Compile failed." << std::endl;
						if (result < SourceFile::MutationResult::FailedCompile) {
							result = SourceFile::MutationResult::FailedCompile;
							file.SaveModification(result);
						}
					}
					file.Revert();
				}
			}
			if (result != SourceFile::MutationResult::NoMutation) {
				HTMLExporter::WriteHTML(HTMLFileName, file.GetSaved(), Summary(end,
				                                                               i + 1, file.LineCount(), mutations,
				                                                               compilefailes, testfailes,
				                                                               survived).str());
				file.Revert();
			};
		} else {
			std::cout << line << std::endl;
			std::cout << "Multiline comment, ignoring." << std::endl;

		}
	}
	HTMLExporter::WriteHTML(HTMLFileName, file.GetSaved(), Summary(end,
	                                                               file.LineCount(), file.LineCount(), mutations,
	                                                               compilefailes, testfailes,
	                                                               survived).str());
	std::cout << std::endl << std::endl;
	std::cout << "The following mutations survived: " << std::endl;
	for (size_t i = 0; i < file.LineCount(); ++i) {
		if (file.GetSaved().at(i).second == SourceFile::MutationResult::Survived)
		{
			std::cout << filepath << ":" << (i+1) << " " << file.GetSaved().at(i).first << std::endl;
		}
	}
	std::cout << Summary(end,
	                     file.LineCount(), file.LineCount(), mutations, compilefailes, testfailes, survived).str();

	for (auto &Mutation : Mutations) {
		delete Mutation;
	}
	Mutations.clear();
	if (threshold == 0) {
		return static_cast<int>(survived);
	} else {
		return KillRatioPerc(testfailes, survived) < threshold ? 1:0;
	}
}

std::stringstream
Summary(const std::chrono::time_point<std::chrono::system_clock> timepoint_start,
        size_t linesdone, size_t linestotal, size_t mutations, size_t compilefailes, size_t testfailes,
        size_t survived) {
	std::chrono::duration<double, std::ratio<1,1>> totaltime = std::chrono::system_clock::now() - timepoint_start;
	std::stringstream s;
	std::time_t timev = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	size_t killratio = KillRatioPerc(testfailes, survived);
	s << "\n" << "\n";
	s << "-----------------------------" << "\n";
	s << /*"Time now: " << "\n" << */std::ctime(&timev);// << "\n";
	s << "Time passed: " << (int)totaltime.count()/60 << " minutes" << "\n";
	s << "Lines processed: " << linesdone << " of " << linestotal << "\n";
	s << "Mutations: " << mutations << "\n";
	s << "Compile failed: " << compilefailes << "\n";
	s << "Test failed: " << testfailes << "\n";
	s << "Mutations survived: " << survived << "\n";
	s << "Mutation killratio: " << killratio << "%\n";
	s << "-----------------------------" << "\n";
	return s;
}

size_t KillRatioPerc(size_t testfailes, size_t survived) {
	size_t killratio = 0;
	if  ((signed int)((signed int)testfailes - (signed int) survived) > (signed int) 0) {
		killratio = static_cast<size_t>(100.0f * (((testfailes-survived) * 100.0f) / (testfailes * 100.0f)));
	}
	return killratio;
}


bool RunCommand(const char * command)
{
	return (system(command) == 0);
}

bool Test() {
	std::cout << "Testing..." << std::endl;
	return RunCommand(testcommand.c_str());
}

bool Compile() {
	std::cout << "Compiling..." << std::endl;
	return RunCommand(compilecommand.c_str());
}
