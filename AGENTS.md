# Reticula C++ Library - Agent Guidelines

## Build Commands
- **Build**: `cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j$(nproc)`
- **Build with benchmarks**: `cmake -B build-release -DCMAKE_BUILD_TYPE=Release -DRETICULA_BUILD_BENCHMARKS=ON && cmake --build build-release -j$(nproc)`
- **Test all**: `cmake --build build --target test -j$(nproc)` or `ctest --test-dir build -j$(nproc)`
- **Test single**: `ctest --test-dir build -R <test_name>` (e.g., `-R "directed"`)
- **Run benchmarks**: `./build-release/benchmarks/reticula_benchmarks` or `cmake --build build-release --target run_benchmarks`
- **Clean**: `rm -rf build/` or `rm -rf build-release/`

## Code Style
- **Standard**: C++23 with extensions disabled
- **Format**: LLVM-based with 2-space indentation, left pointer alignment
- **Linting**: clang-tidy with cppcoreguidelines, modernize checks (warnings as errors)
- **Headers**: Use `#pragma once`, include project headers with `<reticula/...>`
- **Naming**: snake_case for functions,variables and types, trailing underscore for private members
- **Namespaces**: All code in `reticula` namespace
- **Templates**: Use concepts for constraints, prefer `std::ranges` and views
- **Testing**: Catch2 framework with descriptive test names and sections

## Key Patterns
- Template-heavy library with concepts for type safety
- Extensive use of C++20/23 features (ranges, concepts, spans)
- Header-only interface with implementation in .cpp files for explicit instantiation
- Network/graph algorithms with generic edge types (directed, undirected, temporal, hyper)
