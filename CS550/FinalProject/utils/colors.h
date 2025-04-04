#pragma once
#include <math.h>

constexpr float RED[4] = {1.0f, 0.0f, 0.0f, 1.0f};
constexpr float GREEN[4] = {0.0f, 1.0f, 0.0f, 1.0f};
constexpr float BLUE[4] = {0.0f, 0.0f, 1.0f, 1.0f};
constexpr float WHITE[4] = {1.0f, 1.0f, 1.0f, 1.0f};
constexpr float BLACK[4] = {0.0f, 0.0f, 0.0f, 1.0f};

// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

// void HsvRgb(float hsv[3], float rgb[3]) {
//   // guarantee valid input:

//   float h = hsv[0] / 60.f;
//   while (h >= 6.) h -= 6.;
//   while (h < 0.) h += 6.;

//   float s = hsv[1];
//   if (s < 0.)
//     s = 0.;
//   if (s > 1.)
//     s = 1.;

//   float v = hsv[2];
//   if (v < 0.)
//     v = 0.;
//   if (v > 1.)
//     v = 1.;

//   // if sat==0, then is a gray:

//   if (s == 0.0) {
//     rgb[0] = rgb[1] = rgb[2] = v;
//     return;
//   }

//   // get an rgb from the hue itself:

//   float i = (float)floor(h);
//   float f = h - i;
//   float p = v * (1.f - s);
//   float q = v * (1.f - s * f);
//   float t = v * (1.f - (s * (1.f - f)));

//   float r = 0., g = 0., b = 0.;  // red, green, blue
//   switch ((int)i) {
//     case 0:
//       r = v;
//       g = t;
//       b = p;
//       break;

//     case 1:
//       r = q;
//       g = v;
//       b = p;
//       break;

//     case 2:
//       r = p;
//       g = v;
//       b = t;
//       break;

//     case 3:
//       r = p;
//       g = q;
//       b = v;
//       break;

//     case 4:
//       r = t;
//       g = p;
//       b = v;
//       break;

//     case 5:
//       r = v;
//       g = p;
//       b = q;
//       break;
//   }

//   rgb[0] = r;
//   rgb[1] = g;
//   rgb[2] = b;
// }