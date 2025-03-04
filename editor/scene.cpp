#include "scene.h"

namespace Scene {

float Time = 0.0;       // used for animation, # of seconds elapsed
float DeltaTime = 0.0;  // used for animation, # of seconds since last frame
double MouseX = 0.0;    // mouse position
double MouseY = 0.0;    // mouse position
int Viewport[4];        // viewport size
int FrameCount;         // frame counter

}  // namespace Scene