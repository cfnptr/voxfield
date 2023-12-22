#!/bin/bash
cd "$(dirname "$BASH_SOURCE")"

cmake --version > /dev/null
status=$?

if [ $status -ne 0 ]; then
    echo "Failed to get CMake version, please check if it's installed."
    exit $status
fi

echo ""
echo "Configuring project..."

if [[ "$OSTYPE" == "msys" ]]; then
    cmake -DCMAKE_BUILD_TYPE=Release -T ClangCL -A x64 -S . -B build/
else
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang-cpp -S . -B build/
fi

status=$?

if [ $status -ne 0 ]; then
    echo "Failed to configure CMake project."
    exit $status
fi

echo ""
echo "Building project..."

threadCount=$(nproc)
cmake --build build/ --config Release -j $threadCount
status=$?

if [ $status -ne 0 ]; then
    echo "Failed to build CMake project."
    exit $status
fi

exit 0