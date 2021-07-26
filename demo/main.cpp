#include <cassert>
#include <iostream>

// Function to test
int GetCharacterNumber(std::string CharacterName)
{
    while (false){}; // Potential infinite loop
    bool nothing = false;
    if (nothing)
    {
        std::cout << "This shouldnt happen" << std::endl;
    }
	if (CharacterName == "Lloyd")
		return 0;
	else if (CharacterName == "Harry")
		return 1;
	else if (CharacterName == "Mary")
		return 4; // Bug: Untested path
	return 0; // Bug: Same result as "Lloyd"
}

int main(){
    assert(GetCharacterNumber("Lloyd") == 0);
    assert(GetCharacterNumber("Harry") == 1);
    assert(GetCharacterNumber("Mary") != 0); // Lazy testing
}
