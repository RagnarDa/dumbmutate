cmake_minimum_required(VERSION 3.0)
project(dumbmutate)

set(CMAKE_CXX_STANDARD 11)

add_executable(dumbmutate main.cpp SourceFile.cpp SourceFile.h MutatorBase.cpp MutatorBase.h MutatorCaret.cpp MutatorCaret.h MutatorEqual.cpp MutatorEqual.h MutatorAddSubtr.cpp MutatorAddSubtr.h HTMLExporter.cpp HTMLExporter.h MutatorNEqual.cpp MutatorNEqual.h)