#pragma once
#include <cmath>
#include <tuple>

typedef unsigned int uint;

uint pairing(uint a, uint b);
uint pairing3(uint a, uint b, uint c);
std::pair<uint, uint> reverse_pairing(uint z);
std::tuple<uint, uint, uint> reverse_pairing3(uint z);