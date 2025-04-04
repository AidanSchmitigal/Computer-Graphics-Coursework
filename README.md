# Computer Graphics Coursework

A collection of my favorite projects from **CS550 - Computer Graphics** and **CS557 - Computer Graphics Shaders** at Oregon State University. These are the ones I had the most fun with and did a little extra.

From the courses: [CS550](CS550/) & [CS557](CS557/).

## Assignments

| Course | Project                                                      | What I did                                                                                                                                                                                                                                                              |
| ------ | ------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| CS550  | [Project 1 - Procedural Plant](CS550/Project1/)              | Made a plant entirely from procedural geometry, with a colorful pot, a twisting stem, golden-ratio leaf placement, and flowers oriented to the stem's surface.                                                                                                          |
| CS550  | [Project 3 - Lighting a Scene](CS550/Project3/)              | Miniature orbital scene with planets, moons, the ISS, and Europa Clipper, using hierarchical transformations, elliptical orbits, and lights that follow the bodies.                                                                                                     |
| CS550  | [Project 4 - Keytime Garden](CS550/Project4/)                | Noise-based grass, models I made in Blender, a watering can and seed pouch moving along equation defined paths, water and seed particles, growing flowers, and a bouncing frog with squash and stretch. The camera and colored light are animated too.                  |
| CS550  | [Final Project - Geometry Nodes](CS550/FinalProject/)        | Built a Blender-inspired procedural node editor with geometry primitives, vector and scalar math, transformations, instancing, scene time, graph saving/loading, lighting controls, and a debug renderer.                                                               |
| CS557  | [Project 6 - Shader Snake](CS557/Project6/)                  | Combined animated Voronoi-based coloring and displacement, rainbow scales, noise-based spots, a bending and waving body, and a fire effect along the snake.                                                                                                             |
| CS557  | [Final Project - Shader Terrain Editor](CS557/FinalProject/) | Made an interactive editor with eight layers of shader-generated noise, height and slope masks, an eight-color material gradient, lighting, material presets, and selectable grid resolutions. Changing the layers and materials makes completely different landscapes. |

## CS550 - Computer Graphics

### [Project 1 - Procedural Plant](CS550/Project1/)

![Procedural plant with a colorful pot, branching stems, leaves, and flowers](CS550/project1.png)

### [Project 3 - Lighting a Scene](CS550/Project3/)

| Orbital scene and ISS                                                    | Europa Clipper and colored lighting                                                          |
| ------------------------------------------------------------------------ | -------------------------------------------------------------------------------------------- |
| ![Orbital scene with the ISS and illuminated bodies](CS550/project3.png) | ![Europa Clipper in the orbital scene above a green-lit surface](CS550/project3_clipper.png) |

### [Project 4 - Keytime Garden](CS550/Project4/)

![Animated garden with a winding dirt path, flowers, watering can, and frog](CS550/project4.png)

![Animated garden with growing flowers, particles, and a bouncing frog](CS550/project4.webp)

### [Final Project - Geometry Nodes](CS550/FinalProject/)

![Geometry node editor displaying a procedural animated arrangement of rings and shapes](CS550/finalproject_fancy_graph.png)

![Animated demo of the procedural geometry node editor](CS550/finalproject_fancy_graph.webp)

## CS557 - Computer Graphics Shaders

### [Project 6 - Shader Snake](CS557/Project6/)

![Animated shader snake with colorful scales and bright fire effects in the editor](CS557/project6.png)

### [Final Project - Shader Terrain Editor](CS557/FinalProject/)

| Mountain landscape                                                                         | Layered terrain materials                                                                           |
| ------------------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------- |
| ![Terrain editor showing rocky mountains and green lowlands](CS557/finalproject_demo1.png) | ![Terrain editor showing contrasting material bands on steep terrain](CS557/finalproject_demo2.png) |
| **River valley**                                                                           | **Earth-toned terrain**                                                                             |
| ![Terrain editor showing a blue river between green hills](CS557/finalproject_demo3.png)   | ![Terrain editor showing layered brown and dark rocky terrain](CS557/finalproject_demo4.png)        |

## Building and running

All six projects are written in C++ and include a `Makefile`. From a project's folder, build with `make` and run `./sample.runme`. The supplied Makefiles target macOS and OpenGL; depending on the project and your setup, you may need to download or configure dependencies such as Dear ImGui, GLM, or GLFW, and adjust the Makefile's include and library paths.
