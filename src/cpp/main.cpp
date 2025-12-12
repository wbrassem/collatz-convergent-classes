/**
 * @file main.cpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief Calls the menu functions
 * @version 0.1
 * @date 2023-05-13
 * 
 * @copyright Copyright (c) 2023
 * 
 * @mainpage Collatz Conjecture Code
 * The purpose of this code was to provide empirical validation while developing the formal proof.  The scale of the
 * integers being investigated quickly outgrew the ability to perform computational analysis by hand.  It them outgrew the
 * standard built-in integer representations (yes, even 64-bit integers were insufficient) so it became evident that having
 * the ability to handle numbers well beyond 64-bit intgers was needed.
 * 
 * The GNU multiple precision integer implementation did the trick.  I will be forever in their debt because the implementation
 * is simply spectacular.  They even went ahead and offered a C++ wrapper which is my personal language of choice because of the
 * way it encapsulates and protects data entries while offering the ability for derived classes to inherit valuable behaviours
 * without endangering the base classes.
 * 
 */

#include "common.hpp"
#include "oeis.hpp"
#include "path.hpp"

// Since internal integer representations varies with hardware platform these routines help determine that experimentally

/**
 * @brief Find out where int becomes negative
 * @details Starting at 1 keeps doubling until it wraps around to negative
 */
void intblowup()
{
    int i = 1;
    int exponent = 0;

    std::cout << "\nintblowup()" << std::endl;
    do
    {
        exponent++;
        i *= 2;
        std::cout << "i = 2^" << exponent << " = " << i << std::endl;
    } while ( i>0 );

    std::cout << "thar she blows: max digits is " << base10_digits( i/2) << "\n\n";
}

/**
 * @brief Find out where long becomes negative
 * @details Starting at 1 keeps doubling until it wraps around to negative
 */
void longblowup()
{
    long i = 1;
    int exponent = 0;

    std::cout << "\nlongblowup()" << std::endl;
    do
    {
        exponent++;
        i *= 2;
        std::cout << "i = 2^" << exponent << " = " << i << std::endl;
    } while ( i>0 );

    std::cout << "thar she blows: max digits is " << base10_digits( i/2) << "\n\n";
}

/**
 * @brief Find out where long long becomes negative
 * @details Starting at 1 keeps doubling until it wraps around to negative
 */
void longlongblowup()
{
    long long i = 1;
    int exponent = 0;

    std::cout << "\nlonglongblowup()" << std::endl;
    do
    {
        exponent++;
        i *= 2;
        std::cout << "i = 2^" << exponent << " = " << i << std::endl;
    } while ( i>0 );

    std::cout << "thar she blows: max digits is " << base10_digits( i/2) << "\n\n";
}

/**
 * @brief Code to exercise the orbit_t and orbit_node_t classes. * 
 */
void orbit_test()
{
    std::cout << "\norbit test" << std::endl;

    // Force scoping
    if (true) {
        orbit_t t1, t2;

        for (int i=0; i<30; i++) {
            t1.append(i);
            t2.append(2*i);
        }

        t1.append(100);
        t2 = t1;
        t2.append(75);

        orbit_t t3( t1 );
        t3.append(150);
        t3.append(200);
        orbit_t t4( t1 = t2 = t3) ;
    }
}

/**
 * @brief Code to exercise the path object typedef which is t_path<int64>.
 */
void path_test()
{
    std::cout << "\npath test" << std::endl;

    // Testing the need for something more in zeroize() or init() when doing an assignment
    path one67(167);
    path four47(447);
    path seven03(703);
    path also_four47(four47);
    path also_seven03(seven03);

    t_path<int64_t> init(123);
    t_path<int64_t> reinit( init );

    one67 = four47;
    one67 = 167;        // Restore origibal value

    path wow(123456789);
    std::cout << "wow:" << wow.flow(60) << std::endl;
    std::cout << "wow:" << wow.flow(0) << std::endl;
    std::cout << "wow:" << wow.flow() << std::endl;

    int neg = -1;
    path broken( 13, neg );
    std::cout << "broken:" << broken.flow() << std::endl;
}

#ifdef gnu_mp

/**
 * @brief Code to exercise the A098294 OEIS class implementation.
 */
void A098294_test()
{
    A098294 a098294;

    // Test increment
    for (int i=0; i<20; ++i) {
        mpz_class t = a098294.term();

        std::cout << "n = " << a098294.index() << ", A186009(n) = " << a098294.term() << std::endl;
        a098294++;      // Postfix utilizes prefix
    }

    // Intentionally overrun the 0th element
    for (int i=25; i>=0; --i) {
        mpz_class t = a098294.term();

        std::cout << "n = " << a098294.index() << ", A186009(n) = " << a098294.term() << std::endl;
        a098294--;      // Postfix utilizws prefx
    }

    // Test increment after pathological decrement
    for (int i=0; i<15; ++i) {
        mpz_class t = a098294.term();

        std::cout << "n = " << a098294.index() << ", A186009(n) = " << a098294.term() << std::endl;
        ++a098294;
    }

    A098294 index_test(6);
    std::cout << "n = " << index_test.index() << ", A186009(n) = " << index_test.term() << std::endl;
    index_test[12];
    std::cout << "n = " << index_test.index() << ", A186009(n) = " << index_test.term() << std::endl;
}

/**
 * @brief Code to exercise the A100982 OEIS class implementation.
 */
void A100982_test()
{
    A100982 rangecheck;

    std::cout << "\nA100982 range check" << std::endl;

    rangecheck[7];
    int size = rangecheck.size();

    mpz_class notgood = rangecheck.elem( -1 );
    mpz_class reallygood = rangecheck.elem( size - 3 );
    mpz_class secondlast = rangecheck.elem( size - 2 );
    mpz_class last = rangecheck.elem( size - 1 );
    mpz_class bad = rangecheck.elem( size );
    mpz_class reallybad = rangecheck.elem( size + 1 );
}

/**
 * @brief One of a number of cumulative class testers.
 */
void cumulative_test1()
{
    mpz_class an(1), last_an(1);

    mpz_class numer = 0, denom = 1, last_denom = 1;
    // std::vector< mpz_class > A100982_vector;

    mpz_class bigun("16253751717361873278362387462384762384723642384718361391301983076253171818271716");
    mp_path bigpath(bigun);
    std::cout << bigpath.flow(280) << std::endl;
    std::cout << bigpath.flow() << std::endl;

    A020914 exp_of_2;
    A000079 power_of_2;
    A186009 a186009;

    std::cout << "\nIncrement functionality" << std::endl;

    // Generate the denominators and numerators for C(n)
    for ( int n = 1; n <= 40; n++ )
    {
        last_an = an;
        last_denom = denom;

        an = a186009.term();

        mpf_class ratio = mpf_class(an)/mpf_class(last_an);
        denom = power_of_2[ exp_of_2() ];
        numer *= denom / last_denom;
        numer += an;
        mpf_class error = 1.0 - mpf_class(numer)/ mpf_class(denom);

        // Now try to replicate the output of the elements
        std::vector< mpz_class > copy = a186009.elements();

        // Output the term details
        std::cout << "n = " << a186009.index() << ", j = " << copy.size() << ", a(n) = " << an;
        std::cout << ", a(n)/a(n-1) = " << ratio;
        std::cout << ", exponent = " << exp_of_2 << ", non-convergent = " << error;
        std::cout << "; numerator = " << numer << "; denominator = " << denom << std::endl;
        
        // Spew out the generating vector
        for ( std::vector< mpz_class >::const_iterator iter = copy.cbegin(); iter != copy.cend(); ++iter )
        {
            std::cout << *iter << " ";
        }

        std::cout << std::endl;

        // Move to next terms in sequences
        ++a186009;
        ++exp_of_2;
    }
}

/**
 * @brief One of a number of cumulative class testers.
 */
void cumulative_test2()
{
    // Test out the class
    Cumulative c(5);

    std::cout << "\nDecrement functionality" << std::endl;

    // Try out the decrement functionality - make sure ut handles overruns gracefully
    for ( int32_t n = 0; n<= 8; n++ )
    {
        std::cout << "n = " << c.index() << ", A186009(n) = " << c.novel();
        std::cout << ", exponent = " << c.exponent();
        std::cout << "; numerator = " << c.numerator() << "; denominator = " << c.denominator() << std::endl;

        --c;
    }

    c.init();
}

/**
 * @brief One of a number of cumulative class testers.
 */
void cumulative_test3()
{
    // Test out the class
    Cumulative c;
    
    mpz_class numer, denom;

    std::cout << "\nFractional computation functionality" << std::endl;

    // Generate the denominators and numerators for C(n) using the class this time
    for ( int n = 0; n <= 50; n++ )
    {
        // mpf_class error = 1.0;
        mpf_class error = 1.0 - mpf_class( numer = c.numerator() )/ mpf_class( denom = c.denominator() );

        std::cout << "n = " << c.index() << ", A186009(n) = " << c.novel();
        std::cout << ", exponent = " << c.exponent() << ", non-convergent = " << error;
        std::cout << "; numerator = " << numer << "; denominator = " << denom << std::endl;

        c++;            // A little joke on C++ :)
    }
}

#endif

/**
 * @brief The main() entry point is used to call menu() and also for testing components \b before calling menu().
 * @details Everything needs a starting point
 * @return int - Executable currently does not return any values to caller.
 */
int main()
{
    bool stop_in_main = true;

    // Unit test modules

    // orbit_test();
    // path_test();
    // A100982_test();
    // A098294_test();
    cumulative_test1();
    // cumulative_test2();
    // cumulative_test3();

    long integer = -321;        // A very cool negative integer starting point

    // stop the menu from popping up while testing()
    // exit(0);

    // Display the selection menu.  Once you return from this you're done.
    menu();

    // That's it.
    printf("all done.\n");
}
