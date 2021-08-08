![Alt text](docs/public/banner_v3.png?raw=true)


**Piranha** is an open-source scripting framework currently under development. It is a reusable framework which allows developers to quickly write scripting interfaces to native C++ libraries.

Piranha is currently used as an SDL (scene description language) for the [MantaRay](https://github.com/ange-yaghi/manta-ray) ray-tracer.

For syntax highlighting in Visual Studio Code check out the [Piranha Visual Studio Code Extension](https://github.com/ange-yaghi/piranha-vscode-extension).

To get started, check out the introductory handbook: [Piranha Handbook](docs/handbook/handbook.md)

Example Piranha compiler implementation: [Hello World Piranha Compiler](https://github.com/ange-yaghi/piranha-hello-world-compiler)

## Design Scripting Interfaces For Your Native Libraries

Write interfaces to your native code with full type-checking, syntax checking and error reporting, syntax highlighting, support for importing libraries and more!

![Alt text](docs/public/example_1.png?raw=true)

## Design Robust Configuration Languages

Skip writing your own parser and implement your own custom configuration language using Piranha. Piranha supports advanced features like binary and unary operations, custom functions and variables which can simplify tasks like writing configuration files.

![Alt text](docs/public/example_2.png?raw=true)

## Design Complex Node-Based Systems

Piranha was designed for the unique demands of a ray-tracer SDL (scene description language) where a user may want to write shaders, specify multiple render jobs in a particular sequence, perform image post processing and a number of other operations. Custom logic written in Piranha has a comparable runtime to native code!

![Alt text](docs/public/example_3.png?raw=true)
 
## Setup Instructions For Developers

Only a few steps are required to begin contributing to Piranha.

### Install Dependencies
I have tried to rely on as few dependencies as possible and at some point may eliminate these dependencies as well, but for now you'll have to live with the pain. Install the following:

1. CMake
2. Boost (make sure to build the optional dependencies as well)
3. Flex
4. Bison

### Build with CMake
After cloning the Piranha repository, `cd` into the repository and run the following commands:

```
mkdir build
cd build
cmake ..
cmake --build .
```

If using MSVC, to set whether it is a Release or Debug build, run the last step like this:
```
cmake --build . --config Release
```

If using MSVC, CMake will output a Visual Studio solution which you can use for development and debugging. You can also try running the reference compiler directly `piranha_reference_compiler`.

**You are now ready to begin development!**
