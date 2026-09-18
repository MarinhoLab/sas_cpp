# marinholab_sas_core

> ROS-free C++ core of the SmartArmStack.
> This repository holds the pure C++ part of
> [SmartArmStack/sas_core](https://github.com/SmartArmStack/sas_core):
> everything that does not depend on ROS2 and does not depend on pybind11.
>
> More information about SmartArmStack is available in
> [smartarmstack.github.io](https://smartarmstack.github.io/).

## Contents

- `include/marinholab/sas/core/` — public C++ headers
  (namespace `marinholab::sas::core`).
- `src/` — library implementation.
- `examples/` — C++ example programs (built by default).
- `docker/` — `ubuntu:noble` build environment and full test pipeline.
- `debian/` — Debian packaging (native source package).
- `tools/version.sh` — rolling version computation.

The library depends on **Eigen3** and **dqrobotics**.

## Versioning

The project uses a rolling `YY.MM.NN` version:

- `YY.MM` — the current month (e.g. `26.09` for September 2026).
- `NN` — the number of commits since the version tag `vYY.MM`
  (e.g. `v26.09`); `00` when the tag does not exist yet.

The version is computed at build time by `tools/version.sh`; the first
release after a new month starts at `.00` and rolls forward with each
commit.

## Building (CMake)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

- A **shared** library (`libmarinholab_sas_core.so`) is built by default
  (the standard CMake `BUILD_SHARED_LIBS` variable).
- For a **static** library instead: `cmake -B build -DBUILD_SHARED_LIBS=OFF`.
- Example executables are built by default; disable with
  `-DMARINHO_LAB_SAS_CORE_BUILD_EXAMPLES=OFF`.

### Prerequisites

- Eigen3: `apt-get install libeigen3-dev`
- dqrobotics: from the DQ Robotics PPA
  (`add-apt-repository ppa:dqrobotics-dev/development && apt-get install libdqrobotics`)
- Note: the dqrobotics headers use `<Eigen/Dense>` without the `eigen3/`
  prefix; if that does not resolve on your system, create the symlink
  `ln -s /usr/include/eigen3/Eigen /usr/include/Eigen`.

### Using it from another CMake project

```cmake
find_package(marinholab_sas_core REQUIRED)
target_link_libraries(your_target marinholab::sas::core)
```

```cpp
#include <marinholab/sas/core/sas_clock.hpp>

marinholab::sas::core::Clock clock(0.01);
```

Or as a subdirectory / FetchContent dependency:

```cmake
include(FetchContent)
FetchContent_Declare(
    marinholab_sas_core
    GIT_REPOSITORY https://github.com/MarinhoLab/sas_cpp.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(marinholab_sas_core)

target_link_libraries(your_target marinholab::sas::core)
```

## Examples

Run them after building (all are bounded; they exit on their own):

```bash
./build/sas_core_example
./build/sas_clock_example
./build/sas_clock_sched_fifo_example
./build/thread_manager_example
./build/sas_robot_driver_example
```

## Debian package

Build the `.deb` with the usual command:

```bash
bash tools/bump-changelog.sh   # align the changelog with the rolling version
dpkg-buildpackage -us -uc -b
```

This produces `../libmarinholab-sas-core_<version>_<arch>.deb` containing the
shared library, the headers under `/usr/include/marinholab/sas/core/` and the
CMake package configuration under `/usr/lib/cmake/marinholab_sas_core/`, so
installed consumers can `find_package(marinholab_sas_core)`.

## Docker

A full build-and-test environment on `ubuntu:noble` (CMake build, running all
examples, the static-library toggle, and the Debian package) lives in
`docker/`:

```bash
cd docker
docker compose run --rm marinholab_sas_core
```