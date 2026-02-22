![Alt text](docs/public/banner_v3.png?raw=true)


**Piranha** is an open-source scripting framework currently under development. It is a reusable framework which allows developers to quickly write scripting interfaces to native C++ libraries.

Piranha is currently used as an SDL (scene description language) for the [MantaRay](https://github.com/ange-yaghi/manta-ray) ray-tracer.

For syntax highlighting in Visual Studio Code check out the [Piranha Visual Studio Code Extension](https://github.com/ange-yaghi/piranha-vscode-extension).

For syntax highlighting in Sublime Text check out the [Piranha Sublime Text Package](https://github.com/Kostasn2/piranha-syntax-highlighting-package).

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

Piranha requires the following dependencies:
- **CMake** (3.10 or later)
- **Boost** (1.40 or later) - with filesystem component
- **Flex** (lexical analyzer generator)
- **Bison** (3.2 or later) - parser generator

#### Platform-Specific Installation

**macOS (Intel & Apple Silicon M1/M2/M3/M4)**
```bash
# Install all dependencies via Homebrew
brew install cmake boost bison flex

# Ensure Bison 3.2+ is available in PATH
# Homebrew installs to /opt/homebrew/opt/bison/bin (Apple Silicon)
# or /usr/local/opt/bison/bin (Intel)
```

**Linux (Ubuntu/Debian)**
```bash
sudo apt update
sudo apt install cmake libboost-all-dev bison flex
```

**Linux (Fedora/RHEL)**
```bash
sudo dnf install cmake boost-devel bison flex
```

**FreeBSD**
```bash
sudo pkg install cmake boost-libs bison flex
```

**OpenBSD**
```bash
pkg_add cmake bison flex
# Boost may need to be installed from ports
```

**Windows (MSVC)**
```bash
# Use vcpkg for dependencies
vcpkg install cmake boost-filesystem bison flex
```

### Build with CMake

After cloning the Piranha repository and installing dependencies:

**macOS, Linux, BSD (Unix-like systems)**
```bash
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)
```

**Windows (MSVC)**
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Platform Detection

The CMake build system automatically detects your platform:
- **Windows**: MSVC compiler
- **macOS**: AppleClang with Apple Silicon (M1/M2/M3/M4) or Intel Mac detection
- **Linux**: GCC or Clang
- **BSD**: Clang or GCC

### Build Output

Upon successful build, you'll find:
- `libpiranha.a` - Static library
- `piranha_reference_compiler` - Example compiler implementation
- `piranha_test` - Test suite

### Running Tests

```bash
cd build
ctest --output-on-failure
```

**You are now ready to begin development!**
