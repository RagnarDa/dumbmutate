#include <iostream>
#include "cxxopts.hpp"
#include "SourceFile.h"
#include "HTMLExporter.h"
#include "MutatorAddSubtr.h"
#include "MutatorCaret.h"
#include "MutatorEqual.h"
#include "MutatorNEqual.h"
#include "MutatorDoubleAddSubtr.h"
#include "MutatorNumShift.h"
#include "MutatorAndOr.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>

bool Compile();

bool Test();

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
//std::chrono::duration<double, std::ratio<1,1>> compiletime = std::chrono::duration<double>(600*5);
//std::chrono::duration<double, std::ratio<1,1>> testtime = std::chrono::duration<double>(600*5);
int main(int argc, char* argv[])
{
	std::string filepath;
	try {
		auto result = ParseCommandLine(argc, argv);
		filepath = result["file"].as<std::string>();
		compilecommand = result["compile"].as<std::string>();
		testcommand = result["test"].as<std::string>();
	} catch (std::exception & e){
		std::cout << "How to use:" << std::endl;
		std::cout << R"(dumbmutate --file="filetotest.cpp" --compile="make" --test="./test")" << std::endl;
		return 1;
	}
	auto start = std::chrono::system_clock::now();
	SourceFile file(filepath);
	if (!Compile()) {
		std::cerr << "Unmodified compile failed. Fix your program.";
		exit(2);
	};
	auto end = std::chrono::system_clock::now();
//	compiletime = end-start;
//	std::cout << "Nominal compile-time: " << compiletime.count() << std::endl;
	start = std::chrono::system_clock::now();
	if (!Test()) {
		std::cerr << "Unmodified test failed. Fix your program.";
		exit(3);
	}
	end = std::chrono::system_clock::now();
//	testtime = end-start;
//	std::cout << "Nominal test-time: " << testtime.count() << std::endl;

	// TODO: More mutations
	// TODO: Mutations as options.

	size_t mutations = 0, compilefailes = 0, testfailes = 0, survived = 0;

	std::vector<MutatorBase*> Mutations({
		  new MutatorEqual()
		, new MutatorAddSubtr()
		, new MutatorDoubleAddSubtr()
		, new MutatorCaret()
		, new MutatorNEqual()
		, new MutatorNumShift()
		, new MutatorAndOr()});

	for (size_t i = 0; i < file.LineCount(); ++i) {
		double percentagedone = (double)i/(double)file.LineCount();
		std::chrono::duration<double,std::ratio<1,1>> timeelapsed = std::chrono::system_clock::now() - end;
		//double timeelapseds = timeelapsed.count()/(1000.0*1000.0);
		std::chrono::duration<double,std::ratio<1,1>> timeremaining = (timeelapsed / percentagedone)-timeelapsed;
		//int timeremainings = (timeelapseds / percentagedone)-timeelapseds;
		std::cout << std::endl << std::endl << "**********************************************" << std::endl;
		std::cout << "Mutation progress:" << std::endl;
		std::cout << i << " of " << file.LineCount() << " lines processed in " << std::ceil(timeelapsed.count()) << " seconds." << std::endl;
		std::cout << "Approximately " << (int)(percentagedone*100) << "% done, " << std::ceil(timeremaining.count()/60) << " minutes left." << std::endl;
		std::cout << "**********************************************" << std::endl << std::endl;
		const std::string &line = file.GetLine(i);
		SourceFile::MutationResult result = SourceFile::NoMutation;
		for (auto &mutator : Mutations) {
			const size_t mutationsPossible = mutator->CheckMutationsPossible(line);
			for (size_t j = 0; j < mutationsPossible && result < SourceFile::MutationResult::Survived; ++j) {
				std::cout << "Original:" << std::endl;
				std::cout << i << ": " << line << std::endl;
				std::cout << "Mutation:" << std::endl;
				const std::string &mutatedline = mutator->MutateLine(line, j);
				assert(line != mutatedline);
				std::cout << i << ": " << mutatedline << std::endl;
				file.Modify(i, mutatedline);
				mutations++;
				file.WriteModification();
				if (Compile()) {
					if (Test()) {
						result = SourceFile::MutationResult::Survived;
						std::cout << "Survived." << std::endl;
						survived++;
						break;
					} else {
						testfailes++;
						std::cout << "Test failed." << std::endl;
						if (result < SourceFile::MutationResult::FailedTest) {
							result = SourceFile::MutationResult::FailedTest;
						}
					}
				} else {
					compilefailes++;
					std::cout << "Compile failed." << std::endl;
					if (result < SourceFile::MutationResult::FailedCompile) {
						result = SourceFile::MutationResult::FailedCompile;
					}
				}
				file.Revert();
			}
			if (result == SourceFile::MutationResult::Survived)
			{
				break;
			}
		}
		if (result != SourceFile::MutationResult::NoMutation) {
			file.SaveModification(result);
			HTMLExporter::WriteHTML("./MutationResult.html", file.GetSaved());
			file.Revert();
		};
	}

	std::cout << std::endl << std::endl;
	std::cout << "The following mutations survived: " << std::endl;
	for (size_t i = 0; i < file.LineCount(); ++i) {
		if (file.GetSaved().at(i).second == SourceFile::MutationResult::Survived)
		{
			std::cout << filepath << ":" << (i+1) << " " << file.GetSaved().at(i).first << std::endl;
		}
	}

	std::chrono::duration<double,std::ratio<1,1>> totaltime = std::chrono::system_clock::now()-end;

	std::cout << std::endl << std::endl;
	std::cout << "----------------------" << std::endl;
	std::cout << "Time: " << (int)totaltime.count()/60 << " minutes" << std::endl;
	std::cout << "Lines: " << file.LineCount() << std::endl;
	std::cout << "Mutations: " << mutations << std::endl;
	std::cout << "Compile failed: " << compilefailes << std::endl;
	std::cout << "Test failed: " << testfailes << std::endl;
	std::cout << "Mutations survived: " << survived << std::endl;
	std::cout << "----------------------" << std::endl;

	for (auto &Mutation : Mutations) {
		delete Mutation;
	}
	Mutations.clear();

	return static_cast<int>(survived);
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
