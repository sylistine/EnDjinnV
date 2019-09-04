# EnDjinnV
A modern renderer with game engine architecture (aspirational).
Basically rerolling EnDjinn and swapping DX12 with Vulkan.

# Goals
Like EnDjinn, EnDjinnV ultimately aspires to be a VR game engine.
That goal is very far off.

Short Term:
- a game Scene logic that influences how the command queue is built
- VR integration
- GLTF and <image type> support, and in generally an asset importing pipeline.
- Profiling tool integration?
  - At least RenderDoc tooling in the short term?
  - new overload for memory profiling?
  - <ihavenoideawhatimdoing.png>
- RayTracing, because hell yeah.

# Status
In general, this is developed following the LunarG Vulkan tutorial. For those following along, we're [here](https://vulkan.lunarg.com/doc/sdk/1.1.114.0/windows/tutorial/html/05-init_swapchain.html).
Platform stuff is pretty much all tucked away in the Platform class, which manages OS GUI Window junk. It should probably be just a platform utility class, and a Surface class made to be the abstract version of hWnd and WndProc (or whatever is used on Linux and Android).

# Conventions
- Prefer throwing to extensive error checking and returns.
- public members are CamelCase, while private members are camelCase.
- indent 4 spaces
- Allman, not K&R:
```
while (true)
{
    // do stuff
}
```
