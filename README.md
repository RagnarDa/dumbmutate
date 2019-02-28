# dumbmutate
Simple mutation-testing

I tried different mutation-solutions but all was difficult to set up (especially on Mac) 
and since mutation testing is of questionable utility anyway I didn't want to spend too
much time on it so I created my own as-simple as possible (hence "dumb") solution.


Features:
- No dependencies (except maybe timeout on Mac, see below). Pure C++11 and should work on
any compiler.
- Only command-line options needed, so could be part of a buildsystem for example.
- Exports HTML line-by-line report and also surviving mutations to terminal so you can
go-to-code directly in your IDE.
- Gives progress updates while running so you can tell how long left to finish the file
(in minutes).


Simple example:

$ dumbmutate --file=FileToTest.cpp --compile=make --test=./test

Results will be shown both in terminal and in MutationResult.html


How to build:

$ cmake ./

$ make



When the test or build hang:
You should use the timeout command (available on Linux but needs to be installed on Mac,
needs to figure out how to do this on Windows).


Recommendation for Mac (requires homebrew):

$ brew install timeout


Run with timeout 30 seconds (Linux):

$ dumbmutate --file=FileToTest.cpp --compile="timeout 30 make" --test="timeout 30 ./test"


Run with timeout 30 seconds (Mac):

$ dumbmutate --file=FileToTest.cpp --compile="gtimeout 30 make" --test="gtimeout 30 ./test"
