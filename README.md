# EnDjinnV
EnDjinnV is a self-indulgent research project focused on bleeding-edge rendering technology and game engine architecture: the sort of slow-and-deliberate focus that is generally unacceptable in a live development/production environment.

!!The [LunarG Vulkan Tutorial](https://vulkan.lunarg.com/doc/sdk/1.1.114.0/windows/tutorial/html/index.html) has been completed!!

![Sample Image](/Samples/renderdoc-analysis3.png)
![Uniform Time Buffer Sample](/Samples/TimeBufferUsageSample.gif)

# Features
- Simple Vulkan Rendering. Game engine architecture beginnings.
- Vulkan Debug/Validation layers
- Renderdoc integration

# Next Steps
- Handle buffer resizing when the window changes.
- Get camera rotation functional.
- Implement game engine style Scene/Lighting objects. We want to set camera transform arbitrarily, have an arbitrary number of meshes (and draw calls), and at least very basic lighting.
- Classes were made to be responsible for cleaning up GPU memory (i.e.: Djn::Gfx::Buffer). These classes have move semantics and somewhat behave like std::unique_ptr, but more robust types are already available via `vulkan.hpp`, so we should probably switch to those to reduce duplication and cruft.

# Goals

## Short Term
- A "scene" architecture to manage and allow serialization of renderable objects and cameras
- VR-integration
- An asset import pipeline (i.e.: When running a built game, these things should already be compiled)
  - GLTF format 3d models
  - A better GLSL shaders
  - any sort of audio or other necessary graphics
- CPU performance and memory profiling instrumentation
  
## Long Term
- Audio
- Physics
- RayTracing
- VR
- Android/Oculus Quest support
  
# NOT-Goals (AKA: high-level libraries)
EnDjinnV does not currently intend to pioneer in some areas. To narrow scope down to extreme-performance graphics and game development, the following libraries are either in use or will be:
- GLM (which even seems to have SIMD support its latest versions)
- Boost (specifically, no one should ever waste time developing a x-plat stack trace utility)
- FMOD or Wwise (or some other audio utility)

# Getting Started
The project is built with Visual Studio 2019.
Necessary envvars:
- VULKAN_SDK - location of Vulkan sdk 1.0.x.
- GLM_DIR - location of GLM version 0.99...
- BOOST_DIR - location of Boost version 1.71...

# Conventions
- Wrap everything in a namespace. The base namespace for this project is `Djn`
- Prefer throwing to error checking and return codes (Djn::Exception provides a stacktrace).
- Consequently, dynamic memory allocation must be self-managing (read: it cleans itself up on exceptions).
- Types are CamelCase while members (functions and fields) are camelCase, regardless of privacy.
- Indent 4 spaces
- Allman for classes and functions, K&R internals:
```
void MyClass::MyFunc()
{
    while (...) {
        // ...
    }

    if (...) {
        // ...
    } else {
        // ...
    }
}
```
- When conditions get long, prefer dropping conditions to the next indent level, and convert to Allman:
```
if(true) {
    // ...
} else {
    // ...
}

if (myLongVariableNameA == someOtherLongVariableNameA &&
    myLongVariableNameB == someOtherLongVariableNameB &&
    myLongVariableNameC == someOtherLongVariableNameC)
{
    // ...
} else {
    // ...
}
```


# Open questions
The best way to resolve GLM and BOOST deps is not set in stone. Whether to include a git submodule or include the headers in the EnDjinn vsproj directly is undecided. I am currently leaning toward direct inclusion in the vsproj, but implementation feels like fighting a rising tide. Very open to opinions regarding this.
