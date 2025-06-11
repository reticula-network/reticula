Coding conventions and testing instructions for Codex:

- When you modify code, ensure all tests are executed.
- Configure a build directory named `build` and compile the test target with:
  `cmake --build build --target reticula_tests -j $(nproc)`
- Run the test suite with:
  `ctest --test-dir build --output-on-failure -j $(nproc)`
