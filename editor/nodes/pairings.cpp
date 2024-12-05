#include "pairings.h"

int pairing(int a, int b) {
  return (a + b) * (a + b + 1) / 2 + b;
}

int pairing3(int a, int b, int c) {
  return pairing(pairing(a, b), c);
}

std::pair<int, int> reverse_pairing(int z) {
  int w = (int)(std::floor((std::sqrt(8.0 * z + 1) - 1) / 2));
  int b = z - (w * (w + 1)) / 2;
  int a = w - b;
  return {a, b};
}

std::tuple<int, int, int> reverse_pairing3(int z) {
  auto [z1, c] = reverse_pairing(z);
  auto [a, b] = reverse_pairing(z1);
  return {a, b, c};
}