#!/usr/bin/env bash
# Full build + test pipeline for marinholab_sas_core:
#   1. CMake build (shared library, the default)
#   2. Run all example executables
#   3. CMake build with the static-library toggle (-DBUILD_SHARED_LIBS=OFF)
#   4. Debian package via the usual dpkg-buildpackage -us -uc -b
#   5. Install the .deb and build a downstream consumer via find_package
set -euo pipefail
cd "$(dirname "$0")/.."

echo "=== Version: $(bash tools/version.sh) ==="

# --- 1+2: shared build (default) and examples ---
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
ls -la build/libmarinholab_sas_core.so*

for example in sas_core_example sas_clock_example sas_clock_sched_fifo_example \
               thread_manager_example sas_robot_driver_example; do
    echo "=== Running ${example} ==="
    timeout 120 "build/${example}"
done

# --- 3: static build toggle ---
echo "=== Static build (-DBUILD_SHARED_LIBS=OFF) ==="
cmake -B build-static -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF
cmake --build build-static -j"$(nproc)"
ls -la build-static/libmarinholab_sas_core.a
"build-static/sas_core_example"

# --- 4: Debian package ---
echo "=== Debian package ==="
bash tools/bump-changelog.sh
dpkg-buildpackage -us -uc -b
ls -la ../*.deb
DEB="$(ls ../*.deb | head -n1)"

# --- 5: install + downstream consumer ---
echo "=== Installing ${DEB} ==="
dpkg -i "${DEB}"
dpkg -L libmarinholab-sas-core | grep -E "libmarinholab|cmake/marinholab|include/marinholab" | head -20

echo "=== Downstream consumer (find_package + marinholab::sas::core) ==="
rm -rf /tmp/sas_consumer && mkdir -p /tmp/sas_consumer && cd /tmp/sas_consumer
cat > main.cpp <<'EOF'
#include <marinholab/sas/core/sas_clock.hpp>
#include <marinholab/sas/core/eigen3_std_conversions.hpp>
#include <iostream>

int main()
{
    marinholab::sas::core::Clock clock(0.01, false);
    clock.init();
    for (int i = 0; i < 3; i++)
        clock.update_and_sleep();
    std::cout << "consumer OK: elapsed=" << clock.get_elapsed_time_sec()
              << std::endl;
    return 0;
}
EOF
cat > CMakeLists.txt <<'EOF'
cmake_minimum_required(VERSION 3.16)
project(consumer CXX)
find_package(marinholab_sas_core REQUIRED)
add_executable(consumer main.cpp)
target_link_libraries(consumer marinholab::sas::core)
EOF
cmake -B b .
cmake --build b
./b/consumer

echo "=== ALL CHECKS PASSED ==="
