/**
 * @file common.hpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief Common header file which includes references to standard header files and general use classes and templates
 * 
 * Multiple Precision integer header file switch
 * ---------------------------------------------
 * This header file includes a compiler switch `gnu_mp` to control whether or not multiple precision integers
 * are in use or not.  The multiple precision integers leverage the GNU Multiple Precision libraries.
 * 
 * GNU Multiple Precision libraries for C and C++
 * ----------------------------------------------
 * Source files found at https://gmplib.org.
 * This implementation was built from `gmp-6.2.1.tar.xz` (expanded with `tar -xvf gmp-6.2.1.tar.xz`).
 * Source code originally developed on a Mac Darwin environment.
 * 
 * The default build has C++ disabled - so by default only the C header file and library is installed.
 * This project utilizes classes and thus requires this to be enabled (see `./configure` flag below).
 * - C   implementation utilizes `<gmp.h>`   and `<libgmp.dylib>`
 * - C++ implementation utilizes `<gmpxx.h>` and `<libgmpxx.dylib>`
 * 
 * The C++ library leverages components of the C library (but not the other way), so both must be built together:
 * - `cd gmp-6.2.1`
 * - `sudo ./configure --enable-cxx=detect`
 * - `sudo make`
 * - `sudo make install`
 * 
 * Installation and usage of GNU MP libraries and headers
 * ------------------------------------------------------
 * Default installation `path` for Darwin was `/usr/local`.
 * Libraries were installed in `{path}/lib`.
 * Header files were stored in `{path}/include` (which were placed there manually).
 * 
 * The linking requires the `-lgmp` and `-lgmpxx` flags to be present, and when compiling the following headers:
 * - `#include <gmp.h>          // C   header file`
 * - `#include <gmpxx.h>        // C++ header file (which in turn calls C header file <gmp.h>)`
 *  
 * @version 0.1
 * @date 2023-04-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

// Wrapper to prevent duplication if header included twice
#if !defined common_hpp
#define common_hpp

#include <stdio.h>          // Because printf() just plain rocks, no apologies, C++ just can't top this
#include <stdint.h>         // To provide a definition for int32_t if it doesn't already
#include <string>           // Because it is (somewhat) better and definitely safer than char* 
#include <iostream>         // Because people expect ostream compliance, so it's a fair expectation
#include <stdlib.h>         // For the exit() function
#include <time.h>           // Used for timing execution of program components
#include <vector>           // Vector class definitions

// Below is compiler directive switch used to control whether or not you wish to compile with GNU multiple precision or not

/**
 * @def gnu_mp
 * @brief The gnu_mp define controls whether or not the compilation and linking of the executable utilizes the GNU
 * multiple precision libraries or not.  If this switch is not enabled some features are disabled since practical
 * considerations of the built-in integer types become too restrictive.
 */
#define gnu_mp              // Comment this out if you do not wish to compile using the GNU mulitple precision libraries

#ifdef gnu_mp
#include <gmpxx.h>          // C++ header file (which in turn calls C header file <gmp.h>)
#endif

// Class forward declarations
class btree;
template < class K > class t_btree;
void menu();

/**
 * @brief Returns the number of digits in the base 10 representation of an integer of type P
 * @tparam P - An integer type be that built-in (e.g., int, long) or multiple precision ones.
 * @param [in] integer - The integer to determine the number of base 10 digits.
 * @return int - The number of base 10 digits in the integer.
 */
template < class P >
int base10_digits( P integer )
{
    int base_ten_digits = 1;           // There is at least one digit and the loop below terminates with one digit remaining

    // Calculate the base 10 maximum number of digits in convergent segment
    while ( integer / 10 != 0 )
    {
        integer /= 10;
        base_ten_digits++;
    }

    return base_ten_digits;
}

/**
 * @brief The statics class is used for holding globally utilized constants
 * @details The values are stored in this class so as to avoid propgoating a large number of "constants" throughout
 * the implementation and so that they have a namespace in an effort to avoid name collisions.  These values can
 * be changed if you wanted to investigate other Connnection conditions other than Collatz 3n+1 ones.
 * 
 * Some values such as speed, blip_modulus and count change programmatically depending on what is asked for.
 * 
 * The initial values are defined in menu.cpp which is odd.  Perhaps eventually this class will be moved to menu.hpp.
 */
class statics
{
    public:
        // Collatz Conjecture connection parameters
        static int divisor;                             /**< Collatz divisor = 2 */
        static int multiplier;                          /**< Collatz multiplier = 3 */
        static int addend;                              /**< Collatz addend = 1 */

        // Error codes
        static int overflow;                            /**< Error mask code 0x1 */
        static int memory;                              /**< Error mask code 0x2 */

        // Global flags
        static bool speed;                              /**< Speed flag (boolean) for execution which takes shortcuts */
        static int  blip_modulus;                       /**< Integer which detmineds how often to display progress blip */

        // Print control values
        static int count;                               /**< Number of digits in base 10 representation */
        static int width;                               /**< Print width control, currently not used */
};

// Global typdefs and constants

typedef unsigned long ulong;                            /**< This typedef provides a shorthand notation 'ulong' in place of 'unsigned long'. */

// Global Templates

/**
 * @brief Templated implementation of the sgn() function
 * @tparam T - Ordinal type supporting >, < and subtraction operations.
 * @param [in] integer - The integer to analyze.
 * @return int - Returns -1 if negative, 1 if positive and 0 if equal to 0.
 */
template <typename T> int sgn(T integer)
{
    return ( T(0) < integer ) - ( T(0) > integer );
}

#endif      // #if !defined common_hpp
