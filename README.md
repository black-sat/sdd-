# SDD++: a modern C++ wrapper for SDD 2.0

This is a thin C++ wrapper interface for the 
[SDD 2.0 library](http://reasoning.cs.ucla.edu/sdd/), to deal with *sentential
decision diagrams* (SDD), a useful data structure to represent Boolean functions
in a succinct way. 

Compilation and installation requires a C++20 compiler, and should be simple 
enough:
```
$ git clone https://github.com/black-sat/sddpp.git
$ cd sdd++
$ mkdir build && cd build
$ cmake ..
$ make
$ make install
```

Usage of the library is straightforward using CMake, for example:
```
find_package(sdd++) 

# ...

target_link_libraries(your_target PUBLIC sdd++::sdd++)
```

Then, include `<sdd++/sdd++.hpp>`. API documentation is currently unavailable
but the usage should be simple given the API of the SDD library (see their
manual). 

The original SDD library is included. Its C API is available by including `<sdd/sdd.h>`.

