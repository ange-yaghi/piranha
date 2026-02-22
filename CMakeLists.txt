cmake_minimum_required(VERSION 3.10)

# Set CMake policies for compatibility
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

# Enable group projects in folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

project(piranha)

set(CMAKE_CXX_STANDARD 17)

# =========================================================
# Platform Detection and Cross-Platform Support
# =========================================================

# Platform detection
if(WIN32)
    set(PLATFORM_NAME "Windows")
elseif(APPLE)
    set(PLATFORM_NAME "macOS")
    # Detect Apple Silicon
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        set(PLATFORM_APPLE_SILICON ON)
        message(STATUS "Detected Apple Silicon (M1/M2/M3/M4)")
    else()
        set(PLATFORM_INTEL_MAC ON)
        message(STATUS "Detected Intel Mac")
    endif()
elseif(UNIX)
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(PLATFORM_NAME "Linux")
    elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        set(PLATFORM_NAME "FreeBSD")
    elseif(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
        set(PLATFORM_NAME "OpenBSD")
    elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
        set(PLATFORM_NAME "NetBSD")
    else()
        set(PLATFORM_NAME "Generic UNIX")
    endif()
endif()

message(STATUS "Platform: ${PLATFORM_NAME}")

# =========================================================
# Find Bison 3.2+ (Critical Requirement)
# =========================================================

# Platform-specific Bison paths
if(APPLE)
    # Homebrew paths (both Intel and Apple Silicon)
    find_program(BISON_EXECUTABLE bison
        PATHS
            /opt/homebrew/opt/bison/bin
            /usr/local/opt/bison/bin
            /opt/homebrew/bin
            /usr/local/bin
        NO_DEFAULT_PATH
    )
    if(NOT BISON_EXECUTABLE)
        find_program(BISON_EXECUTABLE bison)
    endif()
elseif(UNIX)
    find_program(BISON_EXECUTABLE bison)
endif()

# Verify Bison version
if(BISON_EXECUTABLE)
    execute_process(
        COMMAND "${BISON_EXECUTABLE}" --version
        OUTPUT_VARIABLE BISON_VERSION_OUTPUT
        ERROR_VARIABLE BISON_VERSION_OUTPUT
    )
    if(BISON_VERSION_OUTPUT MATCHES "bison \\(GNU Bison\\) ([0-9]+)\\.([0-9]+)")
        set(BISON_MAJOR ${CMAKE_MATCH_1})
        set(BISON_MINOR ${CMAKE_MATCH_2})
        set(BISON_VERSION "${BISON_MAJOR}.${BISON_MINOR}")

        if(BISON_MAJOR LESS 3 OR (BISON_MAJOR EQUAL 3 AND BISON_MINOR LESS 2))
            message(FATAL_ERROR
                "Bison ${BISON_VERSION} is too old. Piranha requires 3.2+.\n"
                "Install via:\n"
                "  macOS: brew install bison\n"
                "  Linux: sudo apt install bison (Ubuntu) or sudo dnf install bison (Fedora)\n"
                "  BSD: sudo pkg install bison (FreeBSD) or pkg_add bison (OpenBSD)"
            )
        endif()
        message(STATUS "Found Bison ${BISON_VERSION}")
    endif()
else()
    message(FATAL_ERROR "Bison not found. Install Bison 3.2+")
endif()

# =========================================================
# Find Flex
# =========================================================

find_program(FLEX_EXECUTABLE flex)
if(FLEX_EXECUTABLE)
    message(STATUS "Found Flex: ${FLEX_EXECUTABLE}")
endif()

# =========================================================
# Platform-specific Boost configuration
# =========================================================

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

if(APPLE)
    # Homebrew Boost paths
    if(EXISTS "/opt/homebrew")
        list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    endif()
    if(EXISTS "/usr/local")
        list(APPEND CMAKE_PREFIX_PATH "/usr/local")
    endif()
elseif(UNIX)
    # Linux/BSD - CMake finds Boost in standard locations
endif()

find_package(Boost 1.40 COMPONENTS filesystem REQUIRED)
message(STATUS "Found Boost ${Boost_VERSION}")

# =========================================================
# Platform-specific compiler flags
# =========================================================

if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
    add_compile_options(-Wall -Wextra)

    if(APPLE)
        # macOS-specific: avoid warnings
        add_compile_options(-Wno-unused-command-line-argument)
    endif()

    # Flex compatibility with C++17
    add_compile_options(-Wno-register)
endif()

# =========================================================
# GTEST

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set_property(TARGET gmock PROPERTY FOLDER "gtest")
set_property(TARGET gmock_main PROPERTY FOLDER "gtest")
set_property(TARGET gtest PROPERTY FOLDER "gtest")
set_property(TARGET gtest_main PROPERTY FOLDER "gtest")

# =========================================================
# FLEX BISON

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

flex_target(
    lexer
    flex-bison/scanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.auto.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/scanner.auto.h)

bison_target(
    parser
    flex-bison/specification.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.auto.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.auto.h)
add_flex_bison_dependency(lexer parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(dependencies/libraries/flex/include)
include_directories(include)

add_library(piranha STATIC
    # Source files
    src/assembly.cpp
    src/channel_node.cpp
    src/channel_type.cpp
    src/compilation_error.cpp
    src/compiler.cpp
    src/error_list.cpp
    src/float_conversions.cpp
    src/fundamental_types.cpp
    src/int_conversions.cpp
    src/ir_attribute.cpp
    src/ir_attribute_definition.cpp
    src/ir_attribute_definition_list.cpp
    src/ir_attribute_list.cpp
    src/ir_binary_operator.cpp
    src/ir_compilation_unit.cpp
    src/ir_context_tree.cpp
    src/ir_import_statement.cpp
    src/ir_node.cpp
    src/ir_node_definition.cpp
    src/ir_parser_structure.cpp
    src/ir_unary_operator.cpp
    src/ir_value.cpp
    src/language_rules.cpp
    src/memory_tracker.cpp
    src/node.cpp
    src/node_allocator.cpp
    src/node_container.cpp
    src/node_graph.cpp
    src/node_output.cpp
    src/node_program.cpp
    src/no_op_node.cpp
    src/path.cpp
    src/standard_allocator.cpp
    src/string_conversions.cpp
    src/vector_constructor.cpp

    # Header files
    include/piranha.h
    include/add_operation_output.h
    include/assembly.h
    include/bool_negate_operation.h
    include/build_settings.h
    include/channel_node.h
    include/channel_type.h
    include/compilation_error.h
    include/compiler.h
    include/console_input_node.h
    include/console_output_node.h
    include/default_literal_node.h
    include/divide_operation_output.h
    include/error_list.h
    include/exceptions.h
    include/float_conversions.h
    include/fundamental_output.h
    include/fundamental_types.h
    include/int_conversions.h
    include/int_probe.h
    include/ir_attribute.h
    include/ir_attribute_definition.h
    include/ir_attribute_definition_list.h
    include/ir_attribute_list.h
    include/ir_binary_operator.h
    include/ir_compilation_unit.h
    include/ir_context_tree.h
    include/ir_import_statement.h
    include/ir_literal_node.h
    include/ir_node.h
    include/ir_node_definition.h
    include/ir_parser_structure.h
    include/ir_structure_list.h
    include/ir_token_info.h
    include/ir_unary_operator.h
    include/ir_value.h
    include/ir_value_constant.h
    include/ir_visibility.h
    include/key_value_lookup.h
    include/language_rules.h
    include/literal_node.h
    include/literal_node_output.h
    include/memory_management.h
    include/memory_tracker.h
    include/multiply_operation_output.h
    include/node.h
    include/node_allocator.h
    include/node_container.h
    include/node_graph.h
    include/node_output.h
    include/node_program.h
    include/no_op_node.h
    include/num_negate_operation.h
    include/operation_node.h
    include/path.h
    include/pipe_node.h
    include/pkey_value_lookup.h
    include/rule.h
    include/scanner.h
    include/standard_allocator.h
    include/string_conversions.h
    include/subtract_operation_output.h
    include/throw_runtime_error_node.h
    include/vector_constructor.h
    include/vector_output.h
    include/vector_split.h
    include/vector_split_node.h
    include/version.h

    # Autogen
    ${BISON_parser_OUTPUTS}
    ${FLEX_lexer_OUTPUTS}
)

add_executable(piranha_reference_compiler
    # Source files
    demos/main.cpp
    demos/reference_language_rules.cpp
    demos/utilities.cpp

    # Include files
    demos/utilities.h
    demos/reference_language_rules.h
)

target_link_libraries(piranha_reference_compiler piranha)

# gtest

enable_testing()

add_executable(piranha_test
    test/compilation_tests.cpp
    test/general_tests.cpp
    test/operator_tests.cpp
    test/optimization_tests.cpp
    test/runtime_tests.cpp
    test/sdl_stress_tests.cpp
    test/sdl_tests.cpp
    test/test_rules.cpp
    test/utilities.cpp
)

target_link_libraries(piranha_test
    gtest_main
    piranha
)

include(GoogleTest)
gtest_discover_tests(piranha_test)

set_property(TARGET piranha PROPERTY FOLDER "piranha")
set_property(TARGET piranha_reference_compiler PROPERTY FOLDER "piranha")
set_property(TARGET piranha_test PROPERTY FOLDER "piranha")
