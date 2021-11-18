# PathTracer
This project is an extension of my final project for the course _Fundamentals of Computer Graphics_. The goal of this project is to create a bidirectional path tracer which uses a microfaceted BRDF model. This project will form the backend of my much larger goal of creating a 3D Scene Viewing Application using realtime rendering and offline render.

## Current Progress
- Updated the BVH to use the surface area heuristic (SAH) for faster ray intersection test
- Implemented Sobol Quasi-Random Number Generator for faster convergence of pixel colors during pathtracing
- Integrated ASSIMP to support loading common 3D File Formats
- Created a GPU prototype (See my GPU-Raytracer project) for the eventual port to GPU

## TODO
- Add a PB microfaceted model for material BRDFs
- Add sampling methods for light objects to allow for specular sampling in BRDFs
- Scramble the Sobol Generator to allow for better samples and extend to 4 dimensions to support Depth of Field Sampling
- Integrate QT UI framework for manipulation of scene files
- Port Pathtracer to GPU compute shader
- Create Realtime preview window using OpenGL or Vulkan - should utilize tiled rendering
