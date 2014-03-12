# Feather

A Constraint Satisfaction Problem solver that focuses on modularity. The software architecture is designed to be flexible and extensible to allow:

* Multiple frontends - you can have the user define the constraint network in multiple ways and languages. C++, Python or even interactive environments or specialized CSP languages could be used.
* Multiple problem managers - the way the search space is divided amongst the backends can be adjusted, allowing for multi-core, multi-process or multi-node configurations.
* Multiple backends - the modules that do the heavy lifting and give the solutions to the CSP. You could have different algorithms and implementations running on the same machine.

The primary way this is achieved is through pure virtual base classes which simply provide an interface that can be implemented in various ways.

### Folder structure

The project is structured as follows:

* ./include: public headers that should be in the include paths of the programs compiling against the library
* ./src: source files as well as private headers of the library
* ./external: folder in which to place external dependencies. Some of them might be downloaded here during the build
* ./tests: all tests, using Google Testing Framework
* ./lib: generated binaries of the library after compilation

There's some more structure inside ./src:

* ./src/base: basic types and data structures used throughout the library
* ./src/common: things that are shared between the frontends, the managers and the backends
* ./src/frontends
* ./src/managers
* ./src/backends
