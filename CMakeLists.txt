cmake_minimum_required(VERSION 3.13)
project(dumbmutate)

set(CMAKE_CXX_STANDARD 14)

add_executable(dumbmutate main.cpp FileMutator.cpp FileMutator.h Tester.cpp Tester.h TestClass.cpp TestClass.h SourceFile.cpp SourceFile.h MutatorBase.cpp MutatorBase.h MutatorCaret.cpp MutatorCaret.h MutatorEqual.cpp MutatorEqual.h MutatorAddSubtr.cpp MutatorAddSubtr.h HTMLExporter.cpp HTMLExporter.h cxxopts.h)