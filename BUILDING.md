# Building with CMake

The CMake build targets the existing Windows x64/MSVC application and keeps the project's C++17 setting. SDL2, SDL2_image, OpenAL, and Assimp are provided by the checked-out dependency submodules and their x64 libraries.

## Configure and build

From the repository root, in a Visual Studio Developer PowerShell or a terminal with CMake and the MSVC toolchain available:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Debug
```

Run the Release configuration with:

```powershell
cmake --build build --config Release
```

The executable and copied runtime assets are placed in the generated build directory. CMake copies the simulator's asset folders, SDL2, SDL2_image, OpenAL, Assimp, and SDL_image codec runtime DLLs next to the executable.

## Dependencies

Initialize submodules when cloning the repository:

```powershell
git submodule update --init --recursive
```

CMake expects the SDL2 and SDL2_image x64 Debug and Release libraries under `sdl2/VisualC/x64` and `sdl2_image/VisualC/x64`. If those libraries are absent, build the matching x64 Debug and Release configurations from the dependency submodules before configuring this project. OpenAL and Assimp libraries are read from their existing `openal/libs/Win64` and `assimp2/{Debug,Release}` directories.

The older Qt/qmake and Visual Studio project files are retained during the transition; the root `CMakeLists.txt` is the CMake entry point.
