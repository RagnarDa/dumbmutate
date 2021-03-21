# dumbmutate
Simple mutation-testing.

# Features:
- No dependencies (except maybe timeout on Mac, see below). Pure C++11 and should work on
any compiler.
- Only command-line options needed, so could be part of a buildsystem for example. Has a
threshold option to return error-code when mutations killed falls below a specified percentage.  
- Exports HTML line-by-line report and also surviving mutations to terminal so you can
go-to-code directly in your IDE.
- Gives progress updates while running so you can tell how long left to finish the file
(in minutes).
- Ability to mutate only part of file, i e from line 10 to line 20.


Simple example:
```bash
dumbmutate --mutate=FileToMutate.cpp --build=make --test=./test
```
Results will be shown both in terminal and in MutationResult.html

# Demo (using docutest)
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

