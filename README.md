# Reticula [![Actions Status][action-image]][action-link] [![Documentations][docs-badge]][docs-website] [![Paper][paper-badge]][paper-link]

*Reticula* is a general-purpose complex network analysis tool, supporting
static networks as well as [hypergraphs][hyper], [temporal networks][temp].
It allows you to study radomise networks, calculate static or
[temporal network reachability][out-component] and form [event graphs][event].

Reticula (singular: reticulum) is a word with Latin origin meaning networks or
network-like (i.e., reticulate) structures.

[action-image]: https://github.com/reticula-network/reticula/workflows/Tests/badge.svg
[action-link]: https://github.com/reticula-network/reticula/actions
[hyper]: https://arxiv.org/abs/2006.01764
[temp]: https://arxiv.org/abs/1108.1780
[event]: https://arxiv.org/abs/1709.05647
[out-component]: https://arxiv.org/abs/1908.11831
[docs-badge]: https://img.shields.io/badge/Docs-docs.reticula.network-success
[docs-website]: https://docs.reticula.network
[paper-badge]: https://img.shields.io/badge/Paper-SoftwareX-informational
[paper-link]: https://www.sciencedirect.com/science/article/pii/S2352711022002199


## Getting Started

### Documentation

The documentation is available at [reticula.network][website]

[website]: https://reticula.network

### Requirements

Reticular relies heavily on certain C++20 features, such as concepts and ranges.
If you intend on using Reticula in your project, you need a compiler with decent
support of both. This library is regularly tested on Linux (GCC 11.4.0), MacOS
(XCode 14.3, AppleClang 14.0.3.14030022) and Windows (Visual Studio 17 2022,
MSVC 19.37.32825.0), though anything more recent should probably work just as
well.

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

If you want to fetch other content using CMake FetchContent module, you can
include those in `FetchContent_MakeAvailable` call or call this function
separatly for each content.

Finally, link your target and Reticula like this:

```cmake
target_link_libraries(${TARGET_NAME} PRIVATE reticula)
```


[FetchContent]: https://cmake.org/cmake/help/latest/module/FetchContent.html

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
