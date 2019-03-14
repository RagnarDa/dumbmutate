cmake_minimum_required(VERSION 2.4)
project(dumbmutate)

set(CMAKE_CXX_STANDARD 11)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -pedantic -Wextra")
endif()

add_executable(dumbmutate main.cpp SourceFile.cpp SourceFile.h MutatorBase.cpp MutatorBase.h MutatorCaret.cpp MutatorCaret.h MutatorEqual.cpp MutatorEqual.h MutatorAddSubtr.cpp MutatorAddSubtr.h HTMLExporter.cpp HTMLExporter.h MutatorNEqual.cpp MutatorNEqual.h MutatorIncrDecr.cpp MutatorIncrDecr.h MutatorNumShift.cpp MutatorNumShift.h MutatorAndOr.cpp MutatorAndOr.h MutatorTrueFalse.cpp MutatorTrueFalse.h Utility.h Utility.cpp)