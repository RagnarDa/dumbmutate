cmake_minimum_required(VERSION 2.4)
project(dumbmutate)

set(CMAKE_CXX_STANDARD 11)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -pedantic -Wextra")
endif()

add_executable(dumbmutate Source/main.cpp Source/SourceFile.cpp Source/SourceFile.h Source/MutatorBase.cpp Source/MutatorBase.h Source/MutatorCaret.cpp Source/MutatorCaret.h Source/MutatorEqual.cpp Source/MutatorEqual.h Source/MutatorAddSubtr.cpp Source/MutatorAddSubtr.h Source/HTMLExporter.cpp Source/HTMLExporter.h Source/MutatorNEqual.cpp Source/MutatorNEqual.h Source/MutatorIncrDecr.cpp Source/MutatorIncrDecr.h Source/MutatorNumShift.cpp Source/MutatorNumShift.h Source/MutatorAndOr.cpp Source/MutatorAndOr.h Source/MutatorTrueFalse.cpp Source/MutatorTrueFalse.h Source/Utility.h Source/Utility.cpp Source/CommandRunner.cpp Source/CommandRunner.h Source/MutatorIf.cpp Source/MutatorIf.h)
