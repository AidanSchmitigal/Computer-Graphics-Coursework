#include "pairings.h"

uint pairing(uint a, uint b) {
  return (a + b) * (a + b + 1) / 2 + b;
}

uint pairing3(uint a, uint b, uint c) {
  return pairing(pairing(a, b), c);
}

std::pair<uint, uint> reverse_pairing(uint z) {
  uint w = (uint)(std::floor((std::sqrt(8.0 * z + 1) - 1) / 2));
  uint b = z - (w * (w + 1)) / 2;
  uint a = w - b;
  return {a, b};
}

std::tuple<uint, uint, uint> reverse_pairing3(uint z) {
  auto [z1, c] = reverse_pairing(z);
  auto [a, b] = reverse_pairing(z1);
  return {a, b, c};
}