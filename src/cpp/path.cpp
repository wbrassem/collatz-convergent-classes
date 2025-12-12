/**
 * @file path.cpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief Implementation of the orbit templated path object.  The path object is used to probe the integer convergence patterns.
 * The path objects record the equivalence class and convergent path information.
 * The underlying orbit data structure is a union which allows for faster comparison than simple character representation
 * @version 0.1
 * @date 2023-05-29
 * 
 * @copyright Copyright (c) 2023
 */


// This include brings in the basic definitions
#include "path.hpp"


// orbit_node_t implementations

/**
 * @brief Default constructor for a new orbit_node_t::orbit_node_t object
 * @details Sets the 64-bit unsigned integer representation to 0 - which also zeroes out the 8-bit uint array elements.
 * Also initializes the pointer to the next union structure to the nullptr.
 */
orbit_node_t::orbit_node_t()
{
    // Clear all state in initial node
    init();
};

/**
 * @brief Copy constructor for a new orbit_node_t::orbit_node_t object
 * @details Copies the orbit - including all linked unions - into a new orbit_node_t orbit.  Member function calls the assignment
 * operator=() which performs the same function.
 * @param [in] ro - Const reference to the orbit to copy from.
 */
orbit_node_t::orbit_node_t( const orbit_node_t &ro )
{
    // Call the assignment operator
    operator=( ro );
};

/**
 * @brief Assignment operator
 * @details This implementation copies the contents from one linked list node into this node. Note that this implementation is
 * \b not responsible to free any existing linked list allocations - this is the reponsibility of the orbit_t struct object.
 * @param [in] ro - Const reference to the orbit to copy from.
 * @return orbit_node_t& - Returns a reference to this object so that assignment operations can be chained together if desired.
 */
orbit_node_t &orbit_node_t::operator = ( const orbit_node_t &ro )
{
    // Copy the orbit segment as long integer (fastest)
    key.i_key = ro.key.i_key;

    return *this;
};

/**
 * @brief Ordinal equivalency operator
 * @details This implementation utilizes the 64-bit unsigned integer representation for performing comparisons.
 * When not equivalent for an orbit_node_t node element then this function returns false immediately.
 * If equivalent, it checks to see if both orbits have pointers to the next elements in the orbit and if so recursively
 * follows both links until either a difference is found (false) or else equivalency is established (true) since neither
 * orbit has a link to another orbit_node_t struct \b and the 64-bit integers are equivalent.
 * @param [in] ro - Const reference to the orbit to compare to.
 * @return true - The orbit consists of equivalent elements
 * @return false - The orbit does not consist of equivalent elements
 */
bool orbit_node_t::operator == ( const orbit_node_t &ro ) const
{
    // Is it equivalent comparing the 64-bit integer representation?
    if ( key.i_key == ro.key.i_key )
    {
        orbit_node_t *lop = next;
        orbit_node_t *rop = ro.next;

        // If there is more keep going through the list recursively
        if ( lop && rop )
            return *lop == *rop;

        // Otherwise they are indeed equivalent orbits
        else
            return true;
    }

    return false;
}

/**
 * @brief Determines if this orbit is mathematically less than another
 * @details This implementation utilizes the 64-bit unsigned integer representation for performing comparisons.
 * When not equivalent for an orbit_node_t node element, it returns the less than (<) direct comparison of the 64-bit integers.
 * If equivalent, it checks to see if both orbits have pointers to the next elements in the orbit and if so recursively
 * follows both links until either a difference is found (in which case a (<) direct comparison is performed), or else equivalency
 * is established (false) since neither orbit has a link to another orbit_node_t struct \b and the 64-bit integers are equivalent.
 * @param [in] ro - Const reference to the orbit to compare to.
 * @return true - Returns true if this orbit is less than the one provided.
 * @return false - Returns false if this orbit is greater than or equal to the one provided.
 */
bool orbit_node_t::operator < ( const orbit_node_t &ro ) const
{
    // Is it equivalent comparing the 64-bit integer representation?
    if ( key.i_key == ro.key.i_key )
    {
        orbit_node_t *lop = next;
        orbit_node_t *rop = ro.next;

        // If there is more keep going through the list recursively
        if ( lop && rop )
            return *lop < *rop;

        // Otherwise they are indeed equivalent orbits
        else
            return false;
    }

    return key.i_key < ro.key.i_key;
}

/**
 * @brief Determines if this orbit is mathematically greater than another
 * @details This implementation utilizes the 64-bit integer unsigned representation for performing comparisons.
 * When not equivalent for an orbit_node_t node element, it returns the less than (>) direct comparison of the 64-bit integers.
 * If equivalent, it checks to see if both orbits have pointers to the next elements in the orbit and if so recursively
 * follows both links until either a difference is found (in which case a (>) direct comparison is performed), or else equivalency
 * is established (false) since neither orbit has a link to another orbit_node_t struct \b and the 64-bit integers are equivalent.
 * @param [in] ro - Const reference to the orbit to compare to.
 * @return true - Returns true if this orbit is greater than the one provided.
 * @return false - Returns false if this orbit is less than or equal to the one provided.
 */
bool orbit_node_t::operator > ( const orbit_node_t &ro ) const
{
    // Is it equivalent comparing the 64-bit integer representation?
    if ( key.i_key == ro.key.i_key )
    {
        orbit_node_t *lop = next;
        orbit_node_t *rop = ro.next;

        // If there is more keep going through the list recursively
        if ( lop && rop )
            return *lop > *rop;

        // Otherwise they are indeed equivalent orbits
        else
            return false;
    }

    return key.i_key > ro.key.i_key;
}

/**
 * @brief Initialzes the struct member to known initial values of enpty key (0) and null pointer to next orbit_key_t.
 * @details Typically this member function would be protected, but it's public to allow orbit_t objects to call it.
 */
void orbit_node_t::init()
{
    // Clear all state in initial node
    key.i_key = 0;
    next = nullptr;
}

// struct orbit_t implentations

/**
 * @brief Default constructor for a new orbit_t::orbit_t object
 * @details Calls the init() member function to initialize local variables
 */
orbit_t::orbit_t()
{
    // Clear all state
    init();
};

/**
 * @brief Copy constructor for a new orbit_t::orbit_t object
 * @details First calls the init() function to initalize local variables followed by copy_list() to dunplicate the linked list.
 * @param [in] ro - Objec to copy from which may include information in links lists
 */
orbit_t::orbit_t( const orbit_t &ro )
{
    // Clear all state
    init();
    
    // Copy the list contents
    copy_list( ro );
};

/**
 * @brief Destroys the orbit_t::orbit_t object
 * @details Calls the free_list() member function to deallocate any nodes allocated from the heap via the \b new operator.
 */
orbit_t::~orbit_t()
{
    // Free any memory allocated
    free_list();
};

/**
 * @brief Retrieves the orbital path as a std::string object (e.g. "0 1 2 1 2 1 3")
 * @details Decodes the binary orbit information stored in the linked list and creates a std::string representation.
 * @return std::string 
 */
std::string orbit_t::path() const
{
    std::string path_str;
    const orbit_node_t *curr = nullptr;

    // Create a string output for every leg of the orbital path
    for ( int i = 0; i < path_length; ++i )
    {
        long pos = i % sizeof( orbit_key_t );

        // Whenever you are at the beginning of a union position the pointer appropriately
        if ( pos == 0 )
        {
            // If already pointing to a union go to the next
            if ( curr )
                curr = curr->next;
            
            // Otherwise you are starting from the first union in the list
            else
                curr = &root;
        }

        // Append an integer to the path representing the number of divisor factors, then add a space as a separator
        path_str += std::to_string( curr->key.c_key[sizeof( orbit_key_t ) - pos - 1] ) + ' ';
    }

    // remove the trailing space
    path_str.pop_back();

    return path_str;
}

/**
 * @brief Append a numerical path element to the sequence and store in the orbit object
 * @details Since there is a maximum of 8 unsigned 8-bit integers in each orbit_key_t union this member function first
 * determines whether or not there is any available space left in the current orbit_node_t.  If not, it allocates and links
 * a new one before storing the power of two into the linked list.
 * @param [in] divisors - The const power of 2 exponent which can be factored after a Collatz 3n+1 connection.
 */
void orbit_t::append( const long divisors )
{
    // Calculate the position within the orbit to append the divisors
    long pos = path_length % sizeof( orbit_key_t );

    // If overrun of any given union then link another one
    if ( pos == 0 )
    {
        // If another node needs to be allocated and linked to the list
        if ( pos < path_length )
        {
            // If unable to allocate memory for new then exit
            if ( !(curr -> next = new_node()) ) {
                error_mask |= statics::memory;
                return;
            }

            // Move current node pointer to the new node
            curr = curr -> next;
        }
    }

    // Insert path elements starting from the highest byte order position so that integer comparison operations work normally
    curr->key.c_key[ sizeof( uint64_t ) - pos - 1] = divisors;

    // Increment the path length
    path_length++;
}

/**
 * @brief Returns the error mask
 * @details The only error currently raised is the statics::memory memory allocation failure.
 * @return int - The error mask
 */
int orbit_t::error() const
{
    return error_mask;
}

/**
 * @brief Return the current orbital path length
 * @details The path length is the number of downlegs in the current orbit.
 * @return int - The orbital path length
 */
int orbit_t::path_len() const
{
    return path_length;
}

/**
 * @brief Assignment operator
 * @details The assignment operator must first elegantly handle the case were it already has a linked list which includes nodes
 * which were allocated from the heap.  It first frees any allocated nodes and reinitialized the object variables.
 * @param [in] ro - Reference to a const linked list orbit_t object to replicate including copying the key values of all linked nodes.
 * @return orbit_t& - Return a reference to the new orbit to allow for chaining assignment operations
 */
orbit_t &orbit_t::operator = ( const orbit_t &ro )
{
    // Free any allocated memory from additional union nodes
    free_list();

    // Initialize the root member
    root.init();

    // Copy the state from ro orbt_t to this
    root = ro.root;
    curr = &root;

    // Copy the list elements to a new linked list
    copy_list( ro );
    return *this;
}

/**
 * @brief Ordinal equivalency operator
 * @details The equivalency operator relies on the orbit_node_t equivalency operator
 * @param [in] ro - Const reference to the orbit to compare to.
 * @return true - The orbits are identical.
 * @return false - The orbits are not identical.
 */
bool orbit_t::operator == ( const orbit_t &ro ) const
{
    return root == ro.root;
}

/**
 * @brief Determines if this orbit is mathematically less than another
 * @details The less than (<) operator relies on the orbit_node_t less than (<) operator
 * @param [in] ro - Const reference to the orbit to compare to.
 * @return true - Returns true if this orbit is less than the one provided.
 * @return false - Returns false if this orbit is greater than or equal to the one provided.
 */
bool orbit_t::operator < ( const orbit_t &ro ) const
{
    return root < ro.root;
}

/**
 * @brief Determines if this orbit is mathematically greater than another
 * @details The greater than (>) operator relies on the orbit_node_t greater than (>) operator
 * @param [in] ro - Const reference to the orbit to compare to.
 * @return true - Returns true if this orbit is greater than the one provided.
 * @return false - Returns false if this orbit is less than or equal to the one provided.
 */
bool orbit_t::operator > ( const orbit_t &ro ) const
{
    return root > ro.root;
}

/**
 * @brief Clears the state of the orbit object and reinitializes.
 */
void orbit_t::clear()
{
    free_list();
    init();
}

/**
 * @brief Creates a new linked list node
 * @details The menber function attempts to allocate a new orbit_node_t object from the heap.  If a failure occurs in memory
 * allocation, it prints an error to stdout and sets the memory allocation failure mask bit.  Otherwise, it returns a pointer
 * to the newly allocated node.
 * @return orbit_node_t* - A pointer to the new node (if successful), or nullptr if memory allocation failed.
 */
orbit_node_t *orbit_t::new_node()
{
    // Allocate new node object for additional orbit storage space
    orbit_node_t *next = new orbit_node_t;

    // Check to see if memory allocated correctly
    if ( !next )
    {
        std::cout << "Error: Memory allocation error" << std::endl; 
        error_mask |= statics::memory;     // Maybe restore the error thing afterwards
    }

    return next;
}

/**
 * @brief Copies a linked list
 * @details Copies the orbit_key_t values which contain the path information into the local root node and allocates additional
 * nodes as required to copy any linked node key values.
 * @param [in] ro - Const reference to the linked list to replicate.
 */
void orbit_t::copy_list( const orbit_t &ro )
{
    orbit_node_t *source = ro.root.next;

    // Copy the path length and error mask
    path_length = ro.path_length;
    error_mask = ro.error_mask;
    
    // Copy the root orbit as a long integer
    root.key.i_key = ro.root.key.i_key;

    // Keep looping so long as there is another union in the list
    while ( source )
    {
        // Create a new node to accept the orbit
        if ( !( curr -> next = new_node() ) ) {
            error_mask |= statics::memory;
            return;
        }

        // Position on the new node(s)
        curr = curr->next;

        // Copy the orbit as a long integer
        curr->key.i_key = source->key.i_key;

        // Move to the next orbit object if there is one
        source = source->next;
    }
};

/**
 * @brief Frees up any allocated nodes in the linked list
 * @details Deallocaes any orbit_node_t objects allocated from the heap.  Deallocation is necessary whenever there is an assignement
 * operation since it may already have existing heap allocations and when the orbit_t object is destructed.
 */
void orbit_t::free_list()
{
    orbit_node_t *next_union = root.next;

    // Loop until you've deleted them all
    while ( next_union )
    {
        orbit_node_t *curr = next_union;
        next_union = next_union -> next;
        delete curr;
    }
};

/**
 * @brief Initializes the object variables
 * @details The current node pointer keeps track of the orbit_node_t object to write to.  Initially the object available for
 * writing to is the stack allocated root member so the current node is set to the address of the root node.
 */
void orbit_t::init()
{
    // Clear all state in initial node
    path_length = error_mask = 0;

    // Set the root node integer key to 0 and the next pointer to nullptr
    root.key.i_key = 0;
    root.next = nullptr;

    // Set the pointer to the root node
    curr = &root;
};

// Template class implementation for the path class variants

// Template t_path constructors

/**
 * @brief Default constructor for a new t_path< P >::t_path object
 * @details Calls the init() member function to initialize the member variables.
 * @tparam P - The integer data type.
 */
template < class P >
t_path< P >::t_path()
{
    init();
}

/**
 * @brief Constructor for a new t_path< P >::t_path object given an integer
 * @details Calculates the path for the provided integer and determines it's nominal equivalence class.
 * @tparam P - The integer data type.
 * @param [in] start - The starting integer of the template type - or one which has a defined conversion to the template type.
 */
template < class P >
t_path< P >::t_path( const P &start )
{
    operator=( start );
}

/**
 * @brief Constructor for a new t_path< P >::t_path object given an integer and an equivalence class length
 * @details Calculates the path for the provided integer and assigns it to its specified equivalence class length.
 * @tparam P - The integer data type.
 * @param [in] start - The const starting integer of the template type - or one which has a defined conversion to the template type.
 * @param [in] class_len - The specified equivalence class length.
 */
template < class P >
t_path< P >::t_path( const P &start, long class_len )
{
    setpath( start, class_len );

    // Check for negative values and assign
    ec_len = ( class_len < 0 ) ? 0 : class_len;
}

/**
 * @brief Constructor for a new t_path< P >::t_path object given its equivalence class string representation
 * @details Parses the equivalence class to extract the leading integer from the class.
 * @tparam P - The integer data type.
 * @param [in] input - Const reference to an equivalence class std::string representation.
 */
template < class P >
t_path< P >::t_path( const std::string &input )
{
    // Parse the input string to come up with an integer representation which can then be used to create to the object
    P val = parse( input );

    setpath( val );
    ec_len = get_ec_len( input );
}

/**
 * @brief Constructor for a new t_path< P >::t_path object given its equivalence class character array representation
 * @details Parses the equivalence class to extract the leading integer from the class.
 * @tparam P - The integer data type.
 * @param [in] input - Const reference to an equivalence class character array representation.
 */
template < class P >
t_path< P >::t_path( const char input[] )
{
    // Parse the characters array to come up with an integer representation which can then be used to create to the object
    P val = parse( input );

    setpath( val );
    ec_len = get_ec_len( input );
}

/**
 * @brief Destructor for the t_path< P >::t_path object
 * @details Currently just zeroes out all values and as such is likely not strictly necessary.
 * @tparam P - The integer data type.
 */
template < class P >
t_path< P >::~t_path()
{
    zeroize();
}

/**
 * @brief Set the value for path object (deletes existing state first)
 * @details This member function does quite a bit.  It is called by t_path<> constrcutors or directly for existing objects.
 * It first established the integer starting point for convergence and then calculates the entire oribital path needed to
 * be able to converge to a smaller integer.  This keep track of the total number of factors of 2 used in convergence as well
 * as the number of divergent 3n+1 connections.  The main loop exits once the new integer in the orbit is no longer greater
 * that the starting integer.  This means it also exists upon equivalency which is needed as the termination condition at the
 * global terminus of 1.  Finally it initialize the number of factors of 2 in the equivalence class (which is greatet than
 * or equal to the path factors) and looks ahead to see the number of factors of 2 after then ext 3n+1 connections.
 * @tparam P - The integer data type.
 * @param [in] start - The initial integer that you need to search for a convergent flow for.
 * @param [in] max_factors - Optional upper limit on the number of factors of 2 in the path (defaults to zero).
 */
template < class P >
void t_path< P >::setpath( const P &start, int max_factors )
{
    // Clear any existing state and initialize to the start value
    init( start );

    // Set the current and maximum integer in orbit to starting integer
    P current_int = start;
    max_int = start;

    // orbit_node_t *curr_orb = nullptr;

    // Eliminate the even numbers first, they converge immediately
    if ( start % statics::divisor == 0 )
    {
        // All numbers which divide evenly by the divisor converge locally with a minimum of a single factor of the divisor
        orb.append( 1 );
        path_factors++;
    }
    
    // Otherwise its odd, diverges and you need to figure out how it converges
    else
    {
        // All other paths begin with 0 since they cannot be divided evenly by the divisor
        orb.append( 0 );

        // Loop until you find a smaller magnitude
        do
        {
            long leg = 0;
            P last_int = current_int;

            // Find the next even integer using the connection
            current_int = connection( current_int );

            // if the sign has flipped we've hit a representation limit - print message and break out of loop
            if ( sign() != sgn( current_int ) )
            {
                std::cout << "Error: Signed integer overflow for starting integer " << start_int << 
                        ".  Connection for " << last_int << " in orbit was too big to represent." << std::endl;

                error_mask |= statics::overflow;         // Set the overflow bit

                // Set the current integer to that last reasonable value
                current_int = last_int;
                break;        // Should we break out or not???  I think so because rest of path is invalid anyways
            }

            // Record the largest integer achieved during convergent segment
            if ( current_int > max_int )
                max_int = current_int;

            // Return the number of divisors and actually factor them out of current_int - but stop if result is less that start_int
            leg = factor( current_int, start );

            // Keep track of the total number of factors of divisor used in covergence sequence
            path_factors += leg;

            // Abort the complete object creation process if the current number of path factors is greater than the target
            if ( statics::speed && ( path_factors > max_factors ) )
                return;              // Somewhat even more dramatic to exit part way

            // Append the next path element representing the number of divisors removed on this downleg
            orb.append( leg );
        }

        // Loop until the current integer magnitude is less that the starting point - in other words once the orbit converges
        while ( abs( current_int ) > abs( start ) );
    }

    // Is all the below stuff needed when running in speed mode?  Check wih convergent_eq too
    // Kind of think it's not because it's going to be tossed anyways

    // At a minimum the equivalence factors is the same as the path factors
    ec_factors = path_factors;

    // If non-zero - in other words not the degenerate 0 integer case
    if ( current_int != 0 )
    {
        // Eliminate any residual divisor factors from remainder
        while ( current_int % statics::divisor == 0 )
        {
           current_int /= statics::divisor;
           ec_factors++;                // Counting this produces classic maxfacts
        }

        // Now clean up any divisor factors from the starting integer before finding the next
        current_int = start;
        while ( current_int % statics::divisor == 0 )
        {
           current_int /= statics::divisor;
        }

        // Find the number of divisor factors to get to the next local terminus
        current_int = connection( current_int );
        next_factors = term( current_int );
    }
}

/**
 * @brief Retrieve the path as a std::string object (e.g. 0 1 2 1 2 1 3)
 * @tparam P - The integer data type.
 * @return std::string - Returns the path by calling the orbit::path() function.
 */
template < class P >
std::string t_path< P >::getpath() const
{
    return orb.path();
}

/**
 * @brief Return the equivalence class representation
 * @details The notation used here and the paper is novel.  It begins with a sign indication (+/-) where it is optional for positive
 * intgers.  Next comes a mod 6 intger specifically 0 through 5.  Any digits which follow are binary 0 or 1.  So it's signed hex binary
 * and the number of digits in the flow indicates the level of specificity needed to gaurantee a convergent flow, for example +3011101.
 * 
 * Convergent flows which are initiated with a specified integer starting value begin with the shortest representation which has sufficient
 * specificity to ensure convegence.  As flows progress through 3n+1 connections they converge to less specific (shorter) flows until at
 * last they converge with zero length.
 * 
 * If the starting point is a convergence class itself it may converge before reaching a length of zero because it is more specific than
 * is needed in order to gaurantee that all integers belonging to that equivalence class converge to a smaller integer than even the
 * lowest valued integer in the class, for example +50111.
 * 
 * On the other hand, any given equivalence class may not be sufficently specific (long enough) to guarantee that all members of the
 * class converge identically in which case the convergence patter does not include all connections, for exmaple +301001101.
 * @tparam P - The integer data type.
 * @param [in] digits - Desired nubmer of digits in the representation.  Defaults to -1 which is a signal to use the standard class length.
 * @return std::string - Return the flow representation in signed hex binary notation.
 */
template < class P >
std::string t_path< P >::flow( long digits ) const
{
    if ( digits < 0 )
        digits = ec_len;

    std::string flowrep = "";

    // Initiailize factors by starting with the local value divided by the multiplier
    P factors = start_int / statics::multiplier;

    // Initiailize the remainder by first finding the residual which will become the leading digit
    P remainder = start_int % ( statics::divisor * statics::multiplier );

    // Protect against negative number arguments
    digits = ( digits < 0 ) ? 0 : digits;

    // Prepend a sign iff it is a signed (non-zero) integer and has at least one digit in the representation
    if ( digits && int_sign )
    {
        // If positive
        if ( int_sign > 0 )
            flowrep += "+";
        
        // Otherwise it's negative
        else
            flowrep += "-";
    }

    // While loop that ensures if digits == 0 that it doesn't enter loop
    // Process all required elements in representation of lemgth "digits"
    while ( digits-- )
    {
        P absolute = abs( remainder );

        // The character representation using the absolute value of the remainder
        flowrep += to_str( absolute );

        // Set up for next digits
        factors /= statics::divisor;

        // Get ready for next iteration of do loop
        remainder = factors % statics::divisor;          
    }

    return flowrep;
}

/**
 * @brief Determines if the starting integer has any parent integers and if it does displays the number of ancestors found
 * @details Multiples of 3 have no ancestors so this is the initial check.  Otherwise it perform an exhaustive (non-cheating)
 * check to find all the integers be they smaller or greater that lead to the integer in question.  In reality it appears that
 * all ancestors may be related to each other by factors of 4n+1 (although I haven't yet figured out why) - but this algorith does
 * NOT presume this to be true.  Instead it computes the inverse connection formula to attempt to find the ancestors. 
 * @tparam P - The integer data type.
 * @param [out] scale - The scale is really a convenience for external loops so that the same ancestor is not found repetitively. It is
 * used to multiply the starting integer so that the search resumes beyond where the past ancestor was found.  The scale value is a
 * reference so altough it uses the provided scale as the intial value is does modify it as needed and returns the modified value,
 * @return P - Returns the integer which is a parent integer which leads to the staring integer.  Returns 0 if no parents are possible.
 */
template < class P >
P t_path< P >::ancestry( long &scale ) const
{
    // First check is to see if the number is a multiple of the divisor - there is no parent
    if ( start_int == ( start_int / statics::divisor ) * statics::divisor )
        return 0;

    // Next check is to see if the number is a multiple of the multiplier - there is no parent
    if ( start_int == ( start_int / statics::multiplier ) * statics::multiplier )
        return 0;

    // The starting integers which remain after passing these tests have parents
    long found = 0;
    P parent = 0, last = 0;

    // While loop which terminates as soon as it finds a parent which is NOT a multiple of the multiplier
    // Note this does NOT check for integer rollover for large integers
    do
    {
        parent = scale * start_int * statics::divisor - statics::addend;
        parent /= statics::multiplier;

        P child = connection( parent );
        term( child );

        // if the value found is a valid (non-zero) parent increment the found count and display
        if ( start_int == child )
        {
            break;      // Parent found
        }

        scale++;
    }

    // Loop until you find a parent integer
    while ( true );         // Loop until you find the parent - infinite loops should already be precluded

    // Return the number of ancestors found
    return parent;
}

/**
 * @brief Return the next integer in an orbit
 * @tparam P - The integer data type.
 * @return P - If the value is odd (a local terminus) this return the 3n+1 connection, otherwise for even with one factor of 2 removed.
 */
template < class P >
P t_path< P >::next() const
{
    P next_int = start_int % statics::divisor;          // Initialize to the remainder of mod by divisor

    // Check to see if the value is unevenly divisible by the divisor - in other words it a terminus
    if ( next_int % statics::divisor != 0 )
    {
        // If so then return the next connection value - un-reduced to the terminus
        return connection( start_int );
    }

    // Otherwise simply return the next integer in orbit
    return start_int / statics::divisor;
}


// Inlined const t_path< P > class state retrieval functions

/**
 * @brief Return the starting integer
 * @tparam P - The integer data type.
 * @return P - The staring integer.
 */
template < class P >
P t_path< P >::start() const
{
    return start_int;
}

/**
 * @brief The maximum integer visited during a convergent segment.
 * @details  The setpath() menber function determines the maximum integer value during initialization process.
 * @tparam P - The integer data type.
 * @return P - The maximum integer in the convergence segment.
 */
template < class P >
P t_path< P >::max() const
{
    return max_int;
}

/**
 * @brief Returns the orbit.
 * @tparam P - The integer data type.
 * @return const orbit_t& - Return a const reference to the orbig object.
 */
template < class P >
const orbit_t& t_path< P >::orbit()
{
    return orb;
}

/**
 * @brief Return the sign of the integer
 * @tparam P - The integer data type.
 * @return int - Sign of the integer.
 */
template < class P >
int t_path< P >::sign() const
{
    return int_sign;
}

/**
 * @brief Error code
 * @tparam P - The integer data type.
 * @return int - Error code.  Really only relevant when using standard precision integers which may exceed the internal limits.  Errors
 * are currently not applicable to multiple precision integers when using the GNU MP libraries.
 */
template < class P >
int t_path< P >::error() const
{
    return error_mask | orb.error();
}

/**
 * @brief Return the number if downlegs in the orbit.
 * @tparam P - The integer data type.
 * @return long - The number of downlegs (3n+1 connections) in the convergent orbit.
 */
template < class P >
long t_path< P >::pathLength() const
{
    return orb.path_len();
}

/**
 * @brief The length of the equivalence class
 * @details This value is either computed automatically when an integer is provided as the starting point or as passed as an initial
 * value when define a class length.  For example the equivalence class for 23 is +51100, thus classLength() is 5.
 * @tparam P - The integer data type.
 * @return long - The number of digits in the equivalence class representation.
 */
template < class P >
long t_path< P >::classLength() const
{
    return ec_len;
}

/**
 * @brief The aggregate number of factors of 2 in the orbit
 * @details This is the sum of the orbit path, for example the path for 15 is 0 1 1 1 4, thus pathFactors() is 7
 * @tparam P - The integer data type.
 * @return long - Total factors of 2 from the entire convergent orbit
 */
template < class P >
long t_path< P >::pathFactors() const
{
    return path_factors;
}

/**
 * @brief The number of factors of two common to the entire equivalence class for convergence.
 * @details Equal to pathFactors() plus any residual factors of 2 post convergence.  The pathFactors() gives the number of factors of 2
 * which result in convergence - but the converged (lower) integer may be even and thus further divisible by 2.  Thus classFactors()
 * instead finds all of factors of 2 until it reaches the next local terminus which is an odd value.
 * @tparam P - The integer data type.
 * @return long - The total number of factors of 2 for the equivalence class to get to the convergent local terminus.
 */
template < class P >
long t_path< P >::classFactors() const
{
    return ec_factors;
}

/**
 * @brief Returns the number of factors of two after reaching the next local terminus in the orbit
 * @details This function is really just to determine how to output the convergent pathways.  If nextFactors() returns a 1, then it is a
 * divergent result which increases the indenting of the class by 1 to stand out visually.  Convergence happens whenever there are 2 or
 * more factors of 2 following the next 3n+1 connection and this reduces the indenting of output by the value returned minus 1.
 * 
 * The length of the next equivalence class from the odd local terminus decreases by the result of nextFactors().  Even numbers however
 * leave the length unchanged.  Thus 7 has a class length of 7 but since the nextFactors() is 1 both 22/11 have length of 6.  The next
 * value is also 1 so 34/17 has class length of 5.  But 52/26/13 is convergent (2 factors of two) and thus the class length is now 3.
 * 
 * As shown below integer 7 experiences two divergent connections before converging once it reach 52.  So the indenting of the
 * equivalence class representation increases for 22/11 and 34/17 and decreases (by 1) for 52/26/13.  Then 13 connects to 40 which has
 * 3 factors of 2 and thus the indenting decreases by 2 - and the equivlance class now has length 0 and disappears entirely.
 * 
 * The last digit in the brackets provides the length of the equivalence class.  This represents the exponent of 2 which when mulitplied
 * by three is the difference between consecutive values of members of the equivalence class.
 * 
 * @code {.C}
 *  7: (05,3*2^007): +1100000
 * 22: (01,3*2^006):  +411000
 * 11: (04,3*2^006):  +510000
 * 34: (01,3*2^005):   +41010
 * 17: (02,3*2^005):   +50100
 * 52: (01,3*2^003):  +400
 * 26: (01,3*2^003):  +200
 * 13: (02,3*2^003):  +101
 * 40: (01,3*2^000): 
 * 20: (01,3*2^000): 
 * 10: (01,3*2^000): 
 *  5: (02,3*2^000): 
 * @endcode
 * 
 * @tparam P - The integer data type.
 * @return long - The number of factors of two following the next 3n+1 connection.
 */
template < class P >
long t_path< P >::nextFactors() const
{
    return next_factors;
}


// Path operators

/**
 * @brief Assignment operator
 * @details Replicates the path object by extracting the starting integer and regenerating.
 * @tparam P - The integer data type.
 * @param [in] rp - Const reference to the path object to replicate.
 * @return t_path< P >& - Returns a reference to this object
 */
template < class P >
t_path< P > &t_path< P >::operator = ( const t_path< P > &rp )
{
    return operator= ( rp.start() );
}

/**
 * @brief Assignement operator.  Used also in constructors
 * @details Replicates the path object by extracting the starting integer and regenerating.
 * @tparam P - The integer data type.
 * @param [in] rp - Const reference to the integer object of type P.
 * @return t_path< P >& - Returns a reference to this object
 */
template < class P >
t_path< P > &t_path< P >::operator = ( const P &rp )
{
    setpath( rp );
    set_ec( rp );

    return *this;
}

/**
 * @brief Equivalency check
 * @details Returns the orbit compare results.  Not this does \b not compare the starting integer values.
 * @tparam P - The integer data type.
 * @param [in] rp - Const reference to the path object to replicate.
 * @return true - The paths are equivalent
 * @return false - The paths are different
 */
template < class P >
bool t_path< P >::operator == ( const t_path< P > &rp ) const
{
    // If the path lengths are unequal then its a no-brainer that they are not equal
    if ( orb.path_len() != rp.orb.path_len() )
        return false;
    
    // Otherwise do a more rigourous comparison
    else
        return orb == rp.orb;
}

/**
 * @brief Inequivalency check
 * @details Returns the orbit compare results.  Not this does \b not compare the starting integer values.
 * @tparam P - The integer data type.
 * @param [in] rp - Const reference to the path object to replicate.
 * @return true - The paths are different
 * @return false - The paths are equivalent
 */
template < class P >
bool t_path< P >::operator != ( const t_path< P > &rp ) const
{
    return !( orb == rp.orb );
}

/**
 * @brief Ordinal less than comparison
 * @details Returns the orbit less than comparison results.  Less than means it's integer union representation is less.
 * @tparam P - The integer data type.
 * @param [in] rp - Const reference to the path object to compare to.
 * @return true - The path is less than the argument orbit
 * @return false - The path is greater than or equal to the argument orbit
 */
template < class P >
bool t_path< P >::operator < ( const t_path< P > &rp ) const
{
    // Compare the orbits
    return orb < rp.orb;
}

/**
 * @brief Ordinal greater than comparison
 * @details Returns the orbit less than comparison results.  Greater than means it's integer union representation is greater.
 * @tparam P - The integer data type.
 * @param [in] rp - Const reference to the path object to compare to.
 * @return true - The path is greater than the argument orbit
 * @return false - The path is less than or equal to the argument orbit
 */
template < class P >
bool t_path< P >::operator > ( const t_path< P > &rp ) const
{
    // Compare the orbits
    return orb > rp.orb;
}

/**
 * @brief Ordinal less than or equal to comparison
 * @details Returns the orbit less than comparison results.  False means it's integer union representation is greater.
 * @tparam P - The integer data type.
 * @param [in] rp - Const reference to the path object to compare to.
 * @return true - The path is less than or equal to the argument orbit
 * @return false - The path is greater than the argument orbit
 */
template < class P >
bool t_path< P >::operator <= ( const t_path< P > &rp ) const
{
    // Compare the orbits
    return !( orb > rp.orb );
}

/**
 * @brief Ordinal greater than or equal to comparison
 * @details Returns the orbit less than comparison results.  False means it's integer union representation is less.
 * @tparam P - The integer data type.
 * @param [in] rp - Const reference to the path object to compare to.
 * @return true - The path is greater than or equal to the argument orbit
 * @return false - The path is less than the argument orbit
 */
template < class P >
bool t_path< P >::operator >= ( const t_path< P > &rp ) const
{
    // Compare the orbits
    return !( orb < rp.orb );
}

/**
 * @brief Print out the equivalence class in its nominal form.  Neither truncated not extended.
 * @details The number of digits in the representation is given by pathLength().
 * @tparam P - The integer data type.
 */
template < class P >
void t_path< P >::prettyPrint() const
{
    prettyPrint( ec_len, 0 );
}

/**
 * @brief Print out the equivalence class with first column width (the generating integer)
 * @details The first column width is defined by the argument max_digits which is the number of digits in the \b largest integer
 * in this convergent segment.  This number of digits is used to right justify the first column the output so it is easier to read.
 * @tparam P - The integer data type.
 * @param [in] max_digits - The maximum number of base 10 digits in the first column (used for right justification).
 */
template < class P >
void t_path< P >::prettyPrint( int max_digits ) const
{
    prettyPrint( ec_len, max_digits );
}

/**
 * @brief Print out the equivalence class with first column width (the generating integer) equal to max_digits
 * @details The first column width is defined by the argument max_digits which is the number of digits in the \b largest integer
 * in this convergent segment.  This number of digits is used to right justify the first column the output so it is easier to read.
 * @tparam P - The integer data type.
 * @param [in] len - Length of the convergence class for the convergent flow segment.
 * @param [in] max_digits - The maximum number of base 10 digits in the first column (used for right justification).
 */
template < class P >
void t_path< P >::prettyPrint( long len, int max_digits ) const
{
    pathPrint( start_int, pathLength(), ( len < 0 ) ? 0 : len, 0, flow(len), max_digits );
}

/**
 * @brief Print out the equivalence class with first column width (the integer) equal to max_digits and fancy indenting
 * @details The first column width is defined by the argument max_digits which is the number of digits in the \b largest integer
 * in this convergent segment.  This number of digits is used to right justify the first column the output so it is easier to read.
 * @tparam P - The integer data type.
 * @param [in] len - Length of the convergence class for the convergent flow segment.
 * @param [in] indent - Indents the equivalnce class representation so convergent and divergent flows are visible.
 * @param [in] max_digits - The maximum number of base 10 digits in the first column (used for right justification).
 */
template < class P >
void t_path< P >::prettyPrint( long len, long indent, int max_digits ) const
{
    pathPrint( start_int, pathLength(), ( len < 0 ) ? 0 : len, ( indent < 0 ) ? 0 : indent, flow(len), max_digits );
}

/**
 * @brief Print out the equivalence class whose length is limited to the \b total number of factors of 2
 * @tparam P - The integer data type.
 */
template < class P >
void t_path< P >::prettyPrintClass() const
{
    pathPrint( start_int, pathLength(), pathFactors(), 0, flow(pathFactors()), 0 );
}

/**
 * @brief Print out the equivalence class whose length is limited to the \b total number of factors of 2 required to converge (min 1)
 * @details The first column width is defined by the argument max_digits which is the number of digits in the \b largest integer
 * in this convergent segment.  This number of digits is used to right justify the first column the output so it is easier to read.
 * @tparam P - The integer data type.
 * @param [in] max_digits - The maximum number of base 10 digits in the first column (used for right justification).
 */
template < class P >
void t_path< P >::prettyPrintClass( int max_digits ) const
{
    pathPrint( start_int, pathLength(), pathFactors(), 0, flow(pathFactors()), max_digits );
}

/**
 * @brief Print out the convergent orbit
 * @details This format shows in detail the factors of 2 at each step in the convergent orbit.  The \b total number of factors of 2
 * in the entire sequence is classFactors().
 * @tparam P - The integer data type.
 */
template < class P >
void t_path< P >::prettyPrintPath() const
{
    pathPrint( start_int, pathLength(), path_factors, 0, orb.path(), 0 );
}

/**
 * @brief Print out the convergent orbit
 * @details This format shows in detail the factors of 2 at each step in the convergent orbit.  The \b total number of factors of 2
 * in the entire sequence is classFactors().
 * 
 * The first column width is defined by the argument max_digits which is the number of digits in the \b largest integer
 * in this convergent segment.  This number of digits is used to right justify the first column the output so it is easier to read.
 * @tparam P - The integer data type.
 * @param [in] max_digits - The maximum number of base 10 digits in the first column (used for right justification).
 */
template < class P >
void t_path< P >::prettyPrintPath( int max_digits ) const
{
    pathPrint( start_int, pathLength(), path_factors, 0, orb.path(), max_digits );
}


// Protected t_path< P > member class functions

/**
 * @brief Calculate the 3n+1 Collatz Connection.  Quite simply, this is the center of all of this.
 * 
 * @tparam P - The integer data type.
 * @param [in] terminus - The (odd) positive integer from which to need to compute the (even) connection.
 * @return P - Return the (even) 3n+1 connection from the (odd) local terminus.
 */
template < class P >
P t_path< P >::connection( const P &terminus ) const
{
    return terminus * statics::multiplier + statics::addend;
}

// This should probably be reimplemented so that is properly handles multiple precision to handle large EC
/**
 * @brief Parse the equivalence class string representation
 * @details The program accept equivalence class input string and this parses them to find the leading integer of the class.  The class
 * can carry an option sign indicator - the lack of which is taken to mean positive.
 * @tparam P - The integer data type.
 * @param [in] input - The std::string object holding the character string representation.
 * @return P - Returns the integer value computed - or 0 if there was an erro during parsing.
 */
template < class P >
P t_path< P >::parse( const std::string &input ) const
{
    long pos = 0;                   // Position in the string
    long eq_sign = 1;               // Presume positive equivalence class
    long strlen = input.length();   // Length of the string - including any optional sign indicator
    char ch = input[pos++];         // Grab the first character

    // The first character of the equivalence class can optionally be a sign indicator
    // But how to handle an actual 0 ???
    if ( is_signed( input ) )
    {
        // If it's negative change the sign from the default
        if ( ch == '-' )
        {
            eq_sign = -1;
        }

        // Grab the next characters in string
        ch = input[pos++];

        // The sign consumes one spot in the representation
        strlen--;
    }

    // Make sure the first character is '0' through '5'
    // Note that this kind of limits this to the standard 3n + 1 and could be more generic with effort
    if ( ch < '0' || ch > '5' )
        return 0;

    // Otherwise the first character is indeed valid so "convert" to character to integer
    P local = ch - '0';

    P multiplier = statics::multiplier * statics::divisor;
    bool rollover = false;

    // Consume all digits in the equivalence class representation
    while ( --strlen > 0 )
    {
        // First check for integer rollover which really only matters if you need to actually add it
        if ( abs( multiplier * 2 ) < multiplier )
            rollover = true;

        ch = input[pos++];

        // Only non-zero partition components contribute to terminus
        if (ch == '1')
        {
            // Okay well now it matters - you actually have to add in a value which causes integer rollover
            if ( rollover )
                return 0;

            // Otherwise it's safe
            else
                local += multiplier;
        }
        // Error condition terminates parsing
        else if (ch != '0')
        {
            return 0;
        }

        // Double the multiplier for the more significant digit
        multiplier *= 2;
    }
    
    // Does this multiplication handle to zero case?
    return local * eq_sign;
}

/**
 * @brief Compute and return the number of factors of the divisor (2), and return the argument with those factors removed
 * 
 * @tparam P - The integer data type.
 * @param [out] i - The number from which to extract all factors of two and which will return with those factors removed
 * @return long - The number of factors of of the divisor (2) extracted from the argument.
 */
template < class P >
long t_path< P >::term( P &i ) const
{
    long facts = 0;

    // Carefully avoid the special case where i=0 to sidestep an infinite loop
    if ( i != 0 )
    {
        // Loop until all factors are removed and return the number of them
        while ( i % statics::divisor == 0 )
        {
            i /= statics::divisor;       // Divide but the divisor
            facts++;
        }
    }

    return facts;
}

/**
 * @brief Return the factors of the divisor (2) from a branch point - but stop if you converge
 * 
 * @tparam P - The integer data type.
 * @param [out] branch - Factor out divisors, but stop if you find convergence.
 * @param [in] start - The integer starting point.  If even the factorization results in something smaller in magnitude, stop.
 * @return long - The number of divisor factors removed.
 */
template < class P >
long t_path< P >::factor( P &branch, const P &start ) const
{
    long facts = 0;         // Divisor factor counter

    // Loop until you're eaten up all the factors of the divisor
    while ( branch % statics::divisor == 0 )
    {
        // Divide the divisor out - note that because this argument is by reference the variable in the caller changes too
        branch /= statics::divisor;
        facts++;

        // Exit early if you converge
        if ( abs( branch )  < abs( start ) )
        {
            break;
        }
    }

    return facts;
}

/**
 * @brief Determine the minimum number of digits in the equivalence flow representation
 * @details The process finds the minimum length equivalence class for a given integer
 * @tparam P - The integer data type.
 * @param [in] start - The starting integer.  Segment convergence happens when the current integer in the sequence drops in 
 * magnitutde below this point. 
 * @return long - Return the minimum convergence class representation length.
 */
template < class P >
long t_path< P >::set_ec( const P &start )
{
    // Compute the initial residual
    P residual = start / statics::multiplier;

    // Always at least length one
    ec_len=1;

    // Loop until you hit the global terminus
    while ( abs( residual ) > 1 )
    {
        residual /= statics::divisor;
        ec_len++;
    }

    return ec_len;
}

/**
 * @brief Simple function which sets the length of an equivalnce class to the string lenght minus any polarity indications
 * 
 * @tparam P - The integer data type.
 * @param [in] input - The input string representation of an equivalence class.
 * @return long - The length of the equivalence class excluding any sign (polarity) indications.
 */
template < class P >
long t_path< P >::get_ec_len( const std::string &input ) const
{
    long len = input.length();

    // If the string representation is signed reduce the equivalence class representation by one
    if ( is_signed( input ) )
        len--;

    return len;
}

/**
 * @brief Indicates if an equivalenc class string includes a sign indication
 * 
 * @tparam P - The integer data type.
 * @param [in] input - Const reference to the input string.
 * @return true - The string includes a sign indication.
 * @return false - The string does not carry a sign indication.  This is taken to be positive.
 */
template < class P >
bool t_path< P >::is_signed( const std::string &input ) const
{
    char ch = input[0];         // Grab the first character
    return ( ch == '+' || ch == '-' );
}

/**
 * @brief Initialize the class.  If there is any existing state it is first cleared.
 * 
 * @tparam P - The integer data type.
 * @param [in] start - The new starting integer for the path object.
 */
template < class P >
void t_path< P >::init( P start )
{
    // Clear current state and free memory if needed
    zeroize();

    // Initialize to the new starting value
    start_int = start;
    int_sign = sgn( start );
}

/**
 * @brief Quite literally sets everything in the class object to 0.
 * 
 * @tparam P - The integer data type.
 */
template < class P >
void t_path< P >::zeroize()
{
    // Clear state of integer member variables
    start_int = max_int = int_sign = path_factors = ec_factors = next_factors = ec_len = error_mask = 0;

    // Clear state of orbit member
    orb.clear();
}


// Implementation specific int64_t functions in support of path template instantiation

void pathPrint( const int64_t &start, long length, long factors, int indent, std::string flow, int max_digits )
{
    printf( "%*" PRId64 ": (%02ld,%d*2^%03ld):%*c%s\n",
            max_digits, start, length, statics::multiplier, factors, indent, ' ', flow.c_str() );
}

std::string to_str( const int64_t &remainder )
{
    return std::to_string( remainder );
}


#ifdef gnu_mp

// Implementation specific mpz_class functions in support of mp_path template instantiation

void pathPrint( const mpz_class &start, long length, long factors, int indent, std::string flow, int max_digits )
{
    // The GNU multiple precision implementation extends the printf() functionality
    gmp_printf( "%*Zd: (%02ld,%ld*2^%03ld):%*c%s\n",
            max_digits, start.get_mpz_t(), length, statics::multiplier, factors, indent, ' ', flow.c_str() );
}

std::string to_str( const mpz_class &remainder )
{
    return remainder.get_str();
}

#endif