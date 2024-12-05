#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#include "glew.h"
#endif

#include <vector>

#include "setmaterial.h"

struct Particle {
  float position[3];  // x, y, z position
  float velocity[3];  // x, y, z velocity
  float color[3];     // r, g, b color
  float lifetime;     // Remaining lifetime of the particle
  float size;         // Size of the particle
};

const int MAX_PARTICLES = 1000;
std::vector<Particle> particles;
float spawn_rate = 0.02f;  // Time between spawning new particles
float last_spawn_time = 0.0f;

void spawn_particle(float x, float y, float z, float vx, float vy, float vz, float r, float g, float b) {
  if (particles.size() < MAX_PARTICLES) {
    Particle p;
    p.position[0] = x;
    p.position[1] = y;
    p.position[2] = z;

    p.velocity[0] = vx;
    p.velocity[1] = vy;
    p.velocity[2] = vz;

    p.color[0] = r;
    p.color[1] = g;
    p.color[2] = b;

    p.lifetime = 3.f;  // Lifetime of 3 seconds
    p.size = 0.1f;     // Initial size

    particles.push_back(p);  // Add new particle to the system
  }
}

float gravity = 4.f;

void update_particles(float deltaTime) {
  for (auto it = particles.begin(); it != particles.end();) {
    it->velocity[1] -= gravity * deltaTime;
    it->position[0] += it->velocity[0] * deltaTime;
    it->position[1] += it->velocity[1] * deltaTime;
    it->position[2] += it->velocity[2] * deltaTime;

    it->lifetime -= deltaTime;  // Decrease lifetime

    if (it->lifetime <= 0 || it->position[1] < -1) {
      it = particles.erase(it);  // Remove particle if lifetime is over
    } else {
      ++it;
    }
  }
}

void render_particles() {
  // printf("Rendering %d particles\n", particles.size());
  glPointSize(5.0f);  // Set point size for particles
  glBegin(GL_POINTS);
  for (const auto& p : particles) {
    glColor3f(p.color[0], p.color[1], p.color[2]);
    SetMaterial(p.color[0], p.color[1], p.color[2], 1.f);
    glVertex3f(p.position[0], p.position[1], p.position[2]);
  }
  glEnd();
}
