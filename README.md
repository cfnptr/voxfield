# Voxfield

Welcome to Voxfield, an open source project that offers a voxel based multiplayer sandbox game experience.
This project is a creative platform where players can build, modify, and explore in a dynamic, block based universe.
It's a space where imagination and exploration intersect, allowing users to create their own narratives and environments.

## Supported operating systems

* Windows (10/11)
* Ubuntu (22.04 LTS)
* macOS (Monterey 12)

This list includes only those systems on which functionality testing is conducted.
However, you can also compile it under any other Linux distribution or operating system.

## Supported hardware

* At least GPU with Vulkan API 1.1 support.

### Graphics processing unit (GPU)

* Nvidia [Maxwell](https://developer.nvidia.com/maxwell-compute-architecture) microarchitecture and newer
* AMD [GCN](https://en.wikipedia.org/wiki/Graphics_Core_Next) microarchitecture and newer
* Intel [Gen9](https://en.wikichip.org/wiki/intel/microarchitectures/gen9) microarchitecture and newer
* Apple [M1](https://en.wikipedia.org/wiki/Apple_M1) processor and newer (partial support)

### Central processing unit (CPU)

* Intel [Haswell](https://en.wikipedia.org/wiki/Haswell_(microarchitecture)) microarchitecture and newer
* AMD [Excavator](https://en.wikipedia.org/wiki/Excavator_(microarchitecture)) microarchitecture and newer

These requirements are based on the support of the **AVX2** instruction set.
If you want, you can freely disable it and compile for any other architecture, including **ARM**.

### Minimum memory size

* At least 4GB of RAM
* At least 2GB of VRAM (GPU)

## Build Voxfield

1. Follow Garden building [instructions](https://github.com/cfnptr/garden/blob/main/BUILDING.md)
1. Run ```git clone --recursive https://github.com/cfnptr/voxfield``` using **Terminal** (This may take some time)
3. Open voxfield directory using **IDE** or use [build.sh](build.sh) script

### CMake options

| Name                    | Description                         | Default value |
|-------------------------|-------------------------------------|---------------|
| VOXFIELD_BUILD_SERVER   | Build Voxfield server executable.   | `ON`          |
| VOXFIELD_BUILD_LAUNCHER | Build Voxfield launcher executable. | `ON`          |

## Garden Shading Language (GSL)

Documentation with all GLSL changes is [here](https://github.com/cfnptr/garden/blob/main/docs/gsl.md).
You can install **Visual Studio Code** [extension](https://marketplace.visualstudio.com/items?itemName=cfnptr.gsl-linter) to highlight GSL syntax.

## Third-party

* [Garden](https://github.com/cfnptr/garden) (Apache-2.0 license)