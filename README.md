
# Wave Function Collapse

This application uses [Walnut](https://github.com/TheCherno/Walnut) (specifically [this](https://github.com/thegeeko/Walnut) fork) to visualize how the Wave Function Collapse algorithm works.

## Walnut

Walnut is a nifty application framework that implements ImGui via Vulkan and exposes both.

## Requirements
- VulkanSDK
- <s>Premake</s> (Parent Repo)
- CMake >=3.16 (this fork)

## Building
```bash
mkdir build && cd build
cmake ..
cmake --build . -jN # N = cores
cd color_wave_function_collapse
./colorwavefunctioncollapse # execute
```

## 3rd party libraries
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [stb_image](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm) (included for convenience)
- [Walnut](https://github.com/TheCherno/Walnut)
