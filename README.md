# libtribe [![C++ standard][c++-standard-shield]][c++-standard-link] [![License][license-shield]][license-link]
---
[![TravisCI][travis-shield]][travis-link]

[c++-standard-shield]: https://img.shields.io/badge/c%2B%2B-14/17/20-blue.svg
[c++-standard-link]: https://en.wikipedia.org/wiki/C%2B%2B#Standardization
[travis-shield]: https://travis-ci.org/abbyssoul/libtribe.png?branch=master
[travis-link]: https://travis-ci.org/abbyssoul/libtribe
[license-shield]: https://img.shields.io/badge/License-Apache%202.0-blue.svg
[license-link]: https://opensource.org/licenses/Apache-2.0

C++ _library_ for Group Membership and Failure Detection using gossip style protocol.
> library: a collection of types, functions, classes, etc. implementing a set of facilities (abstractions) meant to be potentially used as part of more that one program. From [Cpp Code guidelines gloassay](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#glossary)

The protocol is based on ["SWIM: Scalable Weakly-consistent Infection-style Process Group Membership Protocol"](http://ieeexplore.ieee.org/document/1028914/)
paper and some ideas implemented in Hashicorp's [Memberlist](https://github.com/hashicorp/memberlist)
Go library although implementations are independent and not compatible.


In distributed systems participants may not be pre-determined. It is not sufficient to have a static list of nodes forming a cluster.
This property is critical as individual nodes may be taken out for service or replace.
This also enables scalability of the cluster as new nodes can be added as required if the task can benefit from new nodes.
Given that nodes can join and leave the group unpredictably, it is required for a given node to keep track of its peers.

On the other hand it may not be practical to keep track of all the members for extremely large clusters. If nodes join and leave group at a steady rate and each such event requires message exchange - number of such messages will grow with the size of a cluster limiting available bandwidth resources required for computation.

It may also be beneficial to use membership protocol on a resource constrained systems aka IoT devices.

That is why this implementation of SWIM includes a notion of peer capacity - that is maximum number of peers one given node can have / track. Note that this setting not required for to be the same for each peer. This trade-off makes this implementation a weakly consistent/ probability based as it it possible for a given node to be tracked only by one other peer to be consider a part of the group.  

Key observation here is that for a node to be part of a group it must be in peer list of at list one other node. It is not enough for a node to know other peers. This peers must know about that node as 'connections' are symmetrical.

See [docs](docs/intro.md) for more information about project motivation and [protocol](docs/protocol.md) design.


# Usage
The library is designed to provide components for building distributed applications.
It does not provide networking. That is user responsibility to provide IO.
For example when a message is received or read from a file - user can pass this message
to the message handler to possibly update an estimated state of the cluster.
The update function in turn produces messages that library user must dispatch to the network.


```C++
#include <tribe/protocol.hpp>  // Convenience header for message builders and parser
#include <tribe/membership.hpp>   // Group membership model

//... inside your IO service that handles datagrams
tribe::PeersModel handleDatagram(tribe::PeersModel model, Solace::MemoryView msgBuffer) {
  auto maybeMessage = tribe::Gossip::MessageParser{}
          .parse(reader);

  if (!maybeMessage) {  // Data in the buffer does not constitute a valid gossip message.
      return false;
  }

  return tribe::update(model, *maybeMessage);
}

```

## Consuming library with conan
There is a [Conan](https://conan.io/) for this library.
If your project is using for Conan for dependency management you can add `libtribe` to your conanfile.txt:

```
[requires]
libtribe/0.0.1@abbyssoul/stable
```

While the library is not available in the conan-central repository - you need to use:
```
    conan remote add <REMOTE> https://api.bintray.com/conan/abbyssoul/public-conan
```

Please check the latest available [binary version](https://bintray.com/abbyssoul/public-conan/libsolace%3Aabbyssoul/_latestVersion).


# Building
Project build is managed by CMake with some make-files only to automate basic operations and drive CMake.

# External dependencies
The library depends on [libsolace](https://github.com/abbyssoul/libsolace) for low level data manipulation primitives
such as ByteReader/ByteWriter and Result<> type.

### GTest
Note test framework used is *gtest* and it is managed via git modules.
Don't forget to do `git submodule update --init --recursive` on a new checkout to pull sub-module dependencies.


### Tool dependencies
In order to build this project following tools must be present in the system:
* git (to check out project and it’s external modules, see dependencies section)
* doxygen (for documentation)
* cppcheck (static code analysis, latest version from git is used as part of the 'codecheck' step)
* cpplint (for static code analysis in addition to cppcheck)
* valgrind (for runtime code quality verification)

This project is using C++17 features extensively. The minimal tested/required version of gcc is gcc-7.
[CI](https://travis-ci.org/abbyssoul/libtribe) is using clang-6 and gcc-7.
To install build tools on Debian based Linux distribution:
```shell
sudo apt-get update -qq
sudo apt-get install git doxygen python-pip valgrind ggcov
sudo pip install cpplint
```

The library has one external dependency: [libsolace](https://github.com/abbyssoul/libsolace)  which is managed via conan.
Please make sure [conan is installed](https://docs.conan.io/en/latest/installation.html) on your system if you want to build this project.

## Building the library
```shell
# In the project check-out directory:
# To build debug version with sanitizer enabled (recommended for development)
./configure --enable-debug --enable-sanitizer

# To build the library it self
make

# To build and run unit tests:
make test

# To run valgrind on test suit:
# Note: `valgrind` doesn’t work with ./configure --enable-sanitize option
make verify

# To build API documentation using doxygen:
make doc
```

To install locally for testing:
```shell
make --prefix=/user/home/<username>/test/lib install
```
To install system wide (as root):
```shell
make install
```
To run code quality check before submission of a patch:
```shell
# Verify code quality before submission
make codecheck
```

## Building examples
Library also has some basic usage examples that can be found in 'examples' subdirectory.

To build all examples run from the based directory:
```shell
# Verify code quality before submission
make examples
```


## Contributing changes
This framework is work in progress and contributions are very welcomed.
Please see  [`CONTRIBUTING.md`](CONTRIBUTING.md) for details on how to contribute to this project.

Note that in order to maintain code quality a set of static code analysis tools is used as part of the build process.
Thus all contributions must be verified by this suite of tools before PR can be accepted.


## Licensing
The library available under Apache License 2.0
Please see [`LICENSE`](LICENSE) for details.

## Authors
Please see [`AUTHORS`](AUTHORS) file for the list of contributors.
