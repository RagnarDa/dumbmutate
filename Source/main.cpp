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
#include "MutatorIf.h"
#include "CommandRunner.h"
#include "MutatorXYZ.h"
#include <chrono>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <csignal>

bool Build(int timeoutms);

bool Test(int timeoutms);

std::stringstream
Summary(std::chrono::time_point<std::chrono::system_clock> timepoint_start,
        size_t linesdone, size_t linestotal, size_t mutations, size_t buildfailes, size_t testfailes,
        size_t survived);

size_t KillRatioPerc(size_t testfailes, size_t survived);


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
						("m, mutate", "File to mutate", cxxopts::value<std::string>(), "FILE")
						("b, build", "Build command", cxxopts::value<std::string>()->default_value("make"))
						("t, test", "Test command", cxxopts::value<std::string>()
						        ->default_value("./test"))
						("k, threshold", "Killratio threshold (%)", cxxopts::value<size_t>()->default_value("0"), "N")
                        ("s, start", "Starting line", cxxopts::value<int>()->default_value("1"),"N")
                        ("e, end", "Ending line", cxxopts::value<int>()->default_value("-1"),"N")
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


SourceFile file;

void signalHandler( int signum ) {
    std::cout << "Interrupt signal (" << signum << ") received. Restoring file.\n";

    file.WriteOriginal();

    exit(signum);
}


// How many times longer a command is allowed to run than the initial case
const int testtimeoutmodifier = 10;
const int buildtimeoutmodifier = 10;

size_t timeoutstest = 0;
size_t timeoutsbuild = 0;
std::string testcommand;
std::string buildcommand;
std::chrono::duration<double, std::ratio<1,1>> buildtime = std::chrono::duration<double>(600 * 5);
std::chrono::duration<double, std::ratio<1,1>> testtime = std::chrono::duration<double>(600*5);
int main(int argc, char* argv[])
{
	size_t threshold;
	int startingline;
	int endingline;
	std::string filepath;
	try {
		auto result = ParseCommandLine(argc, argv);
		filepath = result["mutate"].as<std::string>();
        buildcommand = result["build"].as<std::string>();
		testcommand = result["test"].as<std::string>();
		threshold = result["threshold"].as<size_t>();
		startingline = result["start"].as<int>() - 1;
		endingline = result["end"].as<int>() - 1;
	} catch (...) {
		std::cout << "How to use:" << std::endl;
		std::cout << R"(dumbmutate --mutate="filetotest.cpp" --build="make" --test="./test")" << std::endl;
		std::cout << "Optionally for a 80% killratio threshold:" << std::endl;
		std::cout << R"(dumbmutate --mutate="filetotest.cpp" --build="make" --test="./test --threshold=80")" << std::endl;
        std::cout << "Optionally for mutating lines 80 to 110 of file:" << std::endl;
        std::cout << R"(dumbmutate --mutate="filetotest.cpp" --build="make" --test="./test --start=80 --end=110")" << std::endl;

		return 1;
	}
	auto start = std::chrono::system_clock::now();
    auto beginning = std::chrono::system_clock::now();
	file = SourceFile(filepath);
	file.WriteOriginal(); // Hopefully will trigger a rebuild

	// Register signals so we can restore the file if exiting prematurely
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGABRT, signalHandler);

    if (!Build(0)) {
		std::cerr << "Unmodified build failed. Fix your program.";
		exit(2);
	}
	auto end = std::chrono::system_clock::now();
    buildtime = end - start;
	//std::cout << "Nominal build-time: " << buildtime.count() << std::endl;
	start = std::chrono::system_clock::now();
	if (!Test(0)) {
		std::cerr << "Unmodified test failed. Fix your program.";
		exit(3);
	}
	end = std::chrono::system_clock::now();
	testtime = end-start;
	//std::cout << "Nominal test-time: " << testtime.count() << std::endl;

	// TODO: Mutations as options.

	size_t mutations = 0, buildfailes = 0, testfailes = 0, survived = 0;

	std::vector<MutatorBase*> Mutations({
		  new MutatorEqual()
		, new MutatorAddSubtr()
		, new MutatorIncrDecr()
		//, new MutatorCaret() // HACK! Don't use this for now, most of the time it causes build error
		, new MutatorNEqual()
		, new MutatorNumShift()
		, new MutatorAndOr()
		, new MutatorTrueFalse()
	    , new MutatorIf()
	    , new MutatorXYZ()});

	std::string HTMLFileName = std::string("./MutationResult_" + ReplaceAll(ReplaceAll(ReplaceAll(ReplaceAll(filepath,"/","_"),"\\","_"), ":", ""), "~", "") + ".html");
	if (endingline < 0)
    {
	    endingline = (int)file.LineCount();
    } else if (endingline > (int)file.LineCount())
    {
	    std::cerr << "Ending line past end of file." << std::endl;
	    exit(4);
    } else if (startingline > (int)file.LineCount())
    {
	    std::cerr << "Starting line past end of file." << std::endl;
	    exit(5);
    }
	size_t linecount = endingline-startingline;

	// Let's ignore lines where mutations will likely fail on compile
	// but take long time
	const std::string ignorelineswithkeyword[] =
	{
        "template",
        "constexpr",
        "static_assert",
        "using",
    };
	auto hasignorekeyword = [&](std::string line)
    {
        for (auto bannedword:ignorelineswithkeyword) {
            if (line.find(bannedword) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    };


	// Let's count mutations to go through
	size_t mutationstotal = 0;
	size_t mutationsdonetotal = 0;
	for (size_t i = 0; i < linecount; ++i) {
        size_t lineinfile = i + startingline;
        const std::string &line = file.GetLine(lineinfile);
        if (!file.GetIsMultilineComment(i) && !hasignorekeyword(line)) {
            for (auto &mutator : Mutations) {
                const size_t mutationsPossible = mutator->CheckMutationsPossible(line);
                mutationstotal += mutationsPossible;
            }
        }
    }
	std::cout << "Possible mutations: " << mutationstotal << std::endl;

	std::cout << "" << std::endl; // Line to be deleted but progress report
	for (size_t i = 0; i < linecount; ++i) {
		double percentagedone = (double)mutationsdonetotal/(double)mutationstotal;
		std::chrono::duration<double,std::ratio<1,1>> timeelapsed = std::chrono::system_clock::now() - beginning;
		// Approximately 1.87 second per line on my comp. Maybe mix in this in the estimate?
		std::chrono::duration<double,std::ratio<1,1>> timeremaining = (timeelapsed / percentagedone)-timeelapsed;
		if (percentagedone == 0)
        {
		    // Do a initial estimate on first run
		    timeremaining = (buildtime + testtime) * mutationstotal;
        }
		/*
		std::cout << std::endl << std::endl << "**********************************************" << std::endl;
		std::cout << "Mutation progress:" << std::endl;
		std::cout << i << " of " << linecount << " lines processed in " << std::ceil(timeelapsed.count()) << " seconds." << std::endl;
		std::cout << "Approximately " << (int)(percentagedone*100) << "% done, " << std::ceil(timeremaining.count()/60) << " minutes left." << std::endl;
		std::cout << "**********************************************" << std::endl << std::endl;
		*/
		std::cout << "\033[A\33[2K\rMutation-testing " << (int)(percentagedone*100) << "% done, " << std::ceil(timeremaining.count()/60) << " minute(s) left." << std::endl;
		size_t lineinfile = i+startingline;
		const std::string &line = file.GetLine(lineinfile);
		SourceFile::MutationResult result = SourceFile::NoMutation;
		if (!file.GetIsMultilineComment(i) && !hasignorekeyword(line)) {
			for (auto &mutator : Mutations) {
				const size_t mutationsPossible = mutator->CheckMutationsPossible(line);
				for (size_t j = 0; j < mutationsPossible/* && result < SourceFile::MutationResult::Survived*/; ++j) {
				    mutationsdonetotal++;
					//std::cout << "Original:" << std::endl;
					//std::cout << lineinfile << ": " << line << std::endl;
					//std::cout << "Mutation:" << std::endl;
					const std::string &mutatedline = mutator->MutateLine(line, j);
					//std::cout << lineinfile << ": " << mutatedline << std::endl;
					assert(line != mutatedline);
					file.Modify(i+startingline, mutatedline);
					mutations++;
					file.WriteModification();
					if (Build((int)std::ceil(buildtime.count()) * 1000 * buildtimeoutmodifier)) {
						if (Test((int)std::ceil(testtime.count()) * 1000 * testtimeoutmodifier)) {
							result = SourceFile::MutationResult::Survived;
							file.SaveModification(result);
							//std::cout << "Mutation survived." << std::endl;
							survived++;
						} else {
							testfailes++;
							//std::cout << "Mutation killed." << std::endl;
							if (result < SourceFile::MutationResult::FailedTest) {
								result = SourceFile::MutationResult::FailedTest;
								file.SaveModification(result);
							}
						}
					} else {
						buildfailes++;
						//std::cout << "Build failed." << std::endl;
						if (result < SourceFile::MutationResult::FailedBuild) {
							result = SourceFile::MutationResult::FailedBuild;
							file.SaveModification(result);
						}
					}
					file.Revert();
				}
			}
			if (result != SourceFile::MutationResult::NoMutation) {
				HTMLExporter::WriteHTML(HTMLFileName, file.GetSaved(), Summary(end,
				                                                               lineinfile + 1, endingline, mutations,
                                                                               buildfailes, testfailes,
                                                                               survived).str());
				file.Revert();
			}
		} else {
			//std::cout << line << std::endl;
			//std::cout << "Multiline comment, ignoring." << std::endl;

		}
	}
    std::cout << "\033[A\33[2K\rMutation-testing 100% done, finished." << std::endl;
	HTMLExporter::WriteHTML(HTMLFileName, file.GetSaved(), Summary(end,
                                                                   linecount, linecount, mutations,
                                                                   buildfailes, testfailes,
                                                                   survived).str());
	std::cout << Summary(end,
                         linecount, linecount, mutations, buildfailes, testfailes, survived).str();
    std::cout << std::endl;
    std::cout << "The following mutations survived: " << std::endl;
    for (size_t i = 0; i < file.LineCount(); ++i) {
        if (file.GetSaved().at(i).second == SourceFile::MutationResult::Survived)
        {
            std::cout << filepath << ":" << (i+1) << " " << file.GetSaved().at(i).first << std::endl;
        }
    }
    std::cout << std::endl << "Results have been written to " << HTMLFileName << std::endl;
    std::cout << std::endl;
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
        size_t linesdone, size_t linestotal, size_t mutations, size_t buildfailes, size_t testfailes,
        size_t survived) {
	std::chrono::duration<double, std::ratio<1,1>> totaltime = std::chrono::system_clock::now() - timepoint_start;
	std::stringstream s;
	size_t killratio = KillRatioPerc(testfailes, survived);
	char timebuff[1024];
    time_t temp;
    struct tm *timeptr;
    temp = time(NULL);
    timeptr = localtime(&temp);
    strftime(timebuff, sizeof(timebuff), "%c", timeptr);
	s << "\n";
	s << "-----------------------------" << "\n";
	s << timebuff << "\n";
	s << "Time passed: " << (int)std::ceil(totaltime.count()/60) << " minutes" << "\n";
	s << "Lines processed: " << linesdone << " of " << linestotal << "\n";
	s << "Mutations: " << mutations << "\n";
	s << "Build failed: " << buildfailes << "\n";
	s << "Test failed: " << testfailes << "\n";
	s << "Test timedout: " << timeoutstest << "\n";
	s << "Mutations survived: " << survived << "\n";
	s << "Mutation killratio: " << killratio << "%\n";
	s << "-----------------------------" << "\n";
	return s;
}

size_t KillRatioPerc(size_t testfailes, size_t survived) {
    size_t total = testfailes + survived;
    if (total == 0)
        return 0;
    size_t killratio = (testfailes * 100) / total;
	return killratio;
}



bool Test(int timeoutms) {
	//std::cout << "Testing..." << std::endl;
    auto start = std::chrono::system_clock::now();
	auto rtrn = CommandRunner::RunCommand(testcommand,timeoutms,true);
    auto end = std::chrono::system_clock::now();
    auto testtimethis = end - start;

	if (rtrn == CommandRunner::CommandResult::CommandTimedout) {
	    //std::cout << "Test timedout" << std::endl;
        timeoutstest++;
        return false;
    } else {
	    // Update testing time continually
	    if (testtimethis > testtime)
        {
	        testtime = testtimethis;
        }
	    return rtrn==CommandRunner::CommandResult::CommandResultZero;
	}
}

bool Build(int timeoutms) {
	//std::cout << "Building..." << std::endl;
    auto start = std::chrono::system_clock::now();
    auto rtrn = CommandRunner::RunCommand(buildcommand,timeoutms,true);
    auto end = std::chrono::system_clock::now();
    auto buildtimethis = end - start;
    if (rtrn == CommandRunner::CommandResult::CommandTimedout) {
        //std::cout << "Build timed out" << std::endl;
        timeoutsbuild++;
        return false;
    } else {
        // Update build time continually
        if (buildtimethis > buildtime)
        {
            buildtime = buildtimethis;
        }
        return rtrn==CommandRunner::CommandResult::CommandResultZero;
    }
}
