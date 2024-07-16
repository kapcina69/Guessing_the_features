#include <iostream>
#include "klase.h"


int main() {
    GuessingSystem guessingSystem;

    try {
        guessingSystem.openTerminal();
    }
    catch (const GuessingSystemException& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}