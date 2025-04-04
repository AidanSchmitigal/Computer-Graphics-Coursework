#include <math.h>

#ifndef F_PI
#define F_PI ((float)(M_PI))
#define F_2_PI ((float)(2.f * F_PI))
#define F_PI_2 ((float)(F_PI / 2.f))
#endif

/**
 * Draws a pill shape
 * @param time The current time
 * @param pill_width The width of the pill
 * @param pill_radius The radius of the pill
 * @param coords Pointer to an array of 2 floats to store the x and y coordinates of the pill
 * @param rotation Pointer to an float to store the rotation of the pill
 */
void pill_path(float time, float pill_width, float pill_radius, float coords[2], float* rotation) {
  float circle_length = F_PI * pill_radius;
  float side_length = pill_width * 2;
  float total_length = 2 * circle_length + 2 * side_length;

  // End Cap 1: \left\{t<\frac{c_{len}}{t_{len}}:\left(\cos\left(t\ \frac{t_{len}}{c_{len}}\cdot\pi\ +\frac{\pi}{2}\right)r-a,\sin\left(t\ \frac{t_{len}}{c_{len}}\ \cdot\ \pi\ +\frac{\pi}{2}\right)r\right)\right\}
  float end_cap_1_start_time = 0;
  float end_cap_1_end_time = circle_length / total_length;

  float end_cap_1_x = cos(time * total_length / circle_length * F_PI + F_PI_2) * pill_radius - pill_width;
  float end_cap_1_y = sin(time * total_length / circle_length * F_PI + F_PI_2) * pill_radius;

  // Side 1: \left\{\frac{c_{len}}{t_{len}}<t<\frac{c_{len}+\ s_{len}}{t_{len}}:\left(\left(2\frac{tt_{len}-c_{len}}{s_{len}}-1\right)a,-r\right)\right\}
  float side_1_start_time = end_cap_1_end_time;
  float side_1_end_time = (circle_length + side_length) / total_length;

  float side_1_x = (2 * (time * total_length - circle_length) / side_length - 1) * pill_width;
  float side_1_y = -pill_radius;

  // End Cap 2: \left\{\frac{c_{len}+s_{len}}{t_{len}}<t<\frac{2c_{len}+s_{len}}{t_{len}}:\left(\cos\left(t\ \frac{t_{len}}{c_{len}}\cdot\pi\ +\pi\right)r+a,\sin\left(t\ \frac{t_{len}}{c_{len}}\ \cdot\ \pi+\pi\ \right)r\right)\right\}
  float end_cap_2_start_time = side_1_end_time;
  float end_cap_2_end_time = (2 * circle_length + side_length) / total_length;

  float end_cap_2_x = cos((-circle_length + total_length * time - side_length) / circle_length * F_PI - F_PI_2) * pill_radius + pill_width;
  float end_cap_2_y = sin((-circle_length + total_length * time - side_length) / circle_length * F_PI - F_PI_2) * pill_radius;

  // Side 2: \left\{\frac{2c_{len}+s_{len}}{t_{len}}<t<1:\left(\left(2\frac{tt_{len}-2c_{len}-s_{len}}{s_{len}}-1\right)a,-r\right)\right\}
  float side_2_start_time = end_cap_2_end_time;
  float side_2_end_time = 1;

  float side_2_x = (2 * (-2 * circle_length + total_length * time - side_length) / (-2 * circle_length + total_length - side_length) - 1) * -pill_width;
  float side_2_y = pill_radius;

  if (time < end_cap_1_end_time) {
    coords[0] = end_cap_1_x;
    coords[1] = end_cap_1_y;
    rotation[0] = 90 + time * total_length / circle_length * -180.;
  } else if (time < side_1_end_time) {
    coords[0] = side_1_x;
    coords[1] = side_1_y;
    rotation[0] = 270;
  } else if (time < end_cap_2_end_time) {
    coords[0] = end_cap_2_x;
    coords[1] = end_cap_2_y;
    rotation[0] = 270 + ((-circle_length + total_length * time - side_length) / circle_length) * -180.;
  } else if (time < side_2_end_time) {
    coords[0] = side_2_x;
    coords[1] = side_2_y;
    rotation[0] = 90;
  }
}