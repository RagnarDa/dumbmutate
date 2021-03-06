#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include <string>

// Function to test
int GetCharacterNumber(std::string CharacterName)
{
	if (CharacterName == "Lloyd")
		return 0;
	else if (CharacterName == "Harry")
		return 1;
	else if (CharacterName == "Mary")
		return 4; // Bug: Untested path
	return 0; // Bug: Same result as "Lloyd"
}

TEST_CASE("test of GetCharacterNumber") {
    CHECK(GetCharacterNumber("Lloyd") == 0);
    CHECK(GetCharacterNumber("Harry") == 1);
    CHECK(GetCharacterNumber("Mary") != 0); // Lazy testing
}
