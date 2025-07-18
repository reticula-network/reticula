# AGENTS directives for reticula repository
## Build & Test
- cmake -S . -B build && cmake --build build -j$(nproc)
- ctest --test-dir build --output-on-failure -j$(nproc)
- ctest --test-dir build -R <test_name> --output-on-failure
## Lint & Formatting
- clang-format -i <files>
- cpplint --filter=-legal/copyright <files>
- Adhere to CPPLINT.cfg for cpplint rules
- clang-tidy --fix <files>
- Follow .editorconfig: 2-space indent, LF line endings, UTF-8 charset
## Code Style
- Use modern C++20 features when appropriate
- snake_case for functions, vars, types and classes
- #ifndef guard in headers; group includes: project, third-party, std
- Use exceptions (std::runtime_error) with clear messages
- No trailing whitespace; ensure final newline

*Update AGENTS.md when build or style tools change.*
