# Reticula [![Actions Status][action-image]][action-link]
*Retucla* is a general-purpose complex network analysis tool with focus on
[hypergraphs][hyper], [temporal networks][temp], [event graphs][event] and
[reachability][out-component].

[action-image]: https://github.com/reticula-network/reticula/workflows/Tests/badge.svg
[action-link]: https://github.com/reticula-network/reticula/actions
[hyper]: https://arxiv.org/abs/2006.01764
[temp]: https://arxiv.org/abs/1108.1780
[event]: https://arxiv.org/abs/1709.05647
[out-component]: https://arxiv.org/abs/1908.11831


## Getting Started
### Requirements

Reticular relies heavily on certain C++20 features, such as concepts and ranges.
If you intend on using Reticula in your project, you need a compiler with decent
support of both. This library is regularly tested on GCC version 10.

### Including reticula in your CMake project

You can use [FetchContent][FetchContent] to include Reticula in your C++
project. For example:

```cmake
include(FetchContent)

FetchContent_Declare(
  reticula
  GIT_REPOSITORY https://github.com/reticula-network/reticula.git
  GIT_TAG ${COMMIT_HASH})

FetchContent_MakeAvailable(reticula)
```

You can use a git tag or branch name or the hash of a specific commit. We
recommand that you use the most recent release branch `v#.#` for a new project.

If you want to fetch other content using CMake FetchContent module, you can
include those in `FetchContent_MakeAvailable` call or call this function
separatly for each content.


[FetchContent]: https://cmake.org/cmake/help/latest/module/FetchContent.html

### Documentation


## Development
### Running the test suit

Clone the library:
```bash
$ git clone https://github.com/reticula-network/reticula.git
```

Compile and run the tests:
```
$ cd reticula
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build . --target reticula_tests
$ ./reticula_tests
```

