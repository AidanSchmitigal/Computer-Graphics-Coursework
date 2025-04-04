#include <string>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI ((float)(M_PI))
#define F_2_PI ((float)(2.f * F_PI))
#define F_PI_2 ((float)(F_PI / 2.f))
#endif

struct Trait {
  std::string values[10];
  // std::string names[10];
};

enum Traits {
  HEIGHT,
  GROWTH_PATTERN,
  GROWTH_RATE,
  THORNS,
  STEM_THICKNESS,
  STALKS,
  BRANCH_DENSITY,
  BRANCH_ANGLE,
  BRANCH_DISTRIBUTION,
  LEAF_DENSITY,
  LEAF_SHAPE,
  LEAF_SIZE,
  LEAF_VARIATION,
  LEAF_ARRANGEMENT,
  FLOWER_DENSITY,
  FLOWER_SIZE,
  FLOWER_PETALS,
  FLOWER_SHAPE,
  FLOWER_COLOR,
  FRUIT_DENSITY,
  FRUIT_SHAPE,
  FRUIT_SIZE,
  FRUIT_VARIATION,
  FRUIT_COLOR
};

int trait_count = 24;
int traits[] = {5, 3, 3, 4, 4, 5, 4, 3, 3, 4, 6, 5, 3, 4, 4, 5, 5, 5, 5, 3, 5, 5, 2, 4};

std::string trait_names[24] = {
    "Height",
    "Growth Pattern",
    "Growth Rate",
    "Thorns",
    "Stem Thickness",
    "Stalks",
    "Branch Density",
    "Branch Angle",
    "Branch Distribution",
    "Leaf Density",
    "Leaf Shape",
    "Leaf Size",
    "Leaf Variation",
    "Leaf Arrangement",
    "Flower Density",
    "Flower Size",
    "Flower Petals",
    "Flower Shape",
    "Flower Color",
    "Fruit Density",
    "Fruit Shape",
    "Fruit Size",
    "Fruit Variation",
    "Fruit Color"};

std::string trait_value_names[24][10] = {
    {"Very Short", "Short", "Medium", "Tall", "Very Tall"},
    {"Linear", "Exponential", "Randomized"},
    {"Slow", "Moderate", "Fast"},
    {"None", "Few", "Moderate", "Dense"},
    {"Narrow", "Moderate", "Wide", "Very Wide"},
    {"1", "2", "3", "4", "5"},
    {"Sparse", "Moderate", "Dense", "Very Dense"},
    {"Upright", "Moderate", "Drooping"},
    {"Uniform", "Bottom-heavy", "Random"},
    {"Sparse", "Moderate", "Dense", "Very Dense"},
    {"Oval", "Round", "Serrated", "Needle", "Lobed", "Compound"},
    {"Very Small", "Small", "Medium", "Large", "Very Large"},
    {"Consistent", "Slightly Varied", "Highly Varied"},
    {"Alternate", "Opposite", "Whorled", "Spiral"},
    {"Sparse", "Moderate", "Dense", "Very Dense"},
    {"Tiny", "Small", "Medium", "Large", "Very Large"},
    {"3", "5", "7", "9", "Many"},
    {"Single Petal", "Star", "Cup", "Bell", "Composite"},
    {"Warm", "Cool", "Pastel", "Vibrant", "Exotic"},
    {"Sparse", "Moderate", "Dense"},
    {"Round", "Oval", "Elongated", "Clustered", "Star"},
    {"Tiny", "Small", "Medium", "Large", "Very Large"},
    {"Consistent", "Varied"},
    {"Red Shades", "Yellow/Orange Shades", "Purple/Blue", "Green Shades"},
};

std::string trait_display_names[24][10] = {
    {"Mini", "Petite", "Common", "Towering", "Giant"},
    {"", "Invasive", "Wandering"},
    {"", "Slow", "Rapid"},
    {"", "Prickly", "Thorny", "Barbed"},
    {"Spindle", "Stout", "Bulbous", "Massive"},
    {"", "Twin", "Triple", "Quad", "Penta"},
    {"Bare", "Green", "Bushy", "Dense"},
    {"Upright", "Slanted", "Drooping"},
    {"Balanced", "Heavyfoot", "Scattered"},
    {"Sparse", "Greenleaf", "Thickleaf", "Bushy"},
    {"Pointed", "Soft", "Serrated", "Needled", "Lobed", ""},
    {"Minileaf", "Fern", "", "Palm", "Monstera"},
    {"Uniform", "Variegated", "Diverse"},
    {"Alternating", "Opposing", "Whorled", "Spiral"},
    {"Bloom", "Blossom", "Flourish", "Flowerburst"},
    {"Premier", "Opllen", "Bellowed", "Clustered", "Lobel"},
    {"", "Single", "Few", "Overbloom", "Grand"},
    {"Simple", "Starry", "Cupflower", "Bellflower", "Clustered"},
    {"Sunfire", "Cool", "Pastel", "Bright", "Exotic"},
    {"Sparse", "Yarrow", "Fruitful"},
    {"Berry", "Nut", "Pod", "Cluster", "Starfruit"},
    {"Micro", "Little", "Adamant", "Opulent", "Adornment"},
    {"Pure", "Variegated"},
    {"Spicy", "Sunny", "Sweet", "Aromatic", "Delicate"}};

// Trait traits[] = {
//     //             Trait Name,         Possible Values,                                                         Trait Names
//     {{"Very Short", "Short", "Medium", "Tall", "Very Tall", 0}, {"Mini", "Petite", "Common", "Towering", "Giant", 0}},
//     // Very Short (0.1m-0.5m), Short (0.5m-1.5m), Medium (1.5m-3m), Tall (3m-8m), Very Tall (8m-20m).
//     {{"Linear", "Exponential", "Randomized", 0}, {"", "Invasive", "Wandering", 0}},
//     // Linear, Exponential, Randomized (affects growth progression).
//     {{"Slow", "Moderate", "Fast", 0}, {"", "Slow", "Rapid", 0}},
//     // Slow, Moderate, Fast.

//     {{"None", "Few", "Moderate", "Dense", 0}, {"", "Prickly", "Thorny", "Barbed", 0}},
//     // None, Few, Moderate, Dense.

//     {{"Narrow", "Moderate", "Wide", "Very Wide", 0}, {"Spindle", "Stout", "Bulbous", "Massive", 0}},
//     // Narrow (0.1x height), Moderate (0.5x height), Wide (1.0x height), Very Wide (1.5x height).
//     {{"1", "2", "3", "4", "5", 0}, {"", "Twin", "Triple", "Quad", "Penta", 0}},
//     // Mutually exclusive with branches.

//     {{"Sparse", "Moderate", "Dense", "Very Dense", 0}, {"Bare", "Green", "Bushy", "Dense", 0}},
//     // Sparse, Moderate, Dense, Very Dense.
//     {{"Upright", "Moderate", "Drooping", 0}, {"Upright", "Slanted", "Drooping", 0}},
//     // Upright (10°-30°), Moderate (30°-60°), Drooping (60°-90°).
//     {{"Uniform", "Bottom-heavy", "Random", 0}, {"Balanced", "Heavyfoot", "Scattered", 0}},

//     {{"Sparse", "Moderate", "Dense", "Very Dense", 0}, {"Sparse", "Greenleaf", "Thickleaf", "Bushy", 0}},
//     // Sparse, Moderate, Dense, Very Dense.
//     {{"Oval", "Round", "Serrated", "Needle", "Lobed", "Compound", 0}, {"Pointed", "Soft", "Serrated", "Needled", "Lobed", "", 0}},
//     // Oval, Round, Serrated, Needle, Lobed, Compound.
//     {{"Very Small", "Small", "Medium", "Large", "Very Large", 0}, {"Minileaf", "Fern", "", "Palm", "Monstera", 0}},
//     // Very Small, Small, Medium, Large, Very Large.
//     {{"Consistent", "Slightly Varied", "Highly Varied", 0}, {"Uniform", "Variegated", "Diverse", 0}},
//     // Consistent, Slightly Varied, Highly Varied.
//     {{"Alternate", "Opposite", "Whorled", "Spiral", 0}, {"Alternating", "Opposing", "Whorled", "Spiral", 0}},
//     // Alternate, Opposite, Whorled, Spiral.

//     {{"Sparse", "Moderate", "Dense", "Very Dense", 0}, {"Bloom", "Blossom", "Flourish", "Flowerburst", 0}},
//     // Sparse, Moderate, Dense, Very Dense.
//     {{"Tiny", "Small", "Medium", "Large", "Very Large", 0}, {"Premier", "Opllen", "Bellowed", "Clustered", "Lobel", 0}},
//     // Tiny, Small, Medium, Large, Very Large.
//     {{"3", "5", "7", "9", "Many", 0}, {"", "Single", "Few", "Overbloom", "Grand", 0}},
//     // 3, 5, 7, 9, Many.
//     {{"Single Petal", "Star", "Cup", "Bell", "Composite", 0}, {"Simple", "Starry", "Cupflower", "Bellflower", "Clustered", 0}},
//     // Single Petal, Star, Cup, Bell, Composite.
//     {{"Warm", "Cool", "Pastel", "Vibrant", "Exotic", 0}, {"Sunfire", "Cool", "Pastel", "Bright", "Exotic", 0}},
//     // Warm (red/yellow/orange), Cool (blue/purple), Pastel, Vibrant, Exotic (multicolor/iridescent).

//     {{"Sparse", "Moderate", "Dense", 0}, {"Sparse", "Yarrow", "Fruitful", 0}},
//     // Sparse, Moderate, Dense.
//     {{"Round", "Oval", "Elongated", "Clustered", "Star", 0}, {"Berry", "Nut", "Pod", "Cluster", "Starfruit", 0}},
//     // Round, Oval, Elongated, Clustered, Star.
//     {{"Tiny", "Small", "Medium", "Large", "Very Large", 0}, {"Micro", "Little", "Adamant", "Opulent", "Adornment", 0}},
//     // Tiny, Small, Medium, Large, Very Large.
//     {{"Consistent", "Varied", 0}, {"Pure", "Variegated", 0}},
//     // Consistent, Varied.
//     {{"Red Shades", "Yellow/Orange Shades", "Purple/Blue", "Green Shades", 0}, {"Spicy", "Sunny", "Sweet", "Aromatic", "Delicate", 0}}
//     // Red Shades, Yellow/Orange Shades, Purple/Blue, Green Shades.
// };

struct Plant {
  // std::string name;
  // Value from 0 (unborn) to 1(mature)
  float age;
  int traits[24];
};

int get_random_trait_value(int index) {
  return rand() % traits[index];
}

Plant generate_plant() {
  Plant p;
  p.age = 1.;
  // p.name = "Test Plant " + std::to_string(rand() % 1000);

  for (int i = 0; i < 24; i++) {
    p.traits[i] = get_random_trait_value(i);
  }

  return p;
}

/***************************************************
 * ====     OPENGL DRAW PLANT     ====
 ***************************************************/
void render_plant(Plant p) {
  // glPushMatrix();

  // glBegin(GL_QUADS);
  // // Cylinder
  // int height_slices = 10;
  // int facets = 8;
  // float radius = 0.5;
  // float height = p.traits[HEIGHT] * 2;

  // float height_step = height / height_slices;
  // float facet_step = 2 * F_PI / facets;

  // for (int i = 0; i < height_slices; i++) {
  //   for (int j = 0; j < facets; j++) {
  //     float theta = j * facet_step;
  //     float phi = (j + 1) * facet_step;

  //     glVertex3f(cos(theta) * radius, height_step * i, sin(theta) * radius);
  //     glVertex3f(cos(theta) * radius, height_step * (i + 1), sin(theta) * radius);
  //     glVertex3f(cos(phi) * radius, height_step * (i + 1), sin(phi) * radius);
  //     glVertex3f(cos(phi) * radius, height_step * i, sin(phi) * radius);
  //   }
  // }

  // glEnd();

  // glPopMatrix();
}