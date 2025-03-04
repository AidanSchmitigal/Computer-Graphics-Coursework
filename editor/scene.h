#pragma once

namespace Scene {

extern float Time;       // used for animation, # of seconds elapsed
extern float DeltaTime;  // used for animation, # of seconds since last frame
extern double MouseX;    // mouse position
extern double MouseY;    // mouse position
extern int Viewport[4];  // viewport size
extern int FrameCount;   // frame counter

}  // namespace Scene