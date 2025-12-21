/**
 * @file oeis.cpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief Class implementations for selected OEIS https://oeis.org sequences.
 * @version 1.1
 * @date 2025-12-18
 * 
 * @copyright Copyright (c) 2023-2025 Wayne Brassem
 */

// This include brings in the basic definitions
#include "oeis.hpp"

// The ability to compile the classes which implement the follow OEIS sequences relies on GNU libraries
#ifdef gnu_mp

// Implementation of virtual base class for OEIS sequences
// OEIS_base public member functions

/**
 * @brief This member function computes and returns the term for the OEIS sequence given a 32-bit index.
 * @param [in] index - Calculates and returns sequence term for a given index.
 * @return const mpz_class& - Returns the sequence term as a multiple precision integer.
 */
const mpz_class& OEIS_base::operator[]( const int32_t index )
{
    // Protect against indices which are less than the offset
    if ( index < oeis_offset )
    {
        // If the provided index value is below the offset then reinitialize and return the initial term
        init();
        return oeis_term;
    }

    // Reinitialize the class by calling the virtual init() member function
    init();

    // Calculate how many elements of the series to generate
    int32_t target = index + oeis_index - 1;

    // Loop through the elements beginning at the offset up to the term requested by the index
    for ( int32_t i = oeis_offset; i < target; i++ )
    {
        // Call the virtual increment operator
        operator++();
    }

    // Return the term value
    return oeis_term;
}

/**
 * @brief This member function computes and returns the term for the OEIS sequence given an multiple precision index.
 * @param [in] index - Calculates and returns sequence term for a given index.
 * @return const mpz_class& - Returns the sequence term as a reference to a multiple precision integer.
 */
const mpz_class& OEIS_base::operator[]( const mpz_class& index )
{
    // Protect against indices which are less than the offset
    if ( index < oeis_offset )
    {
        // If the provided index value is below the offset then reinitialize and return the initial term
        init();
        return oeis_term;
    }

    // Reinitialize the class by calling the virtual init() member function
    init();

    // Loop through the elements beginning at the offset up to the term requested by the index
    for ( int32_t i = oeis_offset; i < index; i++ )
    {
        // Call the virtual increment operator
        operator++();
    }

    // Return the term value
    return oeis_term;
}

/**
 * @brief Prefix increment to the next value in the OEIS sequence.
 * @return const mpz_class& - Returns the sequence term as a reference to a multiple precision integer.
 */
const mpz_class& OEIS_base::operator++()
{
    // Increment the index
    ++oeis_index;

    // Return the term object
    return oeis_term;
}

/**
 * @brief Postfix increment to the next value in the OEIS sequence.
 * @return mpz_class - Returns the sequence term as a multiple precision integer.
 */
mpz_class OEIS_base::operator++(int)
{
    // Store a local copy of the current term value
    mpz_class temp = term();

    // Call the prefix increment
    operator++();

    // Return the original term value
    return temp;
}

/**
 * @brief Prefix decrement to the previous value in the OEIS sequence.
 * @return const mpz_class& - Returns the sequence term as a reference to a multiple precision integer.
 */
const mpz_class& OEIS_base::operator--()
{
    // Make sure you don't decrement the index beyond the offset
    if ( oeis_index > oeis_offset )
    {
        // Decrement the index
        --oeis_index;
    }

     // Return the term object
    return oeis_term;
}

/**
 * @brief Postfix decrement to the previous value in the OEIS sequence.
 * @return mpz_class - Returns the sequence term as a multiple precision integer.
 */
mpz_class OEIS_base::operator--(int)
{
    // Store a local copy of the current term value
    mpz_class temp = term();

    // Call the prefix decrement
    operator--();

    // Return the original term value
    return temp;
}

// OEIS_base protected member functions

/**
 * @brief Default constructor for a new oeis base::oeis base object.
 * @details Protected default constructor prevents class from being instantiated directly - only by inheritance can it be utilized.
 */
OEIS_base::OEIS_base()
{
    // Set the OEIS sequence offset and index to 0 and the initial term to 1
    init();
}

/**
 * @brief Parameterized constructor for a new oeis base::oeis base object.
 * @details Protected parameterized constructor prevents class from being instantiated directly - only by inheritance can it be utilized
 * @param [in] offset - The index of the first term in the sequence
 * @param [in] index - The current term index in the sequence index
 * @param [in] term - The value of the sequence at the index term
 */
OEIS_base::OEIS_base( int32_t offset, int32_t index, int32_t term )
{
    // Set the OEIS sequence offset, index and term as requested
    init( offset, index, term );
}

/**
 * @brief Initialize the base class member variables.
 * @details Parameterized virtual base class initialization.  Sets the variables to those given in the arguments.
 * @param [in] offset - The index of the first term in the sequence
 * @param [in] index - Initial value for the sequence index
 * @param [in] term - The initial term in the sequence
 */
void OEIS_base::init( int32_t offset, int32_t index, int32_t term )
{
    // Initialize the base class members
    oeis_offset = offset;
    oeis_index  = index;
    oeis_term   = term;
}


/**
 * @brief This defines the ostream object which allows derived classes to use the operator<<() for extracting the current sequence term.
 * 
 * User defined type is on the right so this is a non-member function.
 * Derived classes use this function for ostream term output.
 * 
 * @param os [in] - An ostream object
 * @param oeis [in] - A derived class from the OEIS base class
 * @return std::ostream& - Reference to ostream object
 */
std::ostream& operator<<(std::ostream& os, const OEIS_base& oeis)
{
    // write obj to stream
    return os << oeis.term();
}


// Implementation of A000079 OEIS sequences, a(n) = 2^n
// Invariant: oeis_term == 2^(oeis_index)
// A000079 public member functions

/**
 * @brief Default constructor for a new A000079::A000079 object.
 * @details Default constructor initializes to the first term in the sequence where n=0.
 */
A000079::A000079()
{
    init(0, 0, 1);   // a(0) = 1
}

/**
 * @brief Parameterized constructor for a new A000079::A000079 object.
 * @details Parameterized constructor initializes the sequence to where n=index.
 * @param [in] index - The term to position the sequence on.
 */
A000079::A000079( int32_t index )
{
    // Position at index term in sequence
    operator[]( index );
}

/**
 * @brief Parameterized constructor for a new A000079::A000079 object.
 * @details Parameterized constructor initializes the sequence to where n=index.
 * @param [in] index - The term to position the sequence on.
 */
A000079::A000079( const mpz_class& index )
{
    // Position at index term in sequence
    operator[]( index );
}

/**
 * @brief Prefix increment to the next value in the sequence OEIS A000079.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A000079::operator++()
{
    // Increment the index
    ++oeis_index;

    // Return the new term value
    return oeis_term *= 2;
}

/**
 * @brief Prefix decrement to the previous value in the sequence OEIS A000079.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A000079::operator--()
{
    // Make sure you don't decrement the index beyond the offset
    if (oeis_index > oeis_offset && oeis_term > 1)
    {
        // Decrement the index
        --oeis_index;

        // Return the new term value
        return oeis_term /= 2 ;
    }

    // Return the original unaltered term value
    return oeis_term;
}


// Implementation of https://oeis.org/A002379, a(n) = floor ( 3^n / 2^n )
// Public member functions

/**
 * @brief Default constructor for a new A002379::A002379 object.
 * @details Default constructor initializes to the first term in the sequence where n=0.
 */
A002379::A002379()
{
    // Initialize derived class specific variables
    init_local();
}

/**
 * @brief Parameterized constructor for a new A002379::A002379 object.
 * @details Parameterized constructor initializes the sequence to where n=index.
 * @param [in] index - The term to position the sequence on.
 */
A002379::A002379( int32_t index )
{
    // Position at index term in sequence
    operator[]( index );
}

/**
 * @brief Prefix increment to the next value in the series OEIS A002379.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A002379::operator++()
{
    // Increment the index
    ++oeis_index;

    // Adjust the local variables as required
    threes *= 3;
    twos *= 2;

    // Return the new term value - this performs integer division so it truncates which is perfect
    return oeis_term = threes / twos;
}

/**
 * @brief Prefix decrement to the previous value in the series OEIS A002379.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A002379::operator--()
{
    // Make sure you don't decrement the index beyond the offset
    if ( oeis_index > oeis_offset && oeis_index > 0)
    {
        // Decrement the index
        --oeis_index;

        // Adjust the local variables as required
        threes /= 3;
        twos /= 2;

        // Return the new term value - this performs integer division so it truncates which is perfect
        return oeis_term = threes / twos;
    }

    // Return the original unaltered term value
    return term();
}

/**
 * @brief Initialize the base and derived class members.
 */
void A002379::init()
{
    // Initialize base class variables
    OEIS_base::init();

    // Initialize A002379 specific variables
    init_local();
}

// Protected member functions

/**
 * @brief Initialize the derived class members.
 */
void A002379::init_local()
{
    // Initialize A002379 specific variables
    twos = threes = 1;
}


// Implementation of https://oeis.org/A020914
// Number of digits in the base-2 representation of 3^n

// Public member functions

/**
 * @brief Default constructor for a new A020914::A020914 object.
 * @details Default constructor initializes to the first term in the sequence where n=0.
 */
A020914::A020914()
{
    // Initialize derived class specific variables
    init_local();
}

/**
 * @brief Parameterized constructor for a new A020914::A020914 object.
 * @details Parameterized constructor initializes the sequence to where n=index.
 * @param [in] index - The term to position the sequence on.
 */
A020914::A020914( int32_t index )
{
    // Position at index term in sequence
    operator[]( index );
}

/**
 * @brief Prefix increment to the next value in the series OEIS A020914.
 * @return const mpz_class& - Returns the sequence term as a reference to a multiple precision integer.
 */
const mpz_class& A020914::operator++()
{
    // Increment the index
    ++oeis_index;

    // Go to next 3^n
    threes *= 3;

    // Increase the power of 2 until its greater than the power of 3
    while ( twos < threes )
    {
        twos *= 2;
        oeis_term++;        
    }

    // Return the new term value
    return oeis_term;
}

/**
 * @brief Prefix decrement to the previous value in the series OEIS A020914.
 * @return const mpz_class& - Returns the sequence term as a reference to a multiple precision integer.
 */
const mpz_class& A020914::operator--()
{
    // Make sure you don't decrement the index beyond the offset
    if ( oeis_index > oeis_offset )
    {
        // Decrement the index
        --oeis_index;

        // Go to previous 3^n
        threes /= 3;

        // If the power of two is just more than the power of three then stop
        while ( ( twos / 2 ) > threes )
        {
            twos /= 2;
            --oeis_term;
        }
    }

    // Return the new term value
    return oeis_term;
}

/**
 * @brief Initialize the base and derived class members.
 */
void A020914::init()
{
    // Initialize base class variables
    OEIS_base::init();

    // Initialize A020914 specific variables
    init_local();
}

// Protected member functions

/**
 * @brief Parameterized constructor for a new A020914::A020914 object.
 * @details Protected parameterized constructor prevents class from being instantiated directly - only by inheritance can it be utilized
 * @param [in] offset - The index of the first term in the sequence
 * @param [in] index - The current term index in the sequence index
 * @param [in] term - The value of the sequence at the index term
 */
A020914::A020914( int32_t offset, int32_t index, int32_t term ) : OEIS_base( offset, index, term )
{
    init_local();
}

/**
 * @brief Initialize the derived class members.
 */
void A020914::init_local()
{
    // Initialize A020914 specific variables, representing the starting condition of 2^1 and 3^0
    twos = 2;
    threes = 1;
}


// Implementation of https://oeis.org/A056576
// Highest k with 2^k <= 3^n ( or A020914(n) -1 )

/**
 * @brief Default constructor for a new A056576::A056576 object.
 * @details This series utilizes the implementation of A020914 but with modified base class starting condtions.
 * Therefore the default constructor utilizes the protected constructor of A020914 to adjust the initial conditions.
 */
A056576::A056576() : A020914( 0, 0, 0 ) {}

/**
 * @brief Parameterized constructor for a new A056576::A056576 object.
 * @details This series utilizes the implementation of A020914 but with modified base class starting condtions.
 * Therefore the parameterized constructor utilizes the protected constructor of A020914 to adjust the initial conditions.
 * @param [in] index - The term to position the sequence on.
 */
A056576::A056576( int32_t index ) : A020914( 0, 0, 0 )
{
    operator[]( index );
}

/**
 * @brief Initialize the base and derived class members.
 */
void A056576::init()
{
    // Initialize base class variables
    OEIS_base::init( 0, 0, 0 );

    // Initialize A056576 specific variables
    A020914::init_local();
}


// Implementation of https://oeis.org/A022921
// Number of 2^m between 3^n and 3^(n+1)

// Public member functions

/**
 * @brief Default constructor for a new A022921::A022921 object.
 * @details Default constructor initializes to the first term in the sequence where n=0.
 */
A022921::A022921()
{
    // Initialize derived class specific variables
    init_local();
}

/**
 * @brief Parameterized constructor for a new A022921::A022921 object.
 * @details Parameterized constructor initializes the sequence to where n=index.
 * @param [in] index - The term to position the sequence on.
 */
A022921::A022921( int32_t index )
{
    // Position at index term in sequence
    operator[]( index );
}

/**
 * @brief Prefix increment to the next value in the series OEIS A022921.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A022921::operator++()
{
    // Store the last exponent of 2 so you can compute the difference to the new exponent of 2
    int32_t last = exponent_of_two;

    // Increment the index
    ++oeis_index;

    // Go to next 3^n
    threes *= 3;

    // Increase the power of 2 until its greater than the power of 3
    while ( twos < threes )
    {
        twos *= 2;
        exponent_of_two++;
    }

    // Return the new term value
    return oeis_term = exponent_of_two - last;
}

/**
 * @brief Prefix decrement to the previous value in the series OEIS A022921
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A022921::operator--()
{
    // Make sure you don't decrement the index beyond the offset
    if ( oeis_index > oeis_offset )
    {
        // Terms can only be calculated on increment so you need to decrement twice and then incrememnt
        // Note that this can cause the index to temporarily dip to -1 when index=1, but the increment increments to 0
        oeis_index -= 2;            // Move the index back two - two decrements

        // Go back two (3^2)        // Divide two factors of three - two decrements
        threes /= 9;

        // If the power of two is just more than the power of three then stop
        while ( ( twos / 2 ) > threes )
        {
            twos /= 2;
            exponent_of_two--;
        }

        // The value calculated on increment beginning from two back from where you started is the answer
        return operator++();
    }

    else
        // Return the original term value
        return oeis_term;
}

/**
 * @brief Initialize the base and derived class members.
 */
void A022921::init()
{
    // Initialize base class variables
    OEIS_base::init();

    // Initialize A022921 specific variables
    init_local();
}

// Protected member functions

/**
 * @brief Initialize the derived class members.
 */
void A022921::init_local()
{
    // Initialize A022921 specific variables
    twos = 4;
    threes = 3;
    exponent_of_two = 2;
}


// Implementation of https://oeis.org/A098294
// ceiling(n*log2(3/2))

// Public member functions

/**
 * @brief Default constructor for a new A098294::A098294 object.
 * @details Default constructor initializes to the first term in the sequence where n=0.
 */
A098294::A098294()
{
    // Initialize base class variables
    OEIS_base::init( 0, 0, 0 );

    // Initialize derived class specific variables
    init_local();
}

/**
 * @brief Parameterized constructor for a new A098294::A098294 object.
 * @details Parameterized constructor initializes the sequence to where n=index.
 * @param [in] index - The term to position the sequence on.
 */
A098294::A098294( int32_t index )
{
    // Position at index term in sequence
    operator[]( index );
}

/**
 * @brief Prefix increment to the next value in the series OEIS A098294.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A098294::operator++()
{
    // Increment the index
    ++oeis_index;

    twos *= 2;
    threes *= 3;

    mpz_class flat = threes / twos;
    mpz_class factors = 0;

    // Compute a(n) = ceil(n * log2(3/2)) using only integer arithmetic.
    // twos  = cumulative power of 2
    // threes = cumulative power of 3
    // flat = threes / twos → integer division automatically gives floor(3^n / 2^n)
    // Counting the number of divisions by 2 of flat yields ceil(n*log2(3/2)) exactly.
    // This avoids floating point and works efficiently with arbitrary precision integers.
    while( flat != 0 )
    {
        flat /= 2;
        ++factors;
    }

    return oeis_term = factors;
}

/**
 * @brief Prefix decrement to the next value in the series OEIS A098294.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A098294::operator--()
{
    // Protect against decremenbting beynod start
    if ( oeis_index > 0 ) {
        // Decrement the index
        --oeis_index;

        twos /= 2;
        threes /= 3;

        mpz_class flat = threes / twos;
        mpz_class factors = 0;

        // Special case when n=0 because only time when log_2( 3^n / 2^n ) generates an integer
        if ( oeis_index == 0 )
            return oeis_term = 0;

        // Loop until there is nothing left
        while( flat != 0 )
        {
            flat /= 2;
            ++factors;
        }
        return oeis_term = factors;
    }

    // Otherwise return the last term value
    else {
        return oeis_term;
    }
}

/**
 * @brief Initialize the base and derived class members.
 */
void A098294::init()
{
    // Initialize base class variables
    OEIS_base::init();

    // Initialize A022921 specific variables
    init_local();
}

/**
 * @brief Initialize the derived class members.
 */
void A098294::init_local()
{
    // Initialize A022921 specific variables
    twos = 1;
    threes = 1;
}

// Implementation of https://oeis.org/A100982
// Collatz dropping time residue

// What is really remarkable about this series is that it all begins with just a single starting value of 1
// It evolves based on how powers of 2 and 3 intermix producing a complex, but very important growth pattern

// Public member functions

/**
 * @brief Default constructor for a new A100982::A100982 object.
 * @details Default constructor initializes to the first term in the sequence where n=1.
 */
A100982::A100982() : OEIS_base( 1, 1, 1 )
{
    // Initialize derived class specific variables
    init_local();
}

/**
 * @brief Parameterized constructor for a new A100982::A100982 object.
 * @details Parameterized constructor initializes the sequence to where n=index.
 * @param [in] index - The term to position the sequence on.
 */
A100982::A100982( int32_t index ) : OEIS_base( 1, 1, 1 )
{
    // Position at index term in sequence
    operator[]( index );
}

/**
 * @brief Return a specific vector element value by index number.
 * @param index - The element in the vector to return
 * @return mpz_class - The multiple precision integer (returns 0 if index out of range of vector)
 */
mpz_class A100982::elem( int32_t index ) const
{
    // Basic index range check
    if ( ( index < 0 ) || ( index >= size() ) )
        return mpz_class( 0 );
    else
        return a100982_vec[ index ];
}
    
/**
 * @brief Prefix increment to the next value in the series OEIS A100982.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A100982::operator++()
{
    // Define an iterator that traverses the vector in forward order
    std::vector< mpz_class >::iterator iter;

    // Zero out the term value - it will be recalculated alongisde the new vector elements
    oeis_term = 0;

    // Increment the index
    ++oeis_index;

    // Regenerate the elements of the vector using the existing values in a Fibonacci way
    for ( iter = a100982_vec.begin() + 1; iter != a100982_vec.end(); ++iter )
    {
        // Add the difference between the current element and the previous one in a Fibonacci way
        * iter += * ( iter - 1 );

        // Keep track of the vector sum
        oeis_term += * iter;
    }

    // Duplicate the last vector entry if A022921( oeis_index - 2 ) == 2
    if ( a022921_test.term() == 2 )
    {
        // Store the value of the last integer in the vector so you can append it and add to total
        mpz_class last = * ( iter - 1 );

        // Duplicate the final term and adjust the term summation
        a100982_vec.push_back( last );
        oeis_term += last;
    }

    // Increment the doubler checker
    ++a022921_test;

    // Return the prefix term value and remember to add the leading 1 term to the sum - thus the prefix increment
    return ++oeis_term;
}

/**
 * @brief Prefix decrement to the previous value in the series OEIS A100982.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A100982::operator--()
{
    // Make sure you don't decrement the index beyond the offset
    if ( oeis_index > oeis_offset )
    {
        // Zero out the term value - it will be recalculated alongisde the new vector elements
        oeis_term = 0;

        // Decrement the doubler checker
        --a022921_test;

        // The algorithm says to double last term when A022921( n-2 ) = 2, which happens beyond n = 2
        if ( oeis_index > 2 )
        {
            // Delete the last vector entry if A022921( oeis_index - 2 ) == 2
            if ( a022921_test.term() == 2 )
            {
                // Delete the last vector element
                a100982_vec.pop_back();
            }
        }

        // Define an iterator that traverses the vector in reverse order
        std::vector< mpz_class >::reverse_iterator iter;

        // Regenerate the elements of the vector using the existing values in a reverse Fibonacci way starting at the end
        // This loop does not go right to the first element which is always 1
        for ( iter = a100982_vec.rbegin(); iter != a100982_vec.rend() - 1; ++iter )
        {
            // Subtract the difference between the current element and the next one (going right to left) in a reverse Fibonacci way
            * iter -= * ( iter + 1 );
            
            // Keep track of the vector ongoing sum
            oeis_term += * iter;
        }

        // Decrement the index
        --oeis_index;

        // Return the prefix term value and remember to add the leading 1 term to the sum - thus the prefix increment
        return ++oeis_term;
    }

    else
        // Return the original term value
        return oeis_term;
}

/**
 * @brief Initialize the base and derived class members.
 */
void A100982::init()
{
    // Initialize base class variables
    OEIS_base::init( 1, 1, 1 );

    // Initialize A100982 specific variables
    init_local();
}

// Protected member functions

/**
 * @brief Initialize the derived class members.
 */
void A100982::init_local()
{
    // Initialize A100982 specific variables

    // Clear out any pre-existing state
    a100982_vec.clear();

    // The entire sequence begins with just a single integer starting value of 1 - that's pretty cool if you think about it
    a100982_vec.push_back( 1 ); 
    
    // Reinitialize the class
    a022921_test.init();
}


// Implementation of https://oeis.org/A186009
// Collatz dropping time residue

// The series A186009 is the same as A100982 with 1 prepended
// As a result the implementation steals most of it's functionality from the A100982 class

// Public member functions

/**
 * @brief Default constructor for a new A186009::A186009 object
 * @details Default constructor initializes to the first term in the sequence where n=1.
 */
A186009::A186009() : OEIS_base( 1, 1, 1 ) {}

/**
 * @brief Parameterized constructor for a new A186009::A186009 object
 * @details Parameterized constructor initializes the sequence to where n=index.
 * @param [in] index - The term to position the sequence on.
 */
A186009::A186009( int32_t index ) : OEIS_base( 1, 1, 1 )
{
    // Position at index term in sequence
    operator[]( index );
}

/**
 * @brief Prefix increment to the next value in the series OEIS A186009.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A186009::operator++()
{
    // Artificially prepend an extra value of 1 in front of A100982(n)
    if ( oeis_index++ == oeis_offset )
        return oeis_term;

    // Increment the underlying sequence
    ++a100982;

    // Assign and return sequence value to term
    return oeis_term = a100982();
}

/**
 * @brief Prefix decrement to the previous value in the series OEIS A186009.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 */
const mpz_class& A186009::operator--()
{
    // Make sure you don't decrement the index beyond the offset
    if ( oeis_index > oeis_offset )
    {
        // Artificially prepend an extra value of 1 in front of A1000982(n)
        if ( --oeis_index == oeis_offset )
            return oeis_term;

        // Decrement the underlying sequence
        --a100982;

        // Copy the term from the underlying sequence
        oeis_term = a100982();
    }

    // Return the term value
    return oeis_term;
}

// Protected member functions

/**
 * @brief Initialize the base and derived class members.
 */
void A186009::init()
{
    // Initialize base class variables
    OEIS_base::init( 1, 1, 1 );

    // Initialize A186009 specific variables
    init_local();
}


// Implementation of Cumulative class C(n)
// Calculate the cumulative convergence of C(n) = sum from 0 to n { N(n) }

// Where N(n) = A186009( n + 1 ) / ( A000079( A020914( n ) ) ) = A186009( n + 1 ) / ( 2 A098232( n ) )

// Public member functions

/**
 * @brief Default constructor for a new Cumulative::Cumulative object
 * @details The Cumulative class actually requires two integers to be calculated - and the traditional OEIS sequence has just one series.
 * The value for the cumulative convergence is a summation of the individual terms of the novel converences factor \b N(n).
 * 
 * Default constructor initializes the numerator of 0, and denominator of 2 in the sequence where n=0.
 */
Cumulative::Cumulative()
{
    init();
}

/**
 * @brief Parameterized constructor for a new Cumulative::Cumulative object.
 * @details Parameterized constructor initializes the sequence to where n=index.
 * @param [in] index - The term to position the sequence on.
 */
Cumulative::Cumulative( int32_t index )
{
    // Position at index term in sequence
    operator[]( index );
}

/**
 * @brief Prefix increment to the next ratio in the Cumulative \b C(n) series.
 * @return const mpz_class& - Returns the sequence term as a const reference to a multiple precision integer.
 * This represents the numerator of the \b C(n) ratio.  The denominator is also calculated and retrieved with the denominator() method.
 */
const mpz_class& Cumulative::operator++()
{
    // Increment the term index
    ++oeis_index;

    // Save the last denominator and go to the next one
    mpz_class last_denom = a000079++;

    // A022921 indicates whether A020914 increased by 1 or 2.
    // A value of 2 means the denominator gains an extra factor of 2.
    if ( a022921++ == 2 )
        ++a000079;

    // Scale numerator to match the new power-of-two denominator
    mpz_class scale = denominator() / last_denom;
    oeis_term *= scale;

    // Return the numerator with the residue added in
    return ( oeis_term += ++a186009 );
}

/**
 * @brief Prefix decrement to the previous value in the Cumulative \b C(n) series.
 * @return const mpz_class& - Returns the sequence term as a reference to a multiple precision integer.
 * This represents the numerator of the \b C(n) ratio.  The denominator is also calculated and retrieved with the denominator() method.
 */
const mpz_class& Cumulative::operator--()
{
    // Make sure you don't decrement the index beyond the offset
    if ( oeis_index > oeis_offset )
    {
        // Decrement the term index
        --oeis_index;

        // Save the last denominator and go to the previous one
        mpz_class last_denom = a000079--;

        // Decrement again if needed - if the sequence is a 1 then already done, if it is a 2 then go to previous term
        if ( --a022921 == 2 )
            --a000079;

        // Remove the incremental contribution
        oeis_term -= a186009--;

        // Contract numerator to match the reduced power-of-two denominator
        mpz_class scale = last_denom / denominator();
        oeis_term /= scale;
    }

    // Return the term value
    return oeis_term;
}

/**
 * @brief Initialize the base and derived class members.
 */
void Cumulative::init()
{
    // Initialize base class variables
    OEIS_base::init();

    // Initialize A002379 specific variables
    init_local();
}

// Protected member functions

/**
 * @brief Initialize the derived class members.
 */
void Cumulative::init_local()
{
    // Initialize Cumulative specific variables
    a000079[2];             // A020914(0) = 1, thus A000079( A020914(0) ) = 2, so initialize A000079 series at term 2 (2^1) without advancing
    a022921.init();         // A022921 is the first differences of A020914 - techincally either could work in this call implementation
    a186009.init();         // Collatz residues which are added or subtracted from the rational on increment or decrement as required
}


#endif