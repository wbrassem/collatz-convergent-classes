/**
 * @file path.hpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief The orbit and path templates and derivatives are used to implement and space and execution speed efficient
 * way to store and retrieve Collatz orbits and convergent pathways
 * @version 1.1
 * @date 2025-12-20
 * 
 * @copyright Copyright (c) 2023-2025 Wayne Brassem
 * 
 */

#pragma once
#include "common.hpp"
#include "safe_arith.hpp"
#include <bit>

/**
 * @brief This constexpr function returns the correct orbit index based on the endianness of the host system.
 * @details Because the orbit_key_t union uses an array of 8-bit unsigned integers to represent orbit elements
 * the order of these elements in memory depends on the endianness of the host system.  This function abstracts
 * away that detail by returning the correct index for accessing the orbit elements in a way that is independent
 * of the underlying system architecture.
 * 
 * @param logical - The logical index (0 to 7) representing the position of the orbit element.
 * @return size_t - The physical index adjusted for system endianness.
 */
constexpr size_t orbit_index(size_t logical)
{
    if constexpr (std::endian::native == std::endian::little)
        return logical;
    else
        return 7 - logical;
}

/**
 * @brief The union orbit_key_t provides both an 8-element array of 8-bit unsigned integers and a 64-bit unsigned
 * integer representation.
 * 
 * As a result of this design, both parts are composed of the same number of bits (64) and the union structure itself can be used
 * either way as is most convenient.  However this introduces a scaling limit of the orbit_key_t orbital path representation.
 * 
 * Each unsigned 8-bit integer element in the array represents the number of divisor factors of 2 in the orbit element. A path is
 * represented by a series of these orbit elements.  Typically, each orbit element consists of small number (most often 1 or 2)
 * as this value represents the factors of 2 which are divisible following a 3n+1 Collatz Connection.  Put another way, if 2^k
 * is the maximum divisible power of 2 following a Connection then \e k is the value stored in the 8-element array of 8-bit
 * unsigned integers. But because it is stored as an 8-bit unsigned integer it is unable to represent more than 255 powers of 2 -
 * which could affect the representation of orbits for integers, \b n, larger than:
 * 
 * \f[ n = 2^{256} > 10^{77}; n \in \mathbf{N} \f]
 * 
 * For the intended purpose this limit is sufficiently distant to not impact the empirical validation of the proposed proof.  However,
 * if this limit proves too restrictive for other investigations you can increase the scale dramatically by storing the exponent of 2
 * as a 16-bit unsigned integer (uint16_t) at the cost of some additional memory and CPU for storing larger exponents and generating
 * longer linked lists for orbits consisting of more than 4 elements.
 * 
 * Using a 16-bit unsigned exponent representation would create a much higher orbit path element representation limit
 * for integers, \b n, larger than:
 * 
 * \f[ n = (2^{256})^2 = 2^{512}> 10^{154}; n \in \mathbf{N} \f]
 * 
 * Thus the orbit_t union holds the \b binary orbit path element represention - but this can also be interpreted as a single 64-bit
 * unsigned integer which is very useful for performing efficient key value comparison and assignment operations.
 */
union orbit_key_t
{
    uint64_t    i_key;                                                  /**< The unsigned 64-bit representation for ordinality checks. */
    uint8_t     c_key[ sizeof( uint64_t ) ];                            /**< Array of 8-bit uints each of which is an orbit element. */
};


/**
 * @brief This struct leverages the orbit_key_t union as nodes of a linked list to create orbital paths of arbitrary length.
 * @details This node is an element of a singly linked list (no back pointers) since all orbital paths are unidrectional in nature
 * and there are no meaningful semantics to a backward traversal of the orbit.
 * This struct also introduces convenient ordinal comparison operators which utilize the 64-bit unsigned integer representation
 * of the orbit_key_t union for fast and efficient integer comparison between unions.
 * 
 * The decision was made to create an entirely public implementation limiting the member functions to ones furnishing notational
 * conveniences (i.e. operators) and initialization rather than one with an excessive number of member functions since orbit_node_t
 * will itself become encapsulated in the orbit_t struct.  The orbit_t linked list struct will in turm become a protected member
 * within the larger context of the templated t_path< P > class inheritance tree.  Note that there is no explicit destructor for
 * this object as this task is handled more elegantly within the context of the orbit_t linked list struct.
 */
struct orbit_node_t
{
    public:
        orbit_node_t();
        orbit_node_t( const orbit_node_t &o );              // Copy constructor

        inline orbit_node_t &operator = ( const orbit_node_t &o );

        bool operator == ( const orbit_node_t &o ) const;
        bool operator <  ( const orbit_node_t &o ) const;
        bool operator >  ( const orbit_node_t &o ) const;

        void init();

        orbit_key_t     key;                                            /**< A 64-bit union representing the orbit elements. */
        orbit_node_t    *next;                                          /**< Pointer to the next union or nullptr. */
};


/**
 * @brief This struct implements the linked list orbital path of arbitrary length along with comparison operators in order compare
 * the orbits of a pair of orbit_t objects.
 * @details The orbit_t struct utilizes orbit_node_t structs to create linked lists consisting of one or more nodes.
 * A linked list consisting of a single orbit_node_t struct can represent an orbit of up to 8 orbit elements.  Beyond length 8,
 * this struct allocates and links additional orbit_node_t objects as needed to represent the complete orbit.
 * If the root element next pointer is nullptr the list can hold up to the first 8 elements in the orbit.
 * If the node itself has a non-nullptr next entry then the number of orbit element extends beyond the root node.
 * In all cases the link list terminates where the orbit_node_t next pointer is nullptr.
 * The root node is allocated off of the stack and is thus deallocated automaticaally once the object goes out of scope.
 * Additional nodes in the linked list are allocated off of the heap and thus need to be explicitly allocated and freed.
 */
struct orbit_t
{
    public:
        orbit_t();
        orbit_t( const orbit_t &o );              // Copy constructor
        ~orbit_t();

        std::string path() const;
        void append( const long divisors );
        inline int error() const;
        inline int path_len() const;

        orbit_t &operator = ( const orbit_t &ro );

        inline bool operator == ( const orbit_t &ro ) const;
        inline bool operator <  ( const orbit_t &ro ) const;
        inline bool operator >  ( const orbit_t &ro ) const;

        void clear();
        
    protected:
        orbit_node_t *new_node();
        void copy_list( const orbit_t &ro );
        void free_list();
        inline void init();

        orbit_node_t    root;                                           /**< The initial node of the orbit linked list */
        orbit_node_t    *curr;                                          /**< A pointer to the current node to write to */
        int             path_length;                                    /**< The orbit path length */
        int             error_mask;                                     /**< The error flag bitmask */
};


/**
 * @brief The templated abstract base class for path objects
 * @details By specifying (signed) "long" as the type you get standard internal representation which works fine for moderate integers.
 * By specifying mpz_class as the type you get the path variant using GNU multiple precision library for arbitrary large integers.
 * You may choose signed or unsigned integer types.  Signed integers allow you to explore anti-Collatz sequences.
 * @tparam P - The integer type (e.g. ulong, int, int64_t, mpz_class) on which the derived class will be based,
 */
template < class P >
class t_path
{
    public:
        /**< Static assertion to ensure the template parameter P is an integral type or mpz_class */
        static_assert(
            std::is_integral<P>::value
            #ifdef gnu_mp
            || std::is_same<P, mpz_class>::value
            #endif
            ,
            "t_path requires an integral type or mpz_class"
        );

        t_path();                                       // Default constructor

        t_path( const P &start );                       // Integer constructor
        t_path( const P &start, long classLen );        // Integer constructor with class length

        t_path( const std::string &input );             // Equivalence class constructor
        t_path( const char input[] );                   // Equivalence class constructor

        ~t_path();

        void setpath( const P &start, int max_factors = 0 );
        inline std::string getpath() const;

        std::string flow( long digits = -1 ) const;

        P ancestry( long &scale ) const;
        P next() const;

        inline P start() const;
        inline P max() const;
        inline const orbit_t& orbit();
        inline int sign() const;
        inline int error() const;

        inline long pathLength() const;
        inline long classLength() const;
        inline long pathFactors() const;
        inline long classFactors() const;
        inline long nextFactors() const;

        t_path< P > &operator = ( const t_path< P > &rp );
        t_path< P > &operator = ( const P &rp );                // Why not just assign to a new number?

        inline bool operator == ( const t_path< P > &rp ) const;
        inline bool operator != ( const t_path< P > &rp ) const;
        inline bool operator <  ( const t_path< P > &rp ) const;
        inline bool operator >  ( const t_path< P > &rp ) const;
        inline bool operator <= ( const t_path< P > &rp ) const;
        inline bool operator >= ( const t_path< P > &rp ) const;

        inline void prettyPrint() const;
        inline void prettyPrintClass() const;
        inline void prettyPrintPath() const;

        inline void prettyPrint( int max_digits ) const;
        inline void prettyPrint( long len, int max_digits ) const;
        inline void prettyPrint( long len, long indent, int max_digits ) const;
        inline void prettyPrintClass( int max_digits ) const;
        inline void prettyPrintPath( int max_digits ) const;
    
    protected:
        inline P connection( const P &terminus ) const;
        P parse( const std::string &input );

        long term( P &i ) const;
        long factor( P &branch, const P &start );
        long set_ec( const P &start );
        long get_ec_len( const std::string &input ) const;
        bool is_signed( const std::string &input ) const;

        void init( P start = 0 );
        void zeroize();

        int  int_sign;                                                  /**< Holds the sign of starting integer. */
        P start_int;                                                    /**< Holds the unsigned magnitude of the starting integer. */
        P max_int;                                                      /**< The largest integer magnitude in the convergent orbit. */
        orbit_t orb;                                                    /**< Holds the complete convergent path. */

        int path_factors;                                               /**< The number of divisor factors (2) in the orbit. */
        int ec_factors;                                                 /**< The total number of divisor factors in convergent orbit. */
        int next_factors;                                               /**< The number of divisor factors after the next connection. */
        int ec_len;                                                     /**< The length of the equivalence class representation. */

        int error_mask;                                                 /**< The bit positions are associate with error conditions. */
        collatz_regime regime;                                          /**< The Collatz regime for this path object. */
};

/**
 * @brief The default path is a signed 64-bit integer
 * @details Other types can be crafted from the t_path<> template, but you need to furnish pathPrint() and to_str() functions
 */
typedef t_path<int64_t> path;

/**
 * @brief Generic print function which all pretty print variants call
 * @details This function is called from various prettyPrint variants defined in t_path<>.  The function is intended to support the int64_t
 * template instantiation and leverages the PRId64 macro in the format string.
 * @param [in] start - The starting integer when the path object was created.
 * @param [in] length - The number of consecutive Collatz connection path downlegs needed to bring this integer to a smaller value.
 * @param [in] factors - The length of the equivalence class in the convergent flow - based on the starting integer and decreases.
 * @param [in] indent - Used to control the indenting of convergence classes - divergence adds 1, convergence decreased by the factors of 2.
 * @param [in] flow - The convergence flow whose length is initially set by the starting point and adjusts in the same way was indent does.
 * This std::string parameter is also used for printing out orbital path factors such as "0 1 1 1 3 2" for integer 79.
 * @param [in] max_digits - This is the column width of the first field and derived from the largest integer in the convergent orbit.
 */
void pathPrint( const int64_t &start, long length, long factors, int indent, std::string flow, int max_digits );

/**
 * @brief Return the int64_t integer decimal representation
 * @details This function is called from the t_path<int64_t>::flow() function.  The need for the function itself is that standard and
 * multiple precision integers use different calls to produce a printable output and therefore cannot be part of the common template
 * t_path(<> template class which they both utilize.  This function takes the internal signed 64-bit integer representation
 * to produce a std:string output.  The function leverages the PRId64 macro in the format string.
 * @param [in] remainder - Const reference to a int64_t integer.
 * @return std::string - The decimal string equivalent representation.
 */
std::string to_str( const int64_t &remainder );


#ifdef gnu_mp

/**
 * @brief The GNU multiple precision path template instatiation supports arbitrary precision integers
 * @details This instatiation requires the GNU MP headers and libraries for successful linking.
 * Inclusiong of this variant (and menu options) is controllable via the gnu_mp define.
 */
typedef t_path<mpz_class> mp_path;

/**
 * @brief Specialization of the safe_arith struct for mpz_class type
 * @details This specialization provides safe arithmetic operations for the mpz_class type.
 * Since GMP handles arbitrary precision integers, the operations simply forward to the
 * standard operators without additional overflow checks.
 */
template <>
struct safe_arith<mpz_class> {
    // GMP is arbitrarily large, so we just forward to normal operators
    static mpz_class add(const mpz_class& a, const mpz_class& b) { return a + b; }
    static mpz_class sub(const mpz_class& a, const mpz_class& b) { return a - b; }
    static mpz_class mul(const mpz_class& a, const mpz_class& b) { return a * b; }
};

/**
 * @brief Multiple precision print function which all pretty print mpz_class template variants call
 * @details This function is called from various prettyPrint variants defined in t_path<>.  The function is intended to support the mpz_class
 * template instantiation.   In the case of a GNU multiple precision integer it requires the gmp_printf() extensions since the standard
 * printf() does not support mpz_class argumnts.
 * @param [in] start - The starting integer when the mp_path object was created.
 * @param [in] length - The number of consecutive Collatz connection path downlegs needed to bring this integer to a smaller value.
 * @param [in] factors - The length of the equivalence class in the convergent flow - based on the starting integer and decreases.
 * @param [in] indent - Used to control the indenting of convergence classes - divergence adds 1, convergence decreased by the factors of 2.
 * @param [in] flow - The convergence flow whose length is initially set by the starting point and adjusts in the same way was indent does.
 * This std::string parameter is also used for printing out orbital path factors such as "0 1 1 1 3 2" for integer 79.
 * @param [in] max_digits - This is the column width of the first field and derived from the largest integer in the convergent orbit.
 */
void pathPrint( const mpz_class &start, long length, long factors, int indent, std::string flow, int max_digits );

/**
 * @brief Return the GNU Multiple precision integer decimal representation
 * @details This function is called from various prettyPrint variants defined in t_path<>.  The function is intended to support the mpz_class
 * template instantiation.   In the case of a GNU multiple precision integer it requires the gmp_printf() extensions since the standard
 * printf() does not support mpz_class argumnts.
 * @param [in] remainder - Const reference to a mpz_class integer.
 * @return std::string - The decimal string equivalent representation of the multiple precision integer.
 */
std::string to_str( const mpz_class &remainder );

#endif

// Maybe inheritence is overkill - all you really need is local constant in the object to change the behavior?
// Negative Collatz with "corrected" constants
class antipath : public path
{
    public:
        antipath();
        ~antipath();

    protected:
};
