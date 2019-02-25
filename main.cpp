#include <iostream>
#include "SourceFile.h"
#include "HTMLExporter.h"
#include "MutatorAddSubtr.h"
#include "MutatorCaret.h"
#include "MutatorEqual.h"

#include "cxxopts.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

bool Compile();

bool Test();

#include <stdio.h>
#include <stdlib.h>

int pipetest( void )
{

	char   psBuffer[128];
	FILE   *pPipe;

	/* Run DIR so that it writes its output to a pipe. Open this
	 * pipe with read text attribute so that we can read it
	 * like a text file.
	 */

	if( (pPipe = popen( "./A6E", "w" )) == NULL )
		exit( 1 );

	/* Read pipe until end of file, or an error occurs. */

	while(fgets(psBuffer, 128, pPipe) || !feof(pPipe))
	{
		printf(psBuffer);
	}



	/* Close pipe and print return value of pPipe. */
	if (feof( pPipe))
	{
		printf( "\nProcess returned %d\n", pclose( pPipe ) );
	}
	else
	{
		printf( "Error: Failed to read the pipe to the end.\n");
	}
}

cxxopts::ParseResult ParseCommandLine(int argc, char* argv[])
{
	try
	{
		cxxopts::Options options(argv[0], " - example command line options");
		options
				.positional_help("[optional args]")
				.show_positional_help();

		bool apple = false;

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
std::chrono::duration<double> compiletime = std::chrono::duration<double>(600*5);
std::chrono::duration<double> testtime = std::chrono::duration<double>(600*5);
int main(int argc, char* argv[])
{
	//pipetest();
	//return 0;
	auto result = ParseCommandLine(argc, argv);
	SourceFile file(result["file"].as<std::string>());
	compilecommand = result["compile"].as<std::string>();
	testcommand = result["test"].as<std::string>();
	auto start = std::chrono::system_clock::now();

	if (!Compile()) {
		std::cerr << "Unmodified compile failed. Fix your program.";
		exit(1);
	};
	auto end = std::chrono::system_clock::now();
	compiletime = end-start;
	std::cout << "Normal compile-time: " << compiletime.count() << std::endl;
	start = std::chrono::system_clock::now();
	if (!Test()) {
		std::cerr << "Unmodified test failed. Fix your program.";
		exit(2);
	}
	end = std::chrono::system_clock::now();
	testtime = end-start;
	std::cout << "Normal test-time: " << compiletime.count() << std::endl;

	// TODO: More mutations

	size_t mutations = 0, compilefailes = 0, testfailes = 0, survived = 0;
	std::vector<MutatorBase*> Mutations({new MutatorEqual(), new MutatorAddSubtr(), new MutatorCaret()});
	for (int i = 0; i < file.LineCount(); ++i) {
		const std::string &line = file.GetLine(i);
		SourceFile::MutationResult result = SourceFile::NoMutation;
		for (auto &mutator : Mutations) {
			const size_t mutationsPossible = mutator->CheckMutationsPossible(line);
			for (int j = 0; j < mutationsPossible && result < SourceFile::MutationResult::Survived; ++j) {
				file.Modify(i, mutator->MutateLine(line));
				mutations++;
				file.WriteModification();
				if (Compile()) {
					if (Test()) {
						result = SourceFile::MutationResult::Survived;
						std::cout << "Survived." << std::endl;
						survived++;
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
		}
		file.SaveModification(result);
		HTMLExporter::WriteHTML("./MutationResult.html", file.GetSaved());
		file.Revert();
	}

	std::cout << std::endl;
	std::cout << "----------------------" << std::endl;
	std::cout << "Mutations: " << mutations << std::endl;
	std::cout << "Compile failed: " << compilefailes << std::endl;
	std::cout << "Test failed: " << testfailes << std::endl;
	std::cout << "Survived: " << survived << std::endl;
	return survived;
}


bool RunCommand(const char * command)
{
	int rtrn = system(command);
//	std::cout << "Returned " << rtrn << std::endl;
	return (rtrn == 0);
}

bool RunCommand(const char * command, std::chrono::duration<double> timeout)
{
	return RunCommand(command);
	std::mutex m;
	std::condition_variable cv;
	bool retValue = false;

	std::thread t([&cv, &retValue, &command]()
	              {
		              retValue = RunCommand(command);
		              cv.notify_one();
	              });

	t.detach();
	std::thread::native_handle_type handle = t.native_handle();

	{
		std::unique_lock<std::mutex> l(m);
		if(cv.wait_for(l, std::chrono::duration<double>(60*10000)) == std::cv_status::timeout) {
			std::cout << "Timed out" << std::endl;
			//pthread_cancel(handle);
			return false;
		}
	}
	std::cout << "Retval " << retValue;
	return retValue;
}

bool Test() {
	std::cout << "Testing." << std::endl;
	return RunCommand(testcommand.c_str(),testtime*3);
}

bool Compile() {
	std::cout << "Compiling." << std::endl;
	return RunCommand(compilecommand.c_str(), compiletime*100);
}
