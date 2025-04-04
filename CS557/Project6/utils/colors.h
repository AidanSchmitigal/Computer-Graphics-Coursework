#pragma once

constexpr float RED[4] = {1.0f, 0.0f, 0.0f, 1.0f};
constexpr float GREEN[4] = {0.0f, 1.0f, 0.0f, 1.0f};
constexpr float BLUE[4] = {0.0f, 0.0f, 1.0f, 1.0f};
constexpr float WHITE[4] = {1.0f, 1.0f, 1.0f, 1.0f};
constexpr float BLACK[4] = {0.0f, 0.0f, 0.0f, 1.0f};

void HsvRgb(float[3], float[3]);
void RgbHsv(float[3], float[3]);