# DAG
`dag` is a general-purpose complex network analysis tool with focus on [temporal
networks][temp] and [event graphs][event] and [reachability][out-component].

[temp]: https://arxiv.org/abs/1108.1780
[event]: https://arxiv.org/abs/1709.05647
[out-component]: https://arxiv.org/abs/1908.11831


## Getting Started

Clone the library:
```bash
$ git clone --recursive https://github.com/arashbm/dag.git
```

Compile the library and run the tests:
```
$ cd dag
$ make
$ ./tests
```

## Wishlist

This repository would be in active development for quite a while, I would be
waiting for certain new c++20 features for certain use-cases since I don't have
any need for them right now but they are good to have:

* [Spaceship operator `<=>` and `<compare>`][spaceship] for strong ordering of
  temporal edges.
* [Empty type optimization `[[no_unique_address]]`][no-unique-address] to allow
  all edges to contain optional data without suffering a few bytes of overhead
  or messy code for edges that don't have additional data.

[spaceship]: https://en.cppreference.com/w/cpp/language/default_comparisons
[no-unique-adress]: https://en.cppreference.com/w/cpp/language/attributes/no_unique_address
`
