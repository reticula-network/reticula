# DAG [![Actions Status][action-image]][action-link]
`dag` is a general-purpose complex network analysis tool with focus on [temporal
networks][temp], [event graphs][event] and [reachability][out-component].

[temp]: https://arxiv.org/abs/1108.1780
[event]: https://arxiv.org/abs/1709.05647
[out-component]: https://arxiv.org/abs/1908.11831


## Getting Started

Clone the library:
```bash
$ git clone https://github.com/arashbm/dag.git
```

Compile and run the tests:
```
$ cd dag
$ mkdir build
$ cd dag
$ cmake ..
$ cmake --build . --target dag_tests
$ ./dag_tests
```

[action-image]: https://github.com/arashbm/dag/workflows/Tests/badge.svg
[action-link]
