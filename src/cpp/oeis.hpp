/**
 * @file oeis.hpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief Class definitions for selected OEIS https://oeis.org sequences.
 * The main goal of this set of classes is to provide a library of Collatz conjecture related integer sequence class objects and classes
 * common to and derived from them.  Where possible derived class names correspond to the identifiers of the OEIS sequences.
 * @version 1.0
 * @date 2023-04-08
 * 
 * @copyright Copyright (c) 2023
 */

#include "common.hpp"

// Wrapper to prevent duplication if header included twice
#if !defined oeis_hpp
#define oeis_hpp

// The ability to compile the classes which implement the following OEIS sequences rely on GNU multiple precision libraries
#ifdef gnu_mp

/**
 * @brief Virtual base class definition for selected https://oeis.org sequences.
 * @details Note that there is no public constructor defined, so this class must be inherited.
 */
class OEIS_base
{
    public:
        // Return class members

        /**
         * @brief All OEIS sequences should have an offset defined.
         * @return const int32_t 
         */
        inline const int32_t offset() const { return oeis_offset; };        // Return offset as const int32_t

        /**
         * @brief All OEIS sequences should have at least one index.
         * @return const int32_t 
         */
        inline const int32_t index() const { return oeis_index; };          // Return index as const int32_t

        /**
         * @brief All OEIS sequences should have at least one term.
         * @return const mpz_class& - Returns a const reference to a multiple precision integer
         */
        inline const mpz_class& term() const { return oeis_term; };         // Return term as const multiple precision

        // Positioning and index operators

        /**
         * @brief Value operation: returns the current value of the sequence.
         * @return const mpz_class& - Returns a const reference to a multiple precision integer
         */
        inline const mpz_class& operator()() const { return term(); };

        const mpz_class& operator[]( const int32_t index );                 // Index operation - calculates and returns term for a given index
        const mpz_class& operator[]( const mpz_class& index );              // Index operation - calculates and returns term for a given index

        // Increment and decrement operators - protected so they cannot be called directly, only by derived classes
        virtual const mpz_class& operator++();                              // Prefix index increment
        virtual const mpz_class& operator--();                              // Prefix index decrement
        virtual       mpz_class  operator++(int);                           // Postfix index increment
        virtual       mpz_class  operator--(int);                           // Postfix index decrement

        /**
         * @brief Virtual init() function which is used for default initialization of base class variables.
         * @details Default virtual base class initialization.  Sets base class variables to common defaults which are an offset of 0, index of 0
         * and term of 1.  These defaults can be changed by derived classes using the protect init() version which accepts arguments.
         */
        inline virtual void init() { init( 0, 0, 1 ); };                    // Initialize the base class members to common default values

    protected:
        OEIS_base();                                                        // Default constructor is protected so this class must be inherited
        OEIS_base( int32_t offset, int32_t index, int32_t term );           // Paramterized constructor allows derived class to vary from defaults

        // Virtual init() function which is used to specify initialization of base class variables
        virtual void init( int32_t offset, int32_t index, int32_t term );

        int32_t   oeis_offset;                                              /**< Index of the first term - A constant for any given sequence. */
        int32_t   oeis_index;                                               /**< The current index, \e n, in the sequence. */
        mpz_class oeis_term;                                                /**< The current value of the sequence, \b a(n). */
};

// User defined type on the right so this is a non-member function
// This is defined for the base class so that all derived classes can benefit
std::ostream& operator<<( std::ostream& os, OEIS_base oeis );


/**
 * @brief Class definition for https://oeis.org/A000079.
 * @details Returns \b a(n) for the series OEIS A000079 where: \f[ a(n) = 2^n; n \in \mathbf{N_0} \f] 
 */
class A000079 : public OEIS_base
{
    public:
        /**
         * @brief Default constructor for a new A000079::A000079 object.
         * @details Default constructor initializes to the first term in the sequence where n=0.
         */
        A000079();                                                          // Default constructor positions at first term in sequence
        A000079( int32_t index );                                           // Parameterized constructor positions at index term in sequence

        // Increment and decrement operators
        virtual const mpz_class& operator++();                              // Prefix increment
        virtual const mpz_class& operator--();                              // Prefix decrement

        /**
         * @brief Postfix increment to the next value in the sequence OEIS A000079.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline virtual mpz_class operator++(int) { return OEIS_base::operator++(0); };

        /**
         * @brief Postfix decrement to the previous value in the sequence OEIS A000079.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline virtual mpz_class operator--(int) { return OEIS_base::operator--(0); };
};


/**
 * @brief Class definition for https://oeis.org/A002379.
 * @details Returns \b a(n) for the series OEIS A002379 where: \f[ a(n) = \lfloor ( 3^n / 2^n ); n \in \mathbf{N_0} \f]
 */
class A002379 : public OEIS_base
{
    public:
        A002379();                                                          // Default constructor positions at first term in sequence
        A002379( int32_t index );                                           // Parameterized constructor positions at index term in sequence

        // Increment and decrement operators
        const mpz_class& operator++();                                      // Prefix increment
        const mpz_class& operator--();                                      // Prefix decrement

        /**
         * @brief Postfix increment to the next value in the series OEIS A002379.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator++(int) { return OEIS_base::operator++(0); };

        /**
         * @brief Postfix decrement to the previous value in the series OEIS A002379.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator--(int) { return OEIS_base::operator--(0); };

        // Virtual init() function which is used for default initialization of class variables
        virtual void init();                                                // Resets the class to the default state which is the first term

    protected:
        void init_local();                                                  // Set initial values in derived class

        mpz_class twos;                                                     /**< Powers of 2. Starting condition at n=0 of: \f[ 2^0 = 1 \f] */
        mpz_class threes;                                                   /**< Powers of 3. Starting condition at n=0 of: \f[ 3^0 = 1 \f] */
};


/**
 * @brief Class definition for https://oeis.org/A020914.
 * @details Returns \b a(n) for the series OEIS A020914 where the a(n) = \e smallest \e k such that:
 * \f[ a(n) = k \Rightarrow \lceil 2^k / 3^n = 2; n,k \in \mathbf{N_0} \f] or expressed in a simpler closed form:
 * \f[ a(n) = 1 + \lfloor \log_2( 3^n ); n \in \mathbf{N_0} \f]
 * which represents the number of digits in the base-2 representation of the nth power 3 ( or A056576(n) + 1 ).
 * 
 * For n=0, the implementation starts with an exponent of 0 for the power of 3, and an exponent of 1 for the power of 2.
 */
class A020914 : public OEIS_base
{
    public:
        A020914();                                                          // Default constructor positions at first term in sequence
        A020914( int32_t index );                                           // Parameterized constructor positions at index term in sequence

        // Increment and decrement operators
        const mpz_class& operator++();                                      // Prefix increment
        const mpz_class& operator--();                                      // Prefix decrement

        /**
         * @brief Postfix increment to the next value in the series OEIS A020914.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */

        inline mpz_class operator++(int) { return OEIS_base::operator++(0); };
        /**
         * @brief Postfix decrement to the previous value in the series OEIS A020914.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator--(int) { return OEIS_base::operator--(0); };

        // Virtual init() function which is used for default initialization of class variables
        virtual void init();                                                // Resets the class to the default state which is the first term

    protected:
        A020914( int32_t offset, int32_t index, int32_t term );             // Paramterized constructor allow derived class to vary from defaults

        void init_local();                                                  // Set initial values in derived class

        mpz_class twos;                                                     /**< Powers of 2. Starting condition at n=0 of: \f[ 2^1 = 2 \f] */
        mpz_class threes;                                                   /**< Powers of 3. Starting condition at n=0 of: \f[ 3^0 = 1 \f] */
};


/**
 * @brief Class definition for https://oeis.org/A056576.
 * @details Returns \b a(n) for the series OEIS A056576 where:
 * \f[ a(n) = \lfloor \log_2( 3^n ); n \in \mathbf{N_0} \f]
 * which is the \e highest \e k with: \f[ 2^k \le 3^n; k,n \in \mathbf{N_0} \f]
 * This result is equivalent to A020914(n) -1.
 */
class A056576 : public A020914
{
    public:
        A056576();                                                  // Default constructor positions at first term in sequence
        A056576( int32_t index );                                   // Parameterized constructor positions at index term in sequence

        // Increment and decrement operators

        /**
         * @brief Prefix increment to the next value in the series OEIS A056576.
         * @return const mpz_class& - Returns a const reference to a multiple precision integer
         */
        inline const mpz_class& operator++() { return A020914::operator++(); };

        /**
         * @brief Prefix decremnet to the previous value in the series OEIS A056576.
         * @return const mpz_class& - Returns a const reference to a multiple precision integer
         */
        inline const mpz_class& operator--() { return A020914::operator--(); };

        /**
         * @brief Postfix increment to the next value in the series OEIS A056576.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator++(int) { return OEIS_base::operator++(0); };

        /**
         * @brief Postfix decrement to the previous value in the series OEIS A056576.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator--(int) { return OEIS_base::operator--(0); };

        // Virtual init() function which is used for default initialization of class variables
        virtual void init();                                        // Resets the class to the default state which is the first term
};


/**
 * @brief Class definition for https://oeis.org/A022921.
 * @details Returns \b a(n) for the series OEIS A022921 where:
 * \f[ a(n) = \lfloor \lbrace (n+1) \cdot \log_2( 3 ) \rbrace - \lfloor \lbrace n \cdot \log_2( 3 ) \rbrace;n \in \mathbf{N_0} \f]
 * which is the number powers of 2 between consecutive powers of three.  This sequence is also first differences of A020914.
 * 
 * This is the number of \f$ 2^m \f$ between \f$ 3^n \f$ and \f$ 3^{n+1}; m \in \mathbf{N}, n \in \mathbf{N_0} \f$.
 * The sequence is also first differences of A020914.
 */
class A022921 : public OEIS_base
{
    public:
        A022921();                                                  // Default constructor positions at first term in sequence
        A022921( int32_t index );                                   // Parameterized constructor positions at index term in sequence

        // Increment and decrement operators
        const mpz_class& operator++();                              // Prefix increment
        const mpz_class& operator--();                              // Prefix decrement

        /**
         * @brief Postfix increment to the next value in the series OEIS A022921.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator++(int) { return OEIS_base::operator++(0); };

        /**
         * @brief Postfix decrement to the previous value in the series OEIS A022921.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator--(int) { return OEIS_base::operator--(0); };

        // Virtual init() function which is used for default initialization of class variables
        virtual void init();                                        // Resets the class to the default state which is the first term

    protected:
        void init_local();                                          // Set initial values in derived class

        int32_t exponent_of_two;                                    /**< Exponent of 2, with a starting value of 2. */
        mpz_class twos;                                             /**< Power of 2. Starting condition of at n=0 of: \f[ 2^2 = 4 \f] */
        mpz_class threes;                                           /**< Power of 3. Starting condition of at n=0 of: \f[ 3^1 = 3 \f] */
};

/**
 * @brief Class definition for https://oeis.org/A098294.
 * 
 * @details Returns \b a(n) for the series OEIS A098294, where \f[ a(n) = \lceil n \cdot log_2 ( 3/2 ); n \in \mathbf{N_0} \f]
 * 
 * This series is interesting because it's elements map out the number of \b terms in the generating vectors for A100982 and A186009.
 * These two sequences define the evolution of the Colltaz dropping time pattern.
 * 
 * The number of terms in the generating vectors for A100982 and A186009 have a length which is given by A098294(n) - but with an
 * term offset of -1 and -2 respectively.
 */
class A098294 : public OEIS_base
{
    public:
        A098294();                                                  // Default constructor positions at first term in sequence
        A098294( int32_t index );                                   // Parameterized constructor positions at index term in sequence 

        // Increment and decrement operators
        const mpz_class& operator++();                              // Prefix increment
        const mpz_class& operator--();                              // Prefix decrement

        /**
         * @brief Postfix increment to the next value in the series OEIS A098294.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator++(int) { return OEIS_base::operator++(0); };

        /**
         * @brief Postfix decrement to the previous value in the series OEIS A098294.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */

        inline mpz_class operator--(int) { return OEIS_base::operator--(0); };        // Virtual init() function which is used for default initialization of class variables
        virtual void init();                                        // Resets the class to the default state which is the first term

    protected:
        void init_local();                                          // Set initial values in derived class

        // int32_t exponent_of_two;                                    /**< Exponent of 2, with a starting value of 2. */
        mpz_class twos;                                             /**< Power of 2. Starting condition of at n=0 of: \f[ 2^2 = 4 \f] */
        mpz_class threes;                                           /**< Power of 3. Starting condition of at n=0 of: \f[ 3^1 = 3 \f] */
};

/**
 * @brief Class definition for https://oeis.org/A100982.
 * @details Returns \b a(n) for the series OEIS A100982, where \b a(n) is the Collatz dropping time pattern and starts with n=1.
 * 
 * This sequence warrants a detailed explanation.  The terms of this series lie at the heart of the Collatz conjecture convergence proof and
 * indeed it's behaviour exhibits the most mysterious aspects of the seemingly chaotic bevahiour.  The algorithm is described in detail in the
 * accompanying formal proof and its origins are explained on the OEIS A100982 website at the link above (see \b Theorem \b 2 on the link above).
 * 
 * To generate the terms of this sequence, one constructs a vector consisting of generator elements.  This vector follows the relation:
 * 
 * \f[ a_{k+1}(n) = a_k(n) + a_k(n+1); k \in \mathbf{N_0},n \in \mathbf{N} \f]
 * and the starting condition of \f[ a(1) = a_1(1) = 1 \f]
 * 
 * This generates the pattern shown at https://oeis.org/A100982.  This algorithm was reformulated to left justify the vector terms as follows:
 * 
 * \f[ a_i(n) = a_i(n-1) + a_{i-1}(n); i,n \in \mathbf{N} \f]
 * 
 * where \e i is the term number in the vector, and \e n is the element index of the A100982 sequence.  To calculate \b a(n), you need to find:
 * \f[ a(n) = \sum_{i=1}^j a_i(n); i,j,n \in \mathbf{N} \f] where \e j is the length of the generating vector and starts with \e j=1 since there
 * is just one term. The last term for a given \e n of the generator vector is repeated whenever \f[ A022921(n-2)=2, n \ge 2, n \in \mathbf{N} \f]
 *
 * In other words, the length of the generating vector, \e j, grows in sympathy with a fixed offset to another sequence A022921,
 * which itself is tied to the powers of 2 and 3.  There is however a direct approach to determining the value of terms, \e j, and
 * this is covered in A098294(n) (https://oeis.org/A098294):
 * 
 * \f[ j = 1; n = 1 \f]
 * \f[ j = A098294(n-1) = \lceil (n-1) \cdot log_2 ( 3/2 ); n \ge 2; n \in \mathbf{N} \f]
 * 
 * All references to generating vector values which fall outside the range of \e generated values are taken to be zero.
 * This series is generated iteratively (with terms equal to zero omitted) as follows:
 * \f[ n=1; j = 1; a(1) = \sum_{i=1}^{j=1} a_i=a_1(1) = 1 \f]
 * \f[ n=2; j = A098294(1) = 1; a(2) = \sum_{i=1}^{j=1} a_i(2) = a_1(2) = a_1(1) = 1 \f]
 * \f[ n=3; j = A098294(2) = 2; a(3) = \sum_{i=1}^{j=2} a_i(3) = a_1(3) + a_2(3) = 1 + 1 = 2 \f]
 * \f[ n=4; j = A098294(3) = 2; a(4) = \sum_{i=1}^{j=2} a_i(4) = a_1(4) + a_2(4) = 1 + 2 = 3 \f]
 * \f[ n=5; j = A098294(4) = 3; a(5) = \sum_{i=1}^{j=3} a_i(5) = a_1(5) + a_2(5) + a_3(5) = 1 + 3 + 3  = 7 \f]
 * \f[ n=6; j = A098294(5) = 3; a(6) = \sum_{i=1}^{j=3} a_i(6) = a_1(6) + a_2(6) + a_3(6) = 1 + 4 + 7  = 12 \f]
 * \f[ n=7; j = A098294(6) = 4; a(7) = \sum_{i=1}^{j=4} a_i(7) = a_1(7) + a_2(7) + a_3(7) + a_4(7) = 1 + 5 + 12 + 12 = 30 \f]
 * \f[ n=8; j = A098294(7) = 5; a(8) = \sum_{i=1}^{j=5} a_i(8) = a_1(8) + a_2(8) + a_3(8) + a_4(8) + a_5(8) = 1 + 6 + 18 + 30 + 30 = 85 \f]
 * 
 * and so forth.
 */
class A100982 : public OEIS_base
{
    public:
        A100982();                                                  // Default constructor positions at first term in sequence
        A100982( int32_t index );                                   // Parameterized constructor positions at index term in sequence

        /**
         * @brief Returns a copy of the class vector holding the elements of \b a(n).
         * @return const std::vector< mpz_class > - A vector of multiple precision integers.
         * @details Returns a copy of the entire generating vector.  Changes made to the copy are not reflected back into the class.
         * This vector holds the individual generating elements which sum to \b a(n).
         */
        inline const std::vector< mpz_class > elements() const { return a100982_vec; };

        /**
         * @brief Return the length (number of elements) of the component vector
         * @return int32_t - The number of elements in the vector
         * @details Note that the sum of the elements in the component vector produces \b a(n) of the sequence.
         */
        inline int32_t size() const { return a100982_vec.size(); };

        mpz_class elem( int32_t index ) const;                      // Return the element of the generating vector given in index

        // Increment and decrement operators
        const mpz_class& operator++();                              // Prefix increment
        const mpz_class& operator--();                              // Prefix decrement

        /**
         * @brief Postfix increment to the next value in the series OEIS A100982.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator++(int) { return OEIS_base::operator++(0); };

        /**
         * @brief Postfix decrement to the previous value in the series OEIS A100982.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator--(int) { return OEIS_base::operator--(0); };

        // Virtual init() function which is used for default initialization of class variables
        virtual void init();                                        // Resets the class to the default state which is the first term

    protected:
        void init_local();                                          // Set initial values in derived class

        A022921 a022921_test;                                       /**< Used to determine when the last term in an \b a(n) expansion doubles. */
        std::vector< mpz_class > a100982_vec;                       /**< Vector to hold and manipulate all of the elements of \b a(n). */
};

/**
 * @brief Class definition for https://oeis.org/A186009.
 * @details Returns \b a(n) for the series OEIS A186009 where:
 * \f[ a(1) = a_1(1) = 1 \f]
 * \f[ a(n) = A100982(n-1); n \ge 2, n \in \mathbf{N_0} \f] which means 1 prepended to the A100982 sequence and starts with n=1.
 * This sequence is in some ways more important from a proof derivation perspective than A100982 because it is in this form that it
 * is possible to develop a simpler argument.  This is also called the Collatz dropping time residue.
 * 
 * We begin with the modified vector term generator introduced for A100982, namely:
 * 
 * \f[ a_i(n) = a_i(n-1) + a_{i-1}(n); i,n \in \mathbf{N} \f] and as before \f[ a(n) = \sum_{i=1}^j a_i(n); i,j,n \in \mathbf{N} \f]
 *  
 * However as a consequence of the prepending of the 1 to the A100982 sequence, the offsets in A186009 are one greater than before
 * and therefore the offsets to related OEIS sequences are one lesser than was the case for A100982.
 * Thus the last term for a given \e n of the generator vector is repeated for the A186009 sequence whenever:
 * \f[ A022921(n-3)=2, n \ge 3, n \in \mathbf{N_0} \f]
 *
 * Moving forward with the more succinct expression, based on the value of \e j covered in A098294(n) (https://oeis.org/A098294) is now:
 * 
 * \f[ j = 1; n = 1 \f]
 * \f[ j = 1; n = 2 \f]
 * \f[ j = A098294(n-2) = \lceil (n-2) \cdot log_2 ( 3/2 ); n \ge 3; n \in \mathbf{N} \f]
 * 
 * As with A100982, all references to generating vector values which fall outside the range of \e generated values are taken to be zero.
 * This series is generated iteratively (with terms equal to zero omitted) as follows:
 * \f[ n=1; j = 1; a(1) = \sum_{i=1}^{j=1} a_i=a_1(1) = 1 \f]
 * \f[ n=2; j = 1; a(2) = \sum_{i=1}^{j=1} a_i=a_1(2) = 1 \f]
 * \f[ n=3; j = A098294(1) = 1; a(3) = \sum_{i=1}^{j=1} a_i(3) = a_1(3) = 1 \f]
 * \f[ n=4; j = A098294(2) = 2; a(4) = \sum_{i=1}^{j=2} a_i(4) = a_1(4) + a_2(4) = 1 + 1 = 2 \f]
 * \f[ n=5; j = A098294(3) = 2; a(5) = \sum_{i=1}^{j=2} a_i(5) = a_1(5) + a_2(5) = 1 + 2 = 3 \f]
 * \f[ n=6; j = A098294(4) = 3; a(6) = \sum_{i=1}^{j=3} a_i(6) = a_1(6) + a_2(6) + a_3(6) = 1 + 3 + 3 = 7 \f]
 * \f[ n=7; j = A098294(5) = 3; a(7) = \sum_{i=1}^{j=3} a_i(7) = a_1(7) + a_2(7) + a_3(7) = 1 + 4 + 7 = 12 \f]
 * \f[ n=8; j = A098294(6) = 4; a(8) = \sum_{i=1}^{j=4} a_i(8) = a_1(8) + a_2(8) + a_3(8) + a_4(8) = 1 + 5 + 12 + 12 = 30 \f]
 * \f[ n=9; j = A098294(7) = 5; a(9) = \sum_{i=1}^{j=5} a_i(9) = a_1(9) + a_2(9) + a_3(9) + a_4(9) + a_5(9) = 1 + 6 + 18 + 30 + 30 = 85 \f]
 * 
 * Which as you can see generates the same sequence as A100982, but prepended with 1 so it has a higher index to produce the same \b a(n) value.
 */
class A186009 : public OEIS_base
{
    public:
        A186009();                                                  // Default constructor positions at first term in sequence
        A186009( int32_t index );                                   // Parameterized constructor positions at index term in sequence

        /**
         * @brief Return a copy of the class vector holding the elements of \b a(n).
         * @return const std::vector< mpz_class > - A vector of multiple precision integers.
         * @details Returns a copy of the entire generating vector.  Changes made to the copy are not reflected back into the class.
         * This vector holds the individual generating elements which sum to \b a(n).
         */
        inline const std::vector< mpz_class > elements() const { return a100982.elements(); };

        /**
         * @brief Return the length (number of elements) of the generating component vector
         * @return int32_t - The number of elements in the generating vector.
         * @details Note that the sum of the elements in the component vector produces \b a(n) of the sequence.
         */
        inline int32_t size() const { return a100982.size(); };

        /**
         * @brief Return a specific vector element value by index number.
         * @param index - The element in the generator vector to return.
         * @return mpz_class - The multiple precision integer (returns 0 if index out of range of vector)
         */
        inline mpz_class elem( int32_t index ) const { return a100982.elem( index ); };

        // Increment and decrement operators
        const mpz_class& operator++();                              // Prefix increment
        const mpz_class& operator--();                              // Prefix decrement

        /**
         * @brief Postfix increment to the next value in the series OEIS A186009.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator++(int) { return OEIS_base::operator++(0); };

        /**
         * @brief Postfix decrement to the previous value in the series OEIS A186009.
         * @return mpz_class - Returns the sequence term as multiple precision integer.
         */
        inline mpz_class operator--(int) { return OEIS_base::operator--(0); };

        // Virtual init() function which is used for default initialization of class variables
        virtual void init();                                        // Resets the class to the default state which is the first term

    protected:
        /**
         * @brief Initialize the derived class members.
         */
        inline  void init_local() { a100982.init(); };
                                          // Set initial values in derived class

        A100982 a100982;                                            /**< A186009 is identical to A100982 prepended with 1 */
};

/**
 * @brief This class is not a defined OEIS sequence.  \b C(n) is in fact a ratio where the numerator and denominator evolve with increasing \e n.
 * @details This class computes \b C(n) which is the summation of individual contributions of the novel convergence factor, \b N(n).
 * The novel convergence factor, \b N(n), is the \e incremental portion (between 0 and 1) of the positive integer space which converges
 * for an orbit of a length given by A020914(n).  In summary, \b N(n) computes the fraction of the positive integer space which are
 * \e guaranteed to converge to a smaller positive integer than the starting point positive integer for an orbit of length A020914(n), by
 * applying the iterative divide even numbers divide by 2 rule.
 * 
 * \b C(n) then tallies up all of the individual contributions of \b N(n) so as to determine the overall portion of the positive integer space
 * which is guaranteed to converge to a smaller positive integer value for all orbits up to length A020914(n).  As expected, the longer the orbit,
 * the greater the portion of positive integers that will converge to a smaller positive integer and thus become part of \b C(n).
 * 
 * The novel convergence factor \b N(n) can be expressed rather tersely in terms of other OEIS sequences. \b N(n) begins at n=0.  The A186009(n)
 * series adds one to the index because this series starts at n=1, not 0:
 * 
 * \f[ N(n) = \frac{ A186009( n + 1 ) }{ A000079( A020914( n ) ) }; n \in \mathbf{N_0}; N(n) \in \mathbf{Q} \f]
 * 
 * The first two terms are simple enough:
 * 
 * \f[ N(0) = \frac{A186009(1)}{A000079( A020814(0))} = \frac{1}{2^1} = \frac{1}{2}; n = 0 \f]
 * \f[ N(1) = \frac{A186009(2)}{A000079( A020814(1))} = \frac{1}{2^2} = \frac{1}{4}; n = 1 \f]
 * 
 * Beyond n=1, it is helpful to expand the expression in order to compute the \b N(n) series elements.  Note that there are some indexing
 * hurdles to overcome here as a result of the fact that different OEIS series have different starting offsets - and the fact that some series
 * have applicability only beyond a certain index.  For example, the series A098294(n-2) is applicable to A186009(n) for n>2 but since \b N(n)
 * is based on A186009(n+1), A098294(n-1) is applicable instead staring at n=2. 
 * 
 * \f[ N(n) = \frac{ \sum_{i=1}^{A098294(n-1)} a_i(n+1) ) }{ A000079( 1 + \lfloor \log_2( 3^n ) ) };
 * n \ge 2; i,n \in \mathbf{N}; N(n) \in \mathbf{Q} \f]
 * 
 * When fully expanded, \b N(n) generates the rather complex looking non-linear expression for n>1:
 * 
 * \f[ N(n) = \frac{ \sum_{i=1}^{\lceil (n-1) \cdot log_2 ( 3/2 )} a_i(n+1) ) }{ 2^{( 1 + \lfloor \log_2( 3^n ) )} };
 * n \ge 2; i,n \in \mathbf{N}; N(n) \in \mathbf{Q} \f]
 * 
 * The term in the numerator (from A186009) defines the vector a(n): \f[ a_i(n) = a_i(n-1) + a_{i-1}(n); i,n \in \mathbf{N} \f]
 * Reformulated to match the expression for \b N(n) above this becomes: \f[ a_i(n+1) = a_i(n) + a_{i-1}(n+1); i,n \in \mathbf{N_0} \f]
 * along with the initial condition: \f[ a_1(1) = 1 \f]
 * 
 * As mentioned before, what \b N(n) represents is the portion of integers which converge for a given orbit length.  So the \e total portion of
 * positive integers which is convergent is computed by taking the sum of the first \e n terms.  And as is true for \b N(n), \b C(n) is always
 * between 0 and 1 because it is a ratio of ( convergent positive integers ) / ( all positive integers ) and thus:
 * \f[ C(n) = \sum^{n} { N(n) }; n \in \mathbf{N_0}, N(n), C(n) \in \mathbf{Q} \f]
 * 
 * Therefore, the final expressions for the convergence function \b C(n) for the first \e n terms is thus (omitting terms equal to zero):
 * 
 * \f[ C(0) = N(0) = \frac{1}{2}; a(1) = a_1(1) = 1 \f]
 * \f[ C(1) = N(0) + N(1) = \frac{1}{2} + \frac{1}{4} = \frac{3}{4}; a(2) = a_1(2) = a_1(1) = 1 \f]
 * \f[ C(2) = N(0) + N(1) + N(2) = \frac{1}{2} + \frac{1}{4} + \frac{1}{16}; a(3) = a_1(3) = a_1(2) = 1 \f]
 * \f[ C(3) = N(0) + N(1) + N(2) + N(3) = \frac{1}{2} + \frac{1}{4} + \frac{1}{16} + \frac{2}{32}; a(4) = a_1(4) = a_1(3) + a_1(3) = 2 \f]
 * \f[ C(n) = \frac{1}{2} + \frac{1}{4} + \sum_{j=2}^n
 * \frac{ \sum_{i=1}^{\lceil (j-1) \cdot log_2 ( 3/2 )} a_i(j+1) ) }{ 2^{( 1 + \lfloor \log_2( 3^j ) ) } };
 * n \ge 2; i,j,n \in \mathbf{N}; N(n), C(n) \in \mathbf{Q} \f]
 * 
 * The proof the Collatz conjecture is based on arguments that:
 * 
 * \f[ \lim_{n \to \infty} C(n) = 1 \f]
 * 
 * Meaning that all integers eventually converge to a smaller value than where it started as the orbit length goes to infinity.
 * Then, by chaining together these sequences into convergent segments it is inevitable that starting with any positive integer
 * that the sequence will always converge to 1.
 */
class Cumulative : public OEIS_base
{
    public:
        Cumulative();                                               // Default constructor positions at first term in sequence
        Cumulative( int32_t index );                                // Parameterized constructor positions at index term in sequence

        // Return a const reference to the top and bottom parts of the fractions

        /**
         * @brief Return the numerator of \b C(n), which is the cumulative fraction.
         * @return const mpz_class& - Returns the numerator term as a const reference to a multiple precision integer.
         */
        inline const mpz_class& numerator() const { return oeis_term; };

        /**
         * @brief Return the denominator of the cumulative fraction as a multiple precision integer.
         * @return const mpz_class& - Returns the denominator term as a const reference to a multiple precision integer.
         */
        inline const mpz_class& denominator() const { return a000079(); };

        /**
         * @brief Return numerator of \b N(n), which is the incremental component
         * @return const mpz_class& - Returns the numerator of the novel component as a const reference to a multiple precision integer.
         */
        inline const mpz_class& novel() const { return a186009(); };

        /**
         * @brief Returns the exponent of the denominator which is a power of 2.
         * @return const int32_t - This exponent is what the power of 2 in the denominator is raise to.
         * This exponent in the denominator applies equally well to \b C(n) and \b N(n).
         */
        inline const int32_t exponent() const { return a000079.index(); };

        // Increment and decrement operators
        const mpz_class& operator++();                              // Prefix increment
        const mpz_class& operator--();                              // Prefix decrement

        /**
         * @brief Postfix increment to the next ratio in the Cumulative \b C(n) series.
         * @return mpz_class - A multiple precision integer.
         * This represents the numerator of the \b C(n) ratio.
         * The denominator is also calculated and retrieved with the denominator() method.
         */
        inline mpz_class operator++(int) { return OEIS_base::operator++(0); };

        /**
         * @brief Postfix decrement to the previous ratio in the Cumulative \b C(n) series.
         * @return mpz_class - A multiple precision integer.
         * This represents the numerator of the \b C(n) ratio.
         * The denominator is also calculated and retrieved with the denominator() method.
         */
        inline mpz_class operator--(int) { return OEIS_base::operator--(0); };

        // Virtual init() function which is used for default initialization of class variables
        virtual void init();                                        // Resets the class to the default state which is the first term

    protected:
        void init_local();                                          // Set initial values in derived class

        A000079 a000079;                                            /**< Member which holds the denominator for \b C(n). */
        A022921 a022921;                                            /**< Member which indicates whether to multiply denominator by 2 or 4. */
        A186009 a186009;                                            /**< The dropping time residue which is incremental convergence, \b N(n). */
};

#endif      // #ifdef gnu_mp

#endif      // #if !defined oeis_hpp
