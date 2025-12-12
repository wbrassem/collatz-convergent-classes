/**
 * @file menu.cpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief The purpose for this code is to provide a menu of choices for running various Collatz related functions.
 * The menu has a couple of switches which can be used to enable/disable multiple precision and computation speed.
 * The menu also has a sub-menu which let's you look as specific OEIS sequences and related derivatives to the proof.
 * @version 0.1
 * @date 2023-04-24
 * 
 * @copyright Copyright (c) 2023
 */

#include <cinttypes>                // For the PRId16 and PRId64 printf format specifiers

#include "common.hpp"
#include "btree.hpp"
#include "path.cpp"
#include "oeis.hpp"

// Wrapper to prevent duplication if header included twice
#if !defined menu_cpp
#define menu_cpp

// Default Collatz connection values
int statics::divisor = 2;
int statics::multiplier = 3;
int statics::addend = 1;

// Error mask codes
int statics::overflow = 0x1;
int statics::memory = 0x2;

// Speed and progress controls
bool statics::speed = false;
int  statics::blip_modulus = 0;

// Print control variables
int statics::count = 0;
int statics::width = 0;

/**
 * @brief Find the difference between two timestamps
 * @details Returns a string which represents human readable difference in time between two timestamps.
 * @param [in] end - A time object repesenting the terminus of the time interval.
 * @param [in] beginning - A time object repesenting the start of the time interval.
 * @param [out] diff_str - A std::string object holding the textual difference between the beginning and the end.
 * @return true  - Returns true  if the interval is greater than 29 seconds along with the string duration.
 * @return false - Returns false if the interval is less    than 30 seconds and leaves the string unchanged.
 */
bool get_time_diff( const time_t end, const time_t beginning, std::string &diff_str )
{
    // Calculate the duration of execution in seconds
    int duration = end - beginning;
    int days, hours, minutes, seconds;

    // If execution time is less than 30 seconds them return nothing
    if ( duration < 30 )
        return false;

    minutes  = duration / 60;
    seconds  = duration % 60;
    hours    = minutes / 60;
    minutes  = minutes % 60;
    days     = hours / 24;
    hours    = hours % 24;

    // Build string up from components
    if ( days )
        diff_str += std::to_string( days ) + " day"  + ( days==1 ? " " : "s ");

    if ( hours )
        diff_str += std::to_string( hours ) + " hour" + ( hours==1 ? " " : "s ");

    if ( minutes )
        diff_str += std::to_string( minutes ) + " minute"  + ( minutes==1 ? " and " : "s and ");

    if ( seconds )
        diff_str += std::to_string( seconds ) + " second" + ( seconds==1 ? "." : "s.");

    return true;
}

/**
 * @defgroup btree Tree traversal callback functions
 * @brief Optional function arguments to binary tree forward and reverse traversals.
 * @details
 * 
 * The btree class accepts an optional function pointer to the btree::constForwardIterator() and btree::constReverseIterator()
 * tree iterators.
 * Likewise, the template t_btree<K> class accepts an optional function pointer to the t_btree<K>::constForwardIterator()
 * and t_btree<K>::constReverseIterator() tree iterators.
 * 
 * These arguments provide a callback function which is optionally called during tree traversals which allows for
 * the customization of the display output as the nodes are visited in the order chosen.
 * 
 * \see btree, t_btree
 * @{
 */

/**
 * @brief Prints out the convergent paths and the number of occurences of the path
 * @details This function is called in support of option \b l in the OEIS sub-menu by the
 * template function \ref t_convergent_path<T>.
 * @param [in] len - The number of downlegs which is the length of the orbit.
 * @param [in] nodes - The number of discrete pathtways of a given length (e.g. 1, 1, 1, 2, 3, 7).
 * @param [in] frequency - The total number of times the positive integer matched the given length over the range.
 */
inline void node_path_print( const long len, const long nodes, const long frequency )
{
    printf("%8ld %6ld (%8ld): %ld\n", len, len-1, nodes, frequency );
}

/**
 * @brief Prints out the orbit length, number of discrete paths and the total number of occurences
 * @details This function is called in support of option \b k in the OEIS sub-menu by the template function \ref t_convergent_eq<T>.
 * @param [in] len - The classlen which is the length of the orbit.
 * @param [in] nodes - The number of discrete pathtways of a given length.
 * @param [in] frequency - The total number of times the positive integer matched the given length over the range.
 */
inline void node_class_print( const long len, const long nodes, const long frequency )
{
    printf("%8ld (%8ld): %ld\n", len, nodes, frequency );
}

/**
 * @brief Prints out the number of occurences of a path (downlegs) of a given length
 * @details This function is called in support of options \b h and \b i in the OEIS sub-menu by the
 * template functions \ref t_dist_legs<T> and \ref t_dist_eq<T>.
 * @param [in] key - The downleg count (path length).
 * @param [in] count - Number of times the downleg count occured over the range.
 */
inline void const_body_downleg_print( const long key, const long count )
{
    printf( "For %3ld: downleg count is %ld\n", key, count );
}

/**
 * @brief Prints the frequency, number of downlegs and the orbit path flow.
 * @details This function is called in support of option \b l in the OEIS sub-menu which itself is called by the
 * template function \ref t_convergent_path<T>.
 * The btree and template t_btree<K> classes accept a function pointer for the forward and
 * reverse iterator member function which provides a callback function which is optionally called during tree traversals.
 * @param [in] o - The const orbit_t object containing the orbit path
 * @param [in] count - The frequency of a particular orbit in the range
 * @see btree, t_btree
 */
inline void const_orbit_print( const orbit_t &o, const long count )
{
    printf( "Count %*ld, downlegs %4d: flow is %s\n", statics::count, count, o.path_len(), o.path().c_str() );
}

/**
 * @brief Prints the frequency, path length and the orbit path flow.
 * @details This function is called in support of option \b j in the OEIS sub-menu called by \ref t_dist_path<T>.
 * The btree and template t_btree<K> classes accept a function pointer for the forward and reverse iterator
 * member function which provides a callback function which is optionally called during tree traversals.
 * @tparam P - The path object type which can be built on regular precision or multiple precision integers.
 * @param [in] p - The path object itself.
 * @param [in] count - The frequency that the path object occured in the exponent range.
 * @see btree, t_btree
 */
template < class P >
inline void t_const_path_downleg_print( const P &p, const long count )
{
    printf( "Count %10ld, downlegs %4lu: flow is %s\n", count, p.pathLength(), p.getpath().c_str() );
}

/**
 * @brief Prints the frequency, equivalence class string length and the orbit path flow.
 * @details This function is called in support of option \b k in the OEIS sub-menu called by \ref t_dist_path<T>.
 * The btree and t_btree<T> classes accept a function pointer for the forward and reverse iterator member function
 * which provides a callback function which is optionally called during tree traversals.
 * @tparam P - The path object type which can be built on regular precision or multiple precision integers.
 * @param [in] p - The path object itself.
 * @param [in] count - The frequency that the path object occured in the exponent range.
 */
template < class P >
inline void t_ec_print( const P &p, long count )
{
    // The equivalence class string length is one less that the total length due to the first character being the sign
    printf( "Count %*ld, class length %4lu: flow is %s\n", statics::count, count, p.length()-1, p.c_str() );
}

/** @} */  // end of btree tree traversal group

/**
 * @brief Conputes the integer range fomr the equivalence class length
 * @details The range of integers in order to create an evenly balanced distribution of orbits occurs naturally at
 * \f[ 3 \cdot 2^n; n \in \mathbf{N_0} \f]
 * Over these ranges the composition of \f$ 2^n \f$ produces predicable and even frequency distibutions for a given orbit length.
 * @param [in] digits - The nubmer of digit (length) of the equivalence class.
 * @return long - Returns the upper limit of the range of positive integers.
 */
long find_range( long digits )
{
    long range = ( digits > 0 ? statics::multiplier : 0 );

    // Compute the integer range as a function of the length of the convergent equivalence class
    for ( long i=0; i<digits; ++i )
        range *= statics::divisor;

    return range;
}

/**
 * @brief Produce a periodic blip message to terminal.
 * @param [in] i - The current integer being processed.
 * @param [in] blip - The integer spacing between successive blips.
 * @param [in] range - The upper limit of the range - used to compute the percentage complete.
 */
inline void make_blip( long i, long blip, long range )
{
    // Create a periodic blip when dealing with large exponents
    if ( ( i % blip ) == statics::blip_modulus )            // A bit dorky but is consitent with speed mode
    {
        // Calculate the number of digits needed to represent the largest integer in range for output format alignment
        int digits = base10_digits( range );

        float done = i * 100.0;
        done /= range;
        printf( "Blip: %*ld (%7.3f%%)\n", digits, i, done );
    }
}

/**
 * @defgroup main_menu Main menu functions
 * @brief Group of functions responsible for displaying and implementing the main menu.
 * @details
 * 
 * The main menu focuses on providing output which shows how integers, equivalence classes and pathways (orbits) behave.
 * These menu items calculate and categorize integers the hard way - one at a time.  This is arguaby computationally expensive,
 * but it does not lie.  These methods do not rely on any theories of operation nor are directly associated with the proof
 * although they do provide empirical evidence which supports the proof.
 * 
 * This menu is templated with path, P, and integer, I, types in order to optionally support multiple precision integers.
 * These class types are used to create class objects directly or are passed along into templated functions such as range
 * oriented functions.
 * 
 * It is possible to enable "speed optimizations" which eliminates all of the even numbers, and half of the odd numbers which
 * are known to converge after a single Collatz connection.  So in effect with this \b cheat enables you eliminate 3/4 of the
 * positive integer space that needs to be examined for a given length of orbit for those menu options \b i through \b l which
 * utilize exponents or length to examine a range of integers.  By default this option is off.
 * 
 * It is also possible to enable "multiple precision" integers if you compiled with the GNU MP libraries.  This allows you to
 * process arbitrarily large integers if desired.  By default this option is off.
 * 
 * @{
 */

/**
 * @brief Display the equivalence class convergence orbit for a given digit length
 * @details This function is not directly used by any menu function but is called indirectly via \ref t_seq_path<T>
 * providing menu option \b b and by \ref t_seq_eq<T> providing menu support for option \b f.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param p - Path object of class P.
 * @param pathlen - Length of path (orbit).
 * @param digits - Desired length of equivalence class representation.  Note this can be longer than the minimum required.
 * @return I - Returns the integer in the representation chosen in the template.
 */
template < class P, class I >
I t_seq( const P &p, long pathlen, long digits )
{
    I curr = p.start(), next = 0;
    long dlegs = p.classFactors();
    int indent = 1;

    // Sanity check for digits range
    digits = ( digits > 0 ? digits : 0 );
    int base10 = base10_digits( p.max() );

    std::cout << "Convergence sequence for " << p.start() << " is:" << std::endl;

    // Loop until all digits used up or you encounter convergence
    while ( pathlen >= 0 )
    {
        P ecFlow( curr, digits );
        ecFlow.prettyPrint( digits, indent, base10 );

        // Break out of loop when you encounter convergence
        if ( abs( curr ) < abs( p.start() ) )
        {
            break;
        }

        // Find the next integer in the orbit - but watch for the zero degenerate case
        if ( ( next = ecFlow.next() ) == 0 )
        {
            return 0;
        }

        // if next does not divide evenly into current then it is some multiple of a new terminus
        if ( curr % next != 0 )
        {
            long facts = ecFlow.nextFactors();      // Grab the number of factors of the next connection

            // If divergent, the depth increases
            if ( facts == 1 )
                indent++;

            // Otherwise it's convergent, but check that indent does not go below 1
            else
                indent = ( facts >= indent ) ? 1 : indent + 1 - facts;

            // Reduce the length of the equivalence class representation
            digits -= facts;
            pathlen -= facts;           // This is the new termination knob which eliminates infinite terminal loops now that digits is single purposed
        }

        // Set up for the next iteration
        curr = next;
    }

    return next;
}

/**
 * @brief Display equivalence class convergence based on path factors
 * @details This function is in support of menu option \b b.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param p - Path object of class P.
 * @return I - Returns the integer in the representation chosen in the template.
 */
template < class P, class I >
inline I t_seq_path( const P &p )
{
    return t_seq< P, I>( p, p.classFactors(), p.pathFactors() );
}

/**
 * @brief Display equivalence class convergence based on path length
 * @details This function is in support of menu option \b l.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param p - Path object of class P.
 * @return I - Returns the integer in the representation chosen in the template.
 */
template < class P, class I >
inline I t_seq_eq( P p )
{
    return t_seq< P, I >( p, p.classFactors(), p.classLength() );
}

/**
 * @brief Chain together convergent sequences in the quest of global terminus given an integer
 * @details This function is in support of menu option \b c. This iteratively calls upon the \ref t_seq_path<P,I>
 * (menu option \b b) so that it shows the complete convergent orbit which terminates at 1.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param integer - Starting integer for which to compute the convergent orbit
 */
template < class P, class I >
void t_term_path( const I &integer )
{
    I last_int, next_int = integer;

    // Do while loop which terminates once it encounters a loop
    do
    {
        last_int = next_int;
        next_int = t_seq_path< P, I >( last_int );
    }
    // Continue until you find a value whose magnitude is less than you started with
    while ( abs( last_int ) > abs( next_int) );

    // If that loop does NOT include the global terminus then you found a local loop
    if ( abs( last_int ) != 1 )
    {
        std::cout << "Non-Global terminus loop detected in terminal orbit" << std::endl;
    }
}

// template < class P, class I >
// inline void t_term_path( const P &p )
// {
//     t_term_path< P, I >( p.start() );
// }

// Chain together convergent sequences in the quest of global terminus given a string object
// template < class P, class I >
// inline void t_term_path( const std::string &str )
// {
//     t_term_path< P, I >( P(str).start() );
// }

// Chain together convergent sequences in the quest of global terminus
// template < class P, class I >
// inline void t_term_ec( const I &integer )
// {
//     t_term_path< P, I >( integer );
// }

// template < class P, class I >
// inline void t_term_ec( const P &p )
// {
//     t_term_path< P, I >( p.start() );
// }

/**
 * @brief Chain together convergent sequences in the quest of global terminus given a convergence class string object
 * @details This function is in support of menu option \b g. This calls upon the \ref t_term_path<P,I>
 * (menu option \b c) using the path integer so that it shows the complete convergent orbit which terminates at 1.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param str - The starting string object representation of the convergence class for which to compute the convergent orbit.
 */
template < class P, class I >
void t_term_ec( const std::string &str )
{
    P p( str );
    t_term_path< P, I >( p.start() );
}

/**
 * @brief Create a histogram of the convergent legs
 * @details This function is in support of menu option \b h. A range of positive integers is examined as governed by the
 * provided exponent.  The range is equal to \f[ 3 \cdot 2^e \f] where e is the exponent argument.
 * 
 * The function creates a histogram by computing the number of downlegs required for convergence to a smaller integer.
 * and incrementing the counter for unique to each downleg count in a btree object.  It then displays the distibution.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param exponent - The range of positive integers evaluated is which 2 raised to exponent times 3 as in the formula shown.
 * @see btree
 */
template < class P, class I >
void t_dist_legs( long exponent )
{
    btree histogram;                // binary tree which grows as needed to store leg counts
    
    int suppress = 12, blipexp = 14;

    int sign = sgn( exponent );     // Use the sign of the exponent to select negative integers
    exponent = abs( exponent );     // Once the sign has been recorded use the positive value for computation

    long range = find_range( exponent );
    long blip  = find_range( blipexp );

    if ( exponent >= suppress )
        std::cout << "Dist legs suppression: " << suppress << " or greater" << std::endl;

    // Iterate through all the odd numbers in the exponent range
    for ( long i = 1; i <= range; ++i )
    {
        P p( i * sign );
        long legs = p.pathLength();

        // Insert node for legs or increment existing node
        histogram.insert( legs );

        // If output suppression is in effect display a progress blip
        if ( exponent > blipexp )
            make_blip( i, blip, range );

        // Otherwise output the path if within the suppress range
        else if ( exponent <= suppress )
            p.prettyPrintPath( base10_digits( range ) );
    }

    // Counter which keeps track of the total distribution size
    long sum = histogram.constForwardIterator( &const_body_downleg_print );
    std::cout << "Total of " << sum << " entries in all categories" << std::endl;
}

/**
 * @brief Create a histogram of the minimum length of the convergent equivalence classes
 * @details This function is in support of menu option \b i. A range of positive integers is examined as governed by the
 * provided exponent.  The range is equal to \f[ 3 \cdot 2^e \f] where e is the exponent argument.
 * 
 * The function creates a histogram by placing integers into convergent equivalence classes to a smaller integer.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param exponent - The range of positive integers evaluated is which 2 raised to exponent times 3 as in the formula shown.
 */
template < class P, class I >
void t_dist_eq( long exponent )
{
    btree histogram;                    // binary tree which grows as needed to store leg counts

    int suppress = 12, blipexp = 14;

    int sign = sgn( exponent );         // Use the sign of the exponent to select negative integers
    exponent = abs( exponent );         // Once the sign has been recorded use the positive value for computation

    long range = find_range( exponent );
    long blip  = find_range( blipexp );

    if ( exponent >= suppress )
        std::cout << "Function dist_eq: Suppressing solutions for exponents " << suppress << " or greater" << std::endl;

    // Loop through all of the possible integers in range
    for ( long i = 1; i <= range; ++i )
    {
        P p( i * sign );
        int p_len = p.pathLength();

        // Insert node for legs or increment existing node
        histogram.insert( p_len );

        // If output suppression is in effect display a progress blip
        if ( exponent > blipexp )
            make_blip( i, blip, range );

        // Otherwise output the equivalence class if within the suppress range
        else if ( exponent <= suppress )
            p.prettyPrintClass( base10_digits( range ) );
    }

    // Counter which keeps track of the total distribution size
    long sum = histogram.constForwardIterator( &const_body_downleg_print );
    std::cout << "Total of " << sum << " entries in all categories" << std::endl;
}

/**
 * @brief Create a histogram of convergent pathways for a given exponent
 * @details This function is in support of menu option \b j. A range of positive integers is examined as governed by the
 * provided exponent.  The range is equal to \f[ 3 \cdot 2^e \f] where e is the exponent argument.
 * 
 * The function creates a histogram by computing the integer's convergent orbit to a smaller integer.  It is actually this pattern
 * which leads to the proof because it is possible to utilize the orbit pattens - chaotic though it is - to develop an argument
 * that all orbit eventually lead to a smaller positive integer.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param exponent - The range of positive integers evaluated is which 2 raised to exponent times 3 as in the formula shown.
 */
template < class P, class I >
void t_dist_path( long exponent )
{
    t_btree< P > histogram;            // Structure to efficiently compare convergent paths

    // Control output and blip settings
    int suppress = 12, blipexp = 14;

    int sign = sgn( exponent );     // Use the sign of the exponent to select negative integers
    exponent = abs(exponent);       // Once the sign has been recorded use the positive value for computation

    long range = find_range( exponent );
    long blip  = find_range( blipexp );

    if ( exponent >= suppress )
        std::cout << "Dist_path suppression: " << suppress << " or greater" << std::endl;

    // Iterate through all the odd numbers in the exponent range
    for ( long i = 1; i <= range; ++i )
    {
        P p( i * sign );
        histogram.insert( p );

        // If output suppression is in effect display a progress blip
        if ( exponent > blipexp )
            make_blip( i, blip, range );

        // Otherwise output the path if within the suppress range
        else if ( exponent <= suppress )
            p.prettyPrintPath( base10_digits( range ) );
    }

    histogram.constForwardIterator( &t_const_path_downleg_print< P > );
}

/**
 * @brief Find all convergent equivalence classes of a given length
 * @details This function is in support of menu option \b k. A range of positive integers is examined as governed by the
 * provided exponent.  The range is equal to \f[ 3 \cdot 2^e \f] where e is the number of digits.
 * 
 * The function creates a histogram by computing the integer's convergent equivalence class and counts the number of occurences.
 * This routine actually generates \f[ 3 \cdot A186009(n); n \in \mathbf{N} \f] in the form of the number of unique equivalence
 * classes. The multiple of 3 is because within the equivalence class strcuture there are three different flavours which behave
 * identically and evolve in lock step with one another.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param digits - The maximum number of digits in the equivalence class to search for
 * @see A186009
 */
template < class P, class I >
void t_convergent_eq( long digits )
{
    int suppress = 12, blipexp = 14, summary = 25;

    int sign = sgn( digits );     // Use the sign of the exponent to select negative integers
    digits = abs( digits );       // Once the sign has been recorded use the positive value for computation

    t_btree< std::string >  string_tree_array[ digits+1 ];   // Array of binary trees of path objects with individual int counters
    int                     string_len_counts[ digits+1 ];   // Array to hold counters of each length (aggregate multiple pathways)

    // Initialize the dynamically sized array
    for ( long i=0; i<=digits; ++i )
    {
        string_len_counts[ i ] = 0;
    }

    long range = find_range( digits );
    long blip  = find_range( blipexp );

    // Largest number of digits in path frequency counters (associated with even number) is digits for 1/6 of the entire range
    statics::count = base10_digits( range / 6 );

    std::cout << "Convergent integers of equivalence class length " << digits << " out of a set of " << range << " possible" << std::endl;

    long found = 0;    

    // Loop through all of the possible integers in range
    for ( long i = 1; i <= range; ++i )
    {
        P p( i * sign );

        // If output suppression is in effect display a progress blip
        if ( digits > blipexp )
            make_blip( i, blip, range );

        // Otherwise output the equivalence class
        else if ( digits <= suppress )
            p.prettyPrintClass( base10_digits( range ) );

        // If the convergent equivalence class length is less than or equal to the goal (digits) then insert into tree
        if ( p.pathFactors() <= digits )
        {
            found ++;           // Increment the number of convergent paritions

            t_btree< std::string > *string_tree_element = &( string_tree_array[ p.pathFactors() ] );
            string_tree_element -> insert( p.flow( p.pathFactors() ) );        // Insert the equivalence class representation
        }
    }

    // Print out the header if you are going to output all of the equivalence classes
    if ( digits <= summary )
        std::cout << "\nSummary of convergent equivalence classes with up to " << digits << " digits in length " << std::endl;


    // Process tree elements in ascending order
    for ( int i = digits; i >= 0; --i )
    {
        // Print out the classes if the number of digits in flow is less than or equal to the summary limit
        if ( digits <= summary )
            string_len_counts[ i ] = string_tree_array[ i ].constForwardIterator( &t_ec_print< std::string > );
        
        // Otherwise collect the counters, but suppress the output of the equivalance classes
        else
            string_len_counts[ i ] = string_tree_array[ i ].constForwardIterator( nullptr );
    }

    // Counter which keeps track of the total distribution size
    int sum = 0;

    printf("\nClasslen (Pathways): Frequency\n");

    // Loop through the array of binary tree looking for case where there is at least one node in the tree
    for ( long i = 0; i <= digits; ++i )
    {
        t_btree< std::string > *string_tree_element = &( string_tree_array[ i ] );
        long len_counts = string_len_counts[ i ];
        long nodes = string_tree_element -> nodes();

        // Print only if there are any nodes in tree of a given length
        if ( nodes )
            node_class_print( i, nodes, len_counts );

        sum += len_counts;
    }

    std::cout << "Found " << found << " convergent equivalence classes of length " << digits << " out of "
                << range << " total (" << found/3 << "/" << range/3 << ")." << std::endl;
}

/**
 * @brief Find all convergent paths up to a given number of divisor factors
 * @details This function is in support of menu option \b l. A range of positive integers is examined as governed by the
 * provided exponent.  The range is equal to \f[ 3 \cdot 2^e \f] where e is the path length equal to the powers of 2.
 * 
 * The function creates a histogram by computing the integer's convergent path orbit and increments the number of occurences.
 * This routine actually generates \f[ A186009(n); n \in \mathbf{N} \f] in the form of the number of unique pathways and as
 * a result lies at the very heart of the Collatz conjecture.  It is hard to overstate the importance of this result as the Collatz
 * dropping time pattern which is found in this manner is what makes the proof so difficult to find.  This routine does not separate
 * the different flavours of equivalences class and therefore provides A186009 directly unlike \ref t_convergent_eq.
 * 
 * This function also displays the overall cumulative convergence, \b C(n), for a path length defined as the number of factors of 2
 * in the orbit.  The main difference from the OEIS submenu is the output provided here is obtained by fastidious processing and
 * sorting of positive integers over a given range, whereas the OEIS submenu \e presumes the existence of the dropping time pattern
 * and processes based on that presumption. 
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param path_length - The maximum number of factors of 2 in the convergent orbital path
 * @see A186009
 */
template < class P, class I >
void t_convergent_path( long path_length )
{
    int suppress = 12, blipexp = path_length > 24 ? ( path_length - 9 ) : 15, summary = 25;

    int sign = sgn( path_length );     // Use the sign of the exponent to select negative integers
    path_length = abs( path_length );       // Once the sign has been recorded use the positive value for computation

    t_btree< orbit_t >  orbit_tree_array[ path_length+1 ];   // Array of binary trees of path objects with individual int counters
    long                orbit_len_counts[ path_length+1 ];   // Array to hold counters of each length (aggregate multiple pathways)

    // Initialize the dynamically sized array
    for ( long i=0; i<=path_length; ++i )
    {
        orbit_len_counts[ i ] = 0;
    }

    long range = find_range( path_length);
    long blip  = find_range( blipexp );

    // Largest number of digits in path frequency counters (associated with even number) is digits in half of the entire range
    statics::count = base10_digits( range / 2 );

    std::cout << "Convergent integers of path length " << path_length << " from 1 up to " << range << std::endl;

    I max_terminus = 0, max_of_max = 0;           // Record the biggest divergent integer and the integer which spawned it

    // Default loop settings
    long start = 1, increment = 1;
    statics::blip_modulus = 0;      

    // Okay, so if in speed mode cheat a bit and only target (mod 4) so i % 4 = 3 because that's where the action is
    // But serously only enable this cheat if you are working with path lengths at least as long as the blip exponent
    if ( statics::speed && ( path_length >= blipexp ) )
    {
        start = 3;
        increment = 4;
        statics::blip_modulus = 3;
    }

    // Loop through all of the possible integers in range
    for ( long i = start; i <= range; i+=increment )
    {
        P p( i * sign, path_length );

        // If error encountered exit
        if ( int error = p.error() )
        {
            std::cout << "Error: Error building path object error code " << error << std::endl;
        }

        // Keep track of the maximums encountered in the range
        if ( p.max() > max_of_max )
        {
            max_terminus = i;
            max_of_max = p.max();
        }

        // If output suppression is in effect display a progress blip
        if ( path_length >= suppress )
            make_blip( i, blip, range );

        // Otherwise output the path
        else
            p.prettyPrintPath( base10_digits( range ) );

        // If the convergent path length is less than or equal to the goal (path_length) then insert into tree
        if ( p.pathFactors() <= path_length )
        {
            t_btree< orbit_t > *t_path_tree_element = &( orbit_tree_array[ p.pathLength() ] );
            t_path_tree_element -> insert( p.orbit() );        // Insert the path object
        }
    }

    // okay lets try to figure out the number of digits in the largest frequency
    // t_btree< P > *t_max_path_tree_element = &( t_path_tree_array[ 1 ] );
    // ulong two_count = t_max_path_tree_element->search( P(2) );
    // int longest = base10_digits( two_count );
    // std::cout << "Path with the highest frequency with a flow of 1 is "  << two_count << " which has " << longest << " digits." << std::endl;

    if ( path_length <= summary )
        std::cout << "\nSummary of convergent paths with up to " << path_length << " factors of " << statics::divisor << std::endl;

    // Process tree elements in ascending order
    for ( int i = path_length; i >= 0; --i )
    {
        // Print out the actual pathways if the number of digits in flow is less than or equal to the summary limit
        if ( path_length <= summary )
            orbit_len_counts[ i ] = orbit_tree_array[ i ].constForwardIterator( &const_orbit_print );
            // orbit_len_counts[ i ] = orbit_tree_array[ i ].constForwardIterator( &t_const_orbit_print );
        
        // Otherwise collect the counters, but suppress the output of the pathways
        else
            orbit_len_counts[ i ] = orbit_tree_array[ i ].constForwardIterator( nullptr );
    }


    // Counter which keeps track of the total distribution size
    long sum = 0;

    printf("\nDownlegs Uplegs (Pathways): Frequency\n");

    // If in speed mode this move fakes the results which we all know it would have otherwise found honestly
    // This little cheat eliminates all even and half of the odd positive integers which convergence after one connection
    // So in effect 3/4 of uninteresting positive integer space is avoided by starting with 3 and incrementing by 4
    if ( statics::speed )
    {
        long freq;
        
        // Fake the even numbers
        freq = range/2;
        node_path_print( 1, 1, freq );
        sum += freq;

        // Fake the odd numbers which only diverge once
        freq = range/4;
        node_path_print( 2, 1, freq);
        sum += freq;
    }

    // Loop through the array of binary tree looking for case where there is at least one node in the tree
    for ( long i = 0; i <= path_length; ++i )
    {
        t_btree< orbit_t > *orbit_tree_element = &( orbit_tree_array[ i ] );
        long len_counts = orbit_len_counts[ i ];
        long nodes = orbit_tree_element -> nodes();

        // Print only if there are any nodes in tree of a given length
        if ( nodes )
            node_path_print( i, nodes, len_counts );

        sum += len_counts;
    }

    // If in speed mode this measurement can be inaccurate
    if ( !statics::speed )
    {
        // Traverse the key which calls the print function in the constructor
        std::cout << "Largest integer divergence in range is " << max_of_max << " (" << base10_digits(max_of_max)
                        << " digits), beginning with terminus " << max_terminus << std::endl;
    }

    // Print out final summary
    std::cout << "Found " << sum << " convergent paths out of " << range << " total (" << sum/3 << "/" << range/3 << 
                ") with up to " << path_length << " factors of " << statics::divisor << std::endl;
}

/** @} */  // end of main_menu Main menu functions

/**
 * @brief Execute the choice from the main menu
 * @details Depending on the option selected the user is prompted for additional information in order to complete the computation.
 * This can be in the form of an integer or an equivalence class string.  Once input is collected it is then passed into the
 * appropriate constructor for a path object or used as an argument into one of the supporting implementation functions.  This
 * itself is templated to support built-in types (long, uint32_t, etc.) or mpz_class if GNU MP libraries were compiled in.
 * @tparam P - Path object type.  Choices are \ref path and \ref mp_path if compiled with GNU MP libraries.
 * @tparam I - Interger object type.  Choices are built-in types (long, unit32_t, etc.) and mpz_class if compiled with GNU MP libraries.
 * @param [in] ch - The main menu selection (char)
 * @return true - The value true indicates that the user has not chosen to exit
 * @return false - The value false indicates that the user has chosen to exit
 * @ingroup main_menu
 */
template < class P, class I >
bool t_serve_menu_selection( char ch )
{
    bool again = true, timed = true;
    time_t start, finish;

    std::string eq_class;
    I t_integer = 0;
    long long_integer = 0;

    // Switch statement to gather the menu choice
    switch ( ch )
    {
        case 'a':   
        case 'b':
        case 'c':
        case 'd':   {   std::cout << "Enter an integer ";
                        std::cin >> t_integer;
                        break;
                    }
        case 'e':
        case 'f':
        case 'g':   {   std::cout << "Enter an equivalence class ";
                        std::cin >> eq_class;
                        break;
                    }
        case 'h':
        case 'i':
        case 'j':   {   std::cout << "Enter an exponent ";
                        std::cin >> long_integer;
                        break;
                    }
        case 'k':
        case 'l':   {   std::cout << "Enter an equivalence class length ";
                        std::cin >> long_integer;
                        break;
                    }
    }

#ifdef gnu_mp
    // This would be a good place to check for input which exceed standard precision integers

    // For example... now 30 is only int - long is 64 bits
//    if ( long_integer > 30 )
//        std::cout << "Warning: exponent is getting big enough to benefit from multiple precision integers" << std::endl;
#endif

    // Capture the start time
    time( &start );

    // Switch statement to act on the menu choice
    switch ( ch )
    {
        case 'a':   {   P p( t_integer );
                        p.prettyPrint();
                        break;
                    }
        case 'b':   {   t_seq_path< P, I >( t_integer );
                        break;
                    }
        case 'c':   {   t_term_path< P, I >( t_integer );
                        break;
                    }
        case 'd':   {   long parents, scale = 0;
                        std::cout << "How many parent equivalence classes to generate: ";
                        std::cin >> parents;

                        // Reset the timer because of the additional input prompt
                        time(&start);
                        P p( t_integer );

                        for ( int i=1; i<= parents; ++i )
                        {
                            I next = p.ancestry( ++scale );

                            // Check to see if no parents were found
                            if ( next == 0 )
                            {
                                std::cout << "No parents exist for this integer" << std::endl;
                                break;
                            }
                            else
                            {
                                P p( next );
                                p.prettyPrint();
                            }
                        }
                        break;
                    }
        case 'e':   {   P p( eq_class );
                        p.prettyPrint();
                        break;
                    }
        case 'f':   {   t_seq_eq< P, I >( eq_class );
                        break;
                    }
        case 'g':   {   t_term_ec< P, I >( eq_class );
                        break;
                    }
        case 'h':   {   t_dist_legs< P, I >( long_integer );
                        break;
                    }
        case 'i':   {   t_dist_eq< P, I>( long_integer );
                        break;
                    }
        case 'j':   {   t_dist_path< P, I >( long_integer );
                        break;
                    }
        case 'k':   {   t_convergent_eq< P, I >( long_integer );
                        break;
                    }
        case 'l':   {   t_convergent_path< P, I >( long_integer );
                        break;
                    }

        case 'x':   {
                        again = false;
                        timed = false;
                        break;
                    }
        default:    {
                        std::cout << "Not a valid menu selection, please try again" << std::endl;
                        again = true;
                        timed = false;
                    }
    }
    
    // Below should really be some function you call which returns a std::string object

    // If the menu selection ran calculate the time taken to execute
    if ( timed )
    {
        time( &finish );
        std::string time_diff;

        // If the time difference was more than one second
        if ( get_time_diff( finish, start, time_diff ) )
            std::cout << "Time to execute task: " << time_diff << std::endl;
    } 

    return again;
}


/**
 * @defgroup oeis OEIS submenu functions
 * @brief Function calls to display initial terms of Collatz related OEIS sequences and derivatives.
 * @details
 * 
 * The Online Encyclopedia of Integer Sequences (OEIS) http://oeis.org submenu allows the user to explore the
 * outputs of a number of Collatz related sequences - some of which are very basic and others which are quite involved.
 * The purpose here is to illustrate how the 3n + 1 connection condition impacts the way the positive integer space reacts
 * and how it manages to converge despite all the chaotic behaviour.
 * 
 * This sub-menu also includes one menu items which is not actually an OEIS sequence.  It is however critical in that it is this
 * function which computes the novel convergence factor, \b N(n), and the cumulative convergence factor, \b C(n).
 * 
 * As a result of the very large integers frequently required for any useful examination, it was decided to make this entire
 * sub-menu dependent on the existence of the GNU multiple precision libraries.  As a result if the code is compiled without
 * these libraries this submenu is suppressed.
 * 
 * @{
 */

#ifdef gnu_mp
/**
 * @brief Leverage polymprohic base class member functions to print out derive class term indices and values
 * @param [in] o - pointer to a default constructed OEIS derived class object
 * @param [in] t - Number of terms to display
 */
void OEIS_seq( OEIS_base *o, int t )
{
    // Print out the first 50 elements of the sequence
    for ( int i=1; i<=t; ++i ) {
        std::cout << "n = " << o->index() << ": " << o->operator()() << std::endl;
        o->operator++();
    }
}

/**
 * @brief Display the novel and cumulative convergence numerators and the common denominator
 * @param [in] c - Pointer to the default constructed \ref Cumulative object
 * @param [in] t - Number of terms to display
 * @see Cumulative
 */
void Cumulative_seq( Cumulative *c, int t )
{
    // Print out the first t elements of the sequence
    for ( int i=1; i<=t; ++i ) {
        std::cout << "n = " << c->index() << ", numerator of N(n) = " << c->novel();
        // std::cout << ", exponent = " << c->exponent();
        std::cout << "; numerator of C(n) = " << c->numerator() << "; denominator = " << c->denominator() << std::endl;

        c->operator++();
    }
}

/**
 * @brief Generate the sequence terms based on the menu selection
 * @details By default this function produces the first 40 term of each sequence.  This can be change with the submenu option t
 * which accept an integer for the number of terms.  To exit to the main menu enter 'x'.
 * @param [in] ch - The menu selection
 * @param [in] t - The number of terms to display
 */
void serve_OEIS_menu_selection( char ch, int &t )
{
    std::string eq_class;
    long t_integer = 0;
    long long_integer = 0;

    // Switch statement to gather the menu choice
    switch ( ch )
    {
        case 'a':   { A000079 s; OEIS_seq( &s, t ); break; }
        case 'b':   { A002379 s; OEIS_seq( &s, t ); break; }
        case 'c':   { A020914 s; OEIS_seq( &s, t ); break; }
        case 'd':   { A056576 s; OEIS_seq( &s, t ); break; }
        case 'e':   { A022921 s; OEIS_seq( &s, t ); break; }
        case 'f':   { A100982 s; OEIS_seq( &s, t ); break; }
        case 'g':   { A186009 s; OEIS_seq( &s, t ); break; }
        case 'h':   { A098294 s; OEIS_seq( &s, t ); break; }

        case 'n':   { Cumulative c; Cumulative_seq( &c, t ); break; }

        case 't':   {   std::cout << "Enter an integer ";
                        std::cin >> t;
                        break;
                    }
        default:    {
                        std::cout << "Not a valid menu selection, please try again" << std::endl;
                    }
    }
}

/**
 * @brief Menu called by selecting \b o from the main menu
 * @details The OEIS classes and derivative which rely on the GNU multiple precision libraries
 * @see menu
 */
void OEIS_menu()
{
    // Some loop asking for input
    std::string input, eq_class;
    int t = 40;

    while ( input != "x" )
    {
        // Menu of OEIS choices

        std::cout << std::endl;

        std::cout << "a: A000079(n)" << std::endl;
        std::cout << "b: A002379(n)" << std::endl;
        std::cout << "c: A020914(n)" << std::endl;
        std::cout << "d: A056576(n)" << std::endl;
        std::cout << "e: A022921(n)" << std::endl;
        std::cout << "f: A100982(n)" << std::endl;
        std::cout << "g: A186009(n)" << std::endl;
        std::cout << "h: A098294(n)" << std::endl;

        std::cout << std::endl;
        std::cout << "n: Novel N(n) and Cumulative C(n) convergence" << std::endl;
      
        std::cout << std::endl;
        std::cout << "t: Number of terms to display.  Current setting is " << t << std::endl;

        // Final menu choice is to gracefully exit the program
        std::cout << std::endl;
        std::cout << "x: Exit" << std::endl;

        // Clear the input stream prior to prompting user for menu selection
        std::cin.clear();           // Clears error flags

        // Prompt for and collection menu selection
        std::cout << "Please make a selection: ";        
        std::cin >> input;

        // Only look at the first character in the string - any additional characters are silently ignored
        char ch = *input.c_str();

        // Call the routine to server the selection
        serve_OEIS_menu_selection( ch, t );
    }
}
#endif // #ifdef gnu_mp

/** @} */  // oeis OEIS Submenu functions

/**
 * @brief The program main menu
 * @details The primary menu does all of it's computation based on the property of the positive integer, equivalence class or
 * range of positive integers.  There is a submenu of OEIS sequences and derivative which do all of their mathematics based
 * on the properties of the sequences themselves.
 * @ingroup main_menu
 */
void menu()
{
    // Some loop asking for input
    std::string input, eq_class;

#ifdef gnu_mp
    // If GNU multiple precision library is enabled create a variable to track multiple precision switch
    bool mp = false;
#endif // #ifdef gnu_mp

    while ( input != "x" )
    {
        // Standard menu of choices


// Why is there no option in here to simply print out the convergent legs for a given integer?

        std::cout << std::endl;

        std::cout << "a: Enter an integer   to find the minimum    equ-class" << std::endl;
        std::cout << "b: Enter an integer   to find the convergent equ-class  segment  flow" << std::endl;
        std::cout << "c: Enter an integer   to find the convergent equ-class  terminal flow" << std::endl;
        std::cout << "d: Enter an integer   to find the parent     equ-classes" << std::endl;

        std::cout << "e: Enter an equ-class to find the leading    terminus" << std::endl;
        std::cout << "f: Enter an equ-class to find the convergent equ-class  segment  flow" << std::endl;
        std::cout << "g: Enter an equ-class to find the convergent equ-class  terminal flow" << std::endl;

        std::cout << "h: Enter an exponent  to find the convergent legs       counts" << std::endl;
        std::cout << "i: Enter an exponent  to find the convergent equ-class  counts" << std::endl;
        std::cout << "j: Enter an exponent  to find the convergent pathway    counts" << std::endl;

        // What is a "length" anyways - is it an exponent?

        std::cout << "k: Enter a  length    to find the convergent equ-class  counts" << std::endl;
        std::cout << "l: Enter a  length    to find the convergent pathway    counts" << std::endl << std::endl;

// The ability to switch on multiple precision and the ability to see OEIS sequences relies on GNU libraries
#ifdef gnu_mp
        // If GNU multiple precision library is enabled add a sub-menu option to display specific OEIS sequences
        std::cout << "o: Display OEIS sequence sub-menu (sets multiple precision to on)" << std::endl << std::endl;

        // If GNU multiple precision library is enabled add a menus item to toggle multiple precision
        std::cout << "p: Toggle multiple precision integers:    Current setting is " << ( mp ? "on" : "off" ) << std::endl;
#endif // #ifdef gnu_mp

        std::cout << "s: Toggle execution speed optimizations:  Current setting is " << ( statics::speed ? "on" : "off" ) << std::endl;

        // This would be a good place to be able to adjust the default Collatz constants

        // Final menu choice is to gracefully exit the program
        std::cout << std::endl;
        std::cout << "x: Exit" << std::endl;

        // Clear the input stream prior to prompting user for menu selection
        std::cin.clear();           // Clears error flags

        // Prompt for and collection menu selection
        std::cout << "Please make a selection: ";        
        std::cin >> input;

        // Only look at the first character in the string - any additional characters are silently ignored
        char ch = *input.c_str();
        bool again = true;

        switch ( ch )
        {
#ifdef gnu_mp
            case 'o':   {   mp = true;
                            OEIS_menu();
                            break;      // This is where you ought to call the OEIS submenu
                        }
            case 'p':   {   mp = !mp;
                            break;
                        }
#endif // #ifdef gnu_mp
            case 's':   {   statics::speed = !statics::speed;
                            break;
                        }
            default:    {
#ifdef gnu_mp
                            // If the multiple precision switch is active
                            if ( mp )

                                // Call the the multiple precision menu function template
                                again = t_serve_menu_selection< mp_path, mpz_class> ( ch );

                            else
#endif // #ifdef gnu_mp
                                // Otherwise call the standard precision menu function template
                                again = t_serve_menu_selection< path, long > ( ch );
                        }
        }

        // Check to see if user has requested to terminate the program
        if ( !again )
            break;
    }
}

#endif // #if !defined menu_cpp