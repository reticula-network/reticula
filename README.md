# Reticula [![Actions Status][action-image]][action-link] [![Documentations][docs-badge]][docs-website] [![Paper][paper-badge]][paper-link]

*Reticula* is a general-purpose complex network analysis tool, supporting
static networks as well as [hypergraphs][hyper], [temporal networks][temp]. It
allows you to study randomise networks, calculate static or [temporal network
reachability][out-component] and form [event graphs][event].

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

Reticula relies heavily on certain C++20 features, such as concepts and ranges.
If you intend on using Reticula in your project, you need a compiler with
decent support of C++20 features.

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
separately for each content.

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
