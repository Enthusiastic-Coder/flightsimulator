# Building with CMake

The CMake build targets Windows x64 with MSVC and C++17. It requires the Qt 6 MSVC x64 kit with Qt Widgets and Qt OpenGL. OpenAL and Assimp are provided by the checked-out dependency submodules and their x64 libraries.

## Configure and build

From the repository root, in a Visual Studio Developer PowerShell or a terminal with CMake and the MSVC toolchain available:

```powershell
cmake -S . -B build -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.11.3/msvc2022_64"
cmake --build build --config Debug
```

Run the Release configuration with:

```powershell
cmake --build build --config Release
```

Use the path to your Qt kit for `CMAKE_PREFIX_PATH`. In Qt Creator, open the root `CMakeLists.txt` and select a Qt 6 MSVC x64 kit.

Run the executable from its output directory, which is `build/Debug` or `build/Release` with the Visual Studio generator. CMake copies the simulator assets and dependency DLLs there and runs `windeployqt` to deploy the Qt libraries and platform plugin.

`QtMain.cpp` creates the Qt application. `QtGameLoop.cpp` uses a `QOpenGLWindow` for the simulator and a Qt timer to schedule frames. Qt handles keyboard and mouse input and decodes textures. Physical joysticks use the existing Windows joystick backend. The cockpit PFD paints live flight data into a Qt image, which the simulator uploads to the cockpit texture.

## Dependencies

Initialize submodules when cloning the repository:

```powershell
git submodule update --init --recursive
```

OpenAL and Assimp libraries are read from their existing `openal/libs/Win64` and `assimp2/{Debug,Release}` directories.

The root `CMakeLists.txt` is the project entry point. Visual Studio can open the repository as a CMake project.
