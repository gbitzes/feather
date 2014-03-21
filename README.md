# Feather

A Constraint Satisfaction Problem solver that focuses on modularity. The software architecture is designed to be flexible and extensible to allow:

* Multiple frontends - you can have the user define the constraint network in multiple ways and languages. C++, Python, Java or even interactive environments and specialized CSP languages could be used.
* Multiple problem managers - the way the search space is divided amongst the backends can be adjusted, allowing for multi-threaded, multi-process or multi-node configurations.
* Multiple backends - the modules that do the heavy lifting and give the solutions to the CSP. You could have different algorithms and implementations running on the same machine. Currently the only supported backend is a modified version of [Naxos Solver](http://cgi.di.uoa.gr/~pothitos/naxos/).

The primary way this is achieved is through pure virtual base classes which simply provide an interface that can be implemented in various ways.

### Folder structure

The project is structured as follows:

* include: public headers that should be in the include paths of the programs compiling against the library
* src: source files as well as private headers of the library
* external: folder in which to place external dependencies. Some of them might be downloaded here during the build
* tests: all tests, using Google Testing Framework
* lib: generated binaries of the library after compilation

There's some more structure inside src:

* src/base: basic types and data structures used throughout the library
* src/common: things that are shared between the frontends, the managers and the backends
* src/frontend
* src/management
* src/backend

### Distinction between private and public headers

There's clear separation between the private and the public headers. Classes which are not meant to be accessed by a user of this library are inside *src* - normally they would only need the headers in *include*. However, when extending the functionality of the library (eg when writing a new frontend) it's also necessary to add *src* to your include directories.

This is mostly done to separate the interface of the library inside the headers and reduce as much as possible the volume of code that a user needs to _#include_, so as to keep build times sane.

