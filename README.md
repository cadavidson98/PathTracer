# PathTracer
This project is an extension of my final project for the course _Fundamentals of Computer Graphics_. The goal of this project is to create a bidirectional path tracer which uses a microfaceted BRDF model. This project will form the backend of my much larger goal of creating a 3D Scene Viewing Application using both a viewport renderer (OpenGL) and offline render.

## Features/Highlights
For detailed explanation of each feature, please check my detailed writeup on [my website](https://cadavidson98.github.io/PathTracer.html).
- Multi-level Scene Graph with Geometric Primitive Instancing
- Microfacted Reflectance Models, including the Cook Torrence Sparrow & Disney Principled BRDFs
- Importancing sampling the Distribution of Visible Normals for the GGX Distribution
- Multiple Importance Sampling of Area Lights & Direct Light Sampling for Dirac Delta Light sources (Directional)
- Custom File Format, complete with a Blender export plugin

## In Progress
- GUI for Scene previewing & interactive debugging of light paths
- Subsurface & Specular Transmission
- Textures for material parameters

## Coming Soon
- Owen Scrambling for the Sobol Generator
