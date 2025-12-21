# =========================================================
# Collatz Convergent Classes - CMake build
# =========================================================

cmake_minimum_required(VERSION 3.16)
project(Collatz VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ======================================================================
# 1. Source and header files
# ======================================================================
set(CPP_SRC
    src/cpp/btree.cpp
    src/cpp/menu.cpp
    src/cpp/oeis.cpp
    # src/cpp/path.cpp   # Uncomment if used
)

set(CPP_HDR
    src/cpp/btree.hpp
    src/cpp/common.hpp
    src/cpp/oeis.hpp
    src/cpp/path.hpp
)

# ======================================================================
# 2. Find libraries
# ======================================================================
find_library(LIBGMP NAMES gmp libgmp)
find_library(LIBGMPXX NAMES gmpxx libgmpxx)

# ======================================================================
# 3. Create executables
# ======================================================================
# Menu executable
add_executable(menu src/cpp/main.cpp ${CPP_SRC})

target_link_libraries(menu PRIVATE
    ${LIBGMP}
    ${LIBGMPXX}
)

# Batch data generation executable
add_executable(data_gen src/cpp/data_gen.cpp ${CPP_SRC})

target_link_libraries(data_gen PRIVATE
    ${LIBGMP}
    ${LIBGMPXX}
)

# The menu executable requires C++20 for <bit> and endian functions
target_compile_features(menu PUBLIC cxx_std_20)
target_compile_features(data_gen PUBLIC cxx_std_20)

# Place the executable in a separate top-level bin directory
set_target_properties(menu PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

    set_target_properties(data_gen PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ======================================================================
# 4. Doxygen documentation
# ======================================================================
find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Doxygen documentation"
        VERBATIM
    )

    # Optional: place Doxygen HTML in docs/doxygen
    set(DOXYGEN_HTML_OUTPUT ${CMAKE_SOURCE_DIR}/docs/doxygen)
endif()
