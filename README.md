# EnDjinnV
EnDjinnV is a self-indulgent Vulkan research project and "simple" game engine. Self-indulgent because it is focused on slow-and-deliberate meditation on bleeding-edge game engine architecture: the sort of slow-and-deliberate meditation that is generally unacceptable in a live development/production environment.

# Goals
The long-term goal of EnDjinnV is to be a full-fledged Vulkan-only game engine. VR support is intended, and probably Android support for the Oculus Quest.

## Short Term Goals
- A "scene" architecture to manage and allow serialization of renderable objects and cameras
- VR-integration
- An asset import pipeline (i.e.: When running a built game, these things should already be compiled)
  - GLTF format 3d models
  - GLSL shaders
  - any sort of audio or other necessary graphics
- Profiling tool and other debug integrations
  - Something? for CPU performance and memory profiling
  - RenderDoc for GPU performance
  
## Long Term Goals
- RayTracing
  
# NOT-Goals (AKA: high-level libraries)
EnDjinnV does not currently intend to pioneer in some areas. To narrow scope down to extreme-performance graphics and game development, the following libraries are either in use or will be:
- GLM (which even seems to have SIMD support its latest versions)
- Boost (specifically, no one should ever waste time developing a x-plat stack trace utility)
- FMOD or Wwise (or some other audio utility)

# Status
Vulkan is being developed (mostly) following the LunarG Vulkan tutorial [here](https://vulkan.lunarg.com/doc/sdk/1.1.114.0/windows/tutorial/html/index.html).
Platform-specific handling is tucked away in the Platform class or the XPlat utility namespace.

# Getting Started
The project is built with Visual Studio 2019.
Necessary envvars:
- VULKAN_SDK - location of Vulkan sdk 1.0.x.
- GLM_DIR - location of GLM version 0.99...
- BOOST_DIR - location of Boost version 1.71...

# Conventions
- Wrap everything in the Djn namespace.
- Prefer throwing to error checking and return codes (use Djn::Exception, which provides a stacktrace).
- This means any allocations (CPU or GPU) must be contained in self-managing classes or smart pointers.
- Fields are camelCase, while functions and methods are CamelCase.
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
