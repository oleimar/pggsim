#include "EvoCode.hpp"
#include <iostream>

// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

int main(int argc, char* argv[])
{
    // Open input file and read indata
    EvoInpData eid(argv[1]);
    if (!eid.OK) {
        std::cout << "Input failed!" << "\n";
        return -1;
    }
    // Run the iteration
    Evo evo(eid);
    evo.Run();
    return 0;
}
