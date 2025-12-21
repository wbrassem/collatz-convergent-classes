/**
 * @file btree.hpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief As the name implies the btree and node classes and templates implement a binary tree.  This structure is used
 * to efficiently search and store Collatz related objects like convergent pathways.
 * @version 1.1
 * @date 2025-12-19
 * 
 * @copyright Copyright (c) 2023-2025 Wayne Brassem
 */

#pragma once
#include "common.hpp"

/**
 * @brief The node struct definition for use in btree implementation
 * @details All member functions and attributes are public, but access is protected within btree.
 */
struct node
{
    public:
        node();                                         // Default constructor

        long key_value;                                 /**< Long integer node key. */
        ulong count;                                    /**< Value used to provide ordinal instance counts. */
        node *left;                                     /**< Pointer to the left subtree. */
        node *right;                                    /**< Pointer to the right subtree. */
};

/**
 * @brief The btree class definition for use in storing convergent paths.
 * @details This is a generic binary tree implementation, but it's primary aim is for storing convergent Collatz paths.
 */
class btree
{
    public:
        btree();                                        // Default constructor
        btree( const btree &tree );                     // Copy constructor
        ~btree();                                       // Destructor
 
        // Basic node insertion and search member functions
        void insert( const long key );                  // Public insert function
        long search( long key ) const;                  // Returns the count for a given node key

        // const Iterators take an optional function pointer which passed in the key and count values as long
        long constForwardIterator( void (*func)( long key, long count ) = nullptr ) const;
        long constReverseIterator( void (*func)( long key, long count ) = nullptr ) const;

        // Assignment operator
        btree& operator=( const btree &tree );          // Assignment operator

        long nodes() const;                             // Return number of nodes in btree
        void destroy_tree();                            // Destroys tree and free memory

    protected:
        // Insert a node or increment existing one
        void insert( long key, node *leaf );

        // Search for a node and return pointer, or nullptr if not found
        node *search( long key, node *leaf ) const;

        // The traverse function iterates the tree in forward or reverse order and optionally calls a function per node
        long traverse( node *leaf, long &sum, void (*func)( long key, long count ), bool forward ) const;

        node *duplicate( node *nptr );                  // Clone a tree and subtree given a starting point
 
        void destroy_tree( node *leaf );                // Destroy the tree and subtree given a starting point
        void zeroize();

        node *root;                                     /**< Pointer to the root node or nullptr if empty tree. */
        ulong node_count;                               /**< Counter which keeps the total number of nodes in tree. */
};


/**
 * @brief The templated t_node< K > struct definition for use in t_btree< class K > implementation
 * @details All member functions and attributes are public, but access is protected within t_btree.  Note that
 * comparison operators which are used for searching and placement must be defined if K is a user-defined class.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 */
template < typename K > 
struct t_node
{
    public:
        t_node();                                       // Default constructor

        K       key_value;                              /**< Key of type K holding the key_value. */
        ulong   count;                                  /**< Value used to provide ordinal instance counts. */
        t_node  *left;                                  /**< Pointer to the left subtree. */
        t_node  *right;                                 /**< Pointer to the right subtree. */
};

/**
 * @brief Construct a new templated t_node< K > object for use in a t_btree< K > binary tree
 * @details Set all initial values to 0 and pointers to subtended nodes to nullptr.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * The key_value member is default initialized.
 */
template < class K >
t_node< K >::t_node()
{
    count       = 0;
    left        = nullptr;
    right       = nullptr;
}


/**
 * @brief The templated t_btree< K > class definition for use in storing convergent paths of element type K.
 * @details This is a generic binary tree implementation, but it's primary aim is for storing convergent Collatz paths.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 */
template < class K > 
class t_btree
{
    public:
        t_btree();
        t_btree( const t_btree< K > &tree );            // Copy constructor
        ~t_btree();
 
        void insert( const K &key );
        long search( const K &key ) const;

        // const Iterators take an optional function pointer which return copies of the key and count values
        inline long constForwardIterator( void (*func)( const K &key, long count ) = nullptr ) const;
        inline long constReverseIterator( void (*func)( const K &key, long count ) = nullptr ) const;

        // Block default shallow copy assignment operator
        t_btree< K >& operator=( const t_btree< K > &tree );

        long nodes() const;                             // Return number of nodes in btree
        void destroy_tree();                            // Destroys tree and free memory

    protected:
        // Insert a node or increment existing one
        void insert( const K &key, t_node< K > *leaf);

        // Search for a node and return pointer, or nullptr if not found
        t_node< K > *search( const K &key, t_node< K > *leaf) const;

        // The traverse function iterates the tree in forward or reverse order and optionally calls a function per node
        long traverse( t_node< K > *leaf, long &sum, void (*func)( const K &key, long count ), bool forward ) const;

        t_node< K > *duplicate( t_node< K > *nptr );    // Clone a tree and subtree given a starting point

        void destroy_tree( t_node< K > *leaf );         // Destroy the tree and subtree given a starting point
        void zeroize();

        t_node< K > *root;                              /**< Pointer to the root node or nullptr if empty tree. */
        ulong node_count;                               /**< Counter which keeps the total number of nodes in tree. */
};

// Template definitions
//
// These are definitions are needed in the header file since the compiler needs to have the template code at compile time.
//
// The key K requires support for comparison operators if K is a user-defined class.

// btree< K > public member functions

/**
 * @brief Default constructor for a new binary t_btree< K > object
 * @details Set the node count to 0 and root node pointer to nullptr.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 */
template < class K >
t_btree< K >::t_btree()
{
    zeroize();
}

/**
 * @brief Copy constructor for creating a new binary t_btree< K > object from another
 * @details Creates a clone of a binary tree allocating new memory for copy.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] tree - Constant reference to a t_btree< K > tree to be cloned.
 */
template < class K >
t_btree< K >::t_btree( const t_btree< K > &tree)
{
    operator=( tree );
}

/**
 * @brief Destructor for the binary t_btree< K > object
 * @details Recursively destroys subtrees and frees memory using the destroy_tree() function.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 */
template < class K >
t_btree< K >::~t_btree()
{
    destroy_tree();
}

/**
 * @brief Public insert function to add a t_node< K > given a key
 * @details Function first searches for the existence of a given node and inserts if not found, or increments count if found.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] key - The node key of type K to add or count (frequency) to increment if found.
 */
template < class K >
void t_btree< K >::insert( const K &key )
{
    // If the tree exists (root is not null) then find where to insert the node
    if ( root != nullptr )
        insert( key, root );

    // Otherwise it's the start of a new tree and set the root node counter to 1
    else
    {
        root = new t_node< K >;
        root->key_value = key;

        root->count = 1;
        node_count = 1;
    } 
}

/**
 * @brief Public search function which looks for key entry of type K
 * @details Searches the t_btree< K > object for a t_node< K > key and returns the count if found, or 0 if not found.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] key - The t_node< K > key of type K to search for.
 * @return long - Return the count (frequency) of the node.
 */
template < class K >
long t_btree< K >::search( const K &key ) const
{
    t_node< K > *n_ptr = search( key, root );

    // If the node was found return the count
    if ( n_ptr )
        return n_ptr->count;

    // Otherwise return zero indicating key not found
    else
        return 0;
}

/**
 * @brief Iterates over the binary tree in forward sort order
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] func - Optional templated function pointer accepting reference to const node key of type K
 * and count values which can be used for node handling during forward iteration.
 * @return long - Returns the sum of counts of all nodes in the binary tree.
 */
template < class K >
long t_btree< K >::constForwardIterator( void (*func)( const K &key, long count )  ) const
{
    // Call the protected traverse function by passing the root node
    long sum = 0;
    return traverse( root, sum, func, true );
}

/**
 * @brief Iterates over the binary tree in reverse sort order
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] func - Optional templated function pointer accepting reference to const node key of type K
 * and count values which can be used for node handling during reverse iteration.
 * @return long - Returns the sum of counts of all nodes in the binary tree.
 */
template < class K >
long t_btree< K >::constReverseIterator( void (*func)( const K &key, long count )  ) const
{
    // Call the protected traverse function by passing the root node
    long sum = 0;
    return traverse( root, sum, func, false );
}

/**
 * @brief Assigns (copies) one tree to another
 * @details Destroys any existing t_btree< K > and creates a clone of a binary tree allocating new memory for copy.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] tree - Constant reference to a t_btree< K > to be cloned.
 * @return btree& - Returns a reference to the cloned tree.
 */
template < class K >
t_btree< K >& t_btree< K >::operator=( const t_btree< K > &tree )
{
    // Protect against self-assignment
    if (this == &tree)
        return *this;

    // First clear any existing state
    destroy_tree();

    // If there is a tree duplicate it
    if ( tree.root != nullptr )
    {
        root = duplicate( tree.root );
        node_count = tree.node_count;
    }

    return *this;
}

/**
 * @brief Fnction which returns the total number of nodes in the t_btree< K >
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @return long - Return the number of nodes in the binary tree
 */
template < class K >
long t_btree< K >::nodes() const
{
    return node_count;
}

/**
 * @brief Destroys the tree and frees up memory, then zeroizes the t_btree< K > to initial state
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 */
template < class K >
void t_btree< K >::destroy_tree()
{
    destroy_tree(root);
    zeroize();
}


// btree< K > protected member functions

/**
 * @brief Protected insert function which inserts node if non-existent, or increments counter if found
 * @details The protected insert begins with the root t_node< K > which is passed in as the starting point by the public
 * insert function.  The function first tries to locate the t_node< K > and inserts it (in order) if it is not found.  If the
 * node is found then the count (frequency) of the t_node< K > is incremented.  The function is recursive until it is known
 * that the node does not exist in the tree in which case it is added with an initial count of 1.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] key - The const key of type K to insert if not found, or the count to increment if found
 * @param [in] leaf - The current t_node< K > being searched.
 */
template < class K >
void t_btree< K >::insert( const K &key, t_node< K > *leaf )
{
    // If the key is found increment the frequency
    if ( key == leaf->key_value )
        leaf->count++;

    // If the key is greater than the current one
    else if ( key > leaf->key_value )
    {
        // If the right path is not null continue search there
        if ( leaf->right != nullptr )
            insert( key, leaf->right );

        // Otherwise insert the new key here and initialize the reference count to 1
        else
        {
            leaf->right = new t_node< K >;
            leaf->right->key_value = key;

            leaf->right->count = 1;
            node_count++;                        // Increment the node count
        }
    }

    // Otherwise if the key is less than the current one
    else
    {
        // If the left path is not null continue search there
        if ( leaf->left != nullptr )
            insert( key, leaf->left );

        // Otherwise insert the new key here and initialize the reference count to 1
        else
        {
            leaf->left = new t_node< K >;
            leaf->left->key_value = key;

            leaf->left->count = 1;
            node_count++;                        // Increment the node count
        }  
    }
}

/**
 * @brief Protected search function which return a pointer to the t_node< K > if found, or nullptr if not
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] key - The key of type K to search for in the tree.
 * @param [in] leaf - The current node being searched.
 * @return node* - Pointer to the located t_node< K > if found, or nullptr if not.
 */
template < class K  >
t_node< K > *t_btree< K >::search( const K &key, t_node< K > *leaf ) const
{
    // If the current node is not null then examine it
    if ( leaf != nullptr )
    {
        // If you have a match return a pointer to the node
        if ( key == leaf->key_value )
            return leaf;

        // If the search key is less than the current search the left subtree
        if ( key < leaf->key_value )
            return search ( key, leaf->left );

        // If the search key is greater than the current search the right subtree
        else
            return search ( key, leaf->right );
    }

    // Otherwise the search for the key failed and return a null pointer
    else
        return nullptr;
}

/**
 * @brief The traverse function iterates over the nodes in forward or reverse directions
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] node - The current t_node< K > being traversed
 * @param [in] sum - The cumulative sum of counts for this subtree
 * @param [in] func - Optional function which can be used for custom t_node< K > processing during traversal
 * @param [in] forward - Boolean value indicating if forward (true) or reverse (false) traversal is required
 * @return long - Returns the total number of counts from all nodes in the tree
 */
template < class K >
long t_btree< K >::traverse( t_node< K > *node, long &sum, void (*func)( const K &key, long count ), bool forward ) const
{
    // The forward argument controls the direction of traversal and defaults to true meaning in forward sort order

    // Anything in the subtree at all?
    if ( node != nullptr ) 
    {
        // Traverse the left or right subtree depending on the forward direction setting
        if ( forward )
            traverse( node->left, sum, func, forward );
        else
            traverse( node->right, sum, func, forward );

        // If there is a callback function during traverse call it
        if ( func )
        {
            (*func)( node->key_value, node->count );
        }

        // Add the node count to the sum
        sum += node->count;

        // Traverse the right or left subtree depending on the forward direction setting
        if ( forward )
            traverse( node->right, sum, func, forward );
        else
            traverse( node->left, sum, func, forward );
    }

    return sum;
}

/**
 * @brief Duplicates a node and subtending nodes
 * @details This function can be used to duplicate any subtree of a binary tree.  If called using the root t_node< K > as
 * the starting point it will replicate the entire t_btree< K >.  New memory is allocated for the node and then values are
 * copied to the new node.  It then recursively calls for replication of the left and right subtrees.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] nptr - The current t_node< K > (and subtree) being duplicated.
 * @return node* - Return a pointer to the t_node< K > (and subtree) just duplicated.
 */
template < class K >
t_node< K >* t_btree< K >::duplicate( t_node< K > *nptr )
{
    // Check for leaf nodes
    if ( nptr == nullptr )
        return nullptr;

    // Create a new node and copy the contents
    t_node< K > *node_copy = new t_node< K >;
    node_copy->count = nptr->count;
    node_copy->key_value = nptr->key_value;

    // Now copy the left and right subtrees
    node_copy->left = duplicate( nptr->left );
    node_copy->right = duplicate( nptr->right );

    return node_copy;
}

/**
 * @brief Destroys current node and subtending nodes
 * @details This function can be used to destroy any subtree of a binary tree.  If called using the root t_node< K > as
 * the starting point it will destroy the entire t_btree< K >.  The function recursively calls for the destruction of the
 * left and right subtrees and upon return frees the current node.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 * @param [in] leaf - The current node (and subtree) freed.
 */
template < class K >
void t_btree< K >::destroy_tree( t_node< K > *leaf )
{
    // If the current node is not null then destruction is required
    if ( leaf != nullptr )
    {
        // Prior to destroying the current node recursively destroy left and right subtrees
        destroy_tree( leaf->left );
        destroy_tree( leaf->right );

        // Once subtrees are destroyed free up the memory allocated for the current node
        delete leaf;
    }
}

/**
 * @brief Initializes the templated class by setting the root node to nullptr and node count to 0.
 * @tparam K - Ordinal type K - must support <, > and == comparison operations.
 */
template < class K >
void t_btree< K >::zeroize()
{
    root = nullptr;
    node_count = 0;
}
