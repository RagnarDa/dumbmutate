# dumbmutate
Simple mutation testing.

# What is mutation testing?
Mutation testing is a way to test your automated tests. Imagine you have a unit-test or integration-test that 100% covers your code. Can you be sure your test actually would catch errors in your code? Mutation testing goes through your code and changes it slightly, like changing a operator (== into = for example) or a number (5 into a 0 for example). It then builds your code and tests it, hoping that your tests will fail (mutation killed). If the test passes the mutation is said to have survived, and the line is marked.

# About dumbmutate
This is a as simple as possible implementation of this technique. There is no setup and no depencies. Just download the binary and run it from the command-line.Works with any unit-testing framework (GoogleTest, Catch2, boosttest etc), as long as they return a non-zero value on failed tests. It is geared towards C-style languages but might work with other ones too.

# Features:
- Only command-line options needed, so could be part of a buildsystem for example. Has a threshold option to return error-code when mutations killed falls below a specified percentage.  
- Exports HTML line-by-line report and also surviving mutations to terminal so you can go-to-code directly in your IDE.
- Gives progress updates while running so you can tell how long left to finish the file (in minutes).
- Ability to mutate only part of file, i e from line 10 to line 20.
- No dependencies (except maybe timeout on Mac, see below). Pure C++11 and should work on any compiler.


Simple example:
```bash
dumbmutate --mutate=FileToMutate.cpp --build=make --test=./test
```
Results will be shown both in terminal and in MutationResult.html

# Demo (using doctest)
<p align="center"><img src="/demo.gif?raw=true"/></p>

# How to build dumbmutate:
```bash
cmake ./
make
```

# Mutate specific part:
Mutate specific lines of FileToTest.cpp, line 10 to 20:
```bash
dumbmutate --mutate=FileToTest.cpp --build="make" --test="./test" --start=10 --end=20
```

# Add threshold for percentage of killed mutations
Useful for example in a CI-solution where you could make dumbmutate return a error when the ratio of killed mutations vs survived falls below a set percentage. Example sets 80% as threshold and will return 0 when mutation-killratio is above this percentage.
```bash
dumbmutate --mutate=FileToTest.cpp --build="make" --test="./test" --threshold=80
```

# If the test or build hang:
You should use the timeout command (available out-of-the-box on Linux but needs to be installed on Mac,
and is available through cygwin on Windows).

Run with timeout 30 seconds (Linux or Windows using cygwin):
```bash
dumbmutate --mutate=FileToTest.cpp --build="timeout 30 make" --test="timeout 30 ./test"
```

Run with timeout 30 seconds on Mac (requires homebrew):
```bash
brew install coreutils
dumbmutate --mutate=FileToTest.cpp --build="gtimeout 30 make" --test="gtimeout 30 ./test"
```

