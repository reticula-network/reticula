# AGENTS directives for reticula repository
## Build & Test
- cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo && cmake --build build -j$(nproc) -t reticula_tests
- ctest --test-dir build --output-on-failure -j$(nproc)
- ctest --test-dir build -R <test_name> --output-on-failure
## Lint & Formatting
- Follow .editorconfig: 2-space indent, LF line endings, UTF-8 charset
## Code Style
- Use modern C++20 features when appropriate
- snake_case for functions, vars, types and classes
- #ifndef guard in headers; group includes: project, third-party, std
- Use exceptions (std::runtime_error) with clear messages
- No trailing whitespace
- No lines above 80 characters

*Update AGENTS.md when build or style tools change.*
