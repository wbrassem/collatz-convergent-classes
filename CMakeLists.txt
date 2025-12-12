# =========================================================
# Collatz Convergent Classes - CMake build
# =========================================================

cmake_minimum_required(VERSION 3.0)
project(Collatz VERSION 1.0 LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ======================================================================
# 1. Source and header files
# ======================================================================
set(CPP_SRC
    src/cpp/main.cpp
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
# 3. Create executable
# ======================================================================
add_executable(menu ${CPP_SRC} ${CPP_HDR})

target_link_libraries(menu PRIVATE
    ${LIBGMP}
    ${LIBGMPXX}
)

# Optional: place the executable in a separate top-level bin directory
set_target_properties(menu PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

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
