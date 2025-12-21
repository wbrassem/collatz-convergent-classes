/**
 * @file btree.cpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief Implementation of binary tree and nodes.  Used for storing Collatz convergent paths.
 * @version 1.1
 * @date 2025-12-19
 * 
 * @copyright Copyright (c) 2023-2025 Wayne Brassem
 */

#include "common.hpp"
#include "btree.hpp"

/**
 * @brief Construct a new node object for use in a binary tree
 * @details Set all initial values to 0 and pointers to subtended nodes to nullptr.
 */
node::node()
{
    key_value   = 0;
    count       = 0;
    left        = nullptr;
    right       = nullptr;
}


// btree public member functions

/**
 * @brief Default constructor for a new binary tree object
 * @details Set the node count to 0 and root node pointer to nullptr.
 */
btree::btree()
{
    zeroize();
}

/**
 * @brief Copy constructor for creating a new binary tree object from another
 * @details Creates a clone of a binary tree allocating new memory for copy.
 * @param [in] tree - Constant reference to a tree to be cloned.
 */
btree::btree( const btree &tree )
{
    operator=( tree );
}

/**
 * @brief Destructor for the binary tree object
 * @details Recursively destroys subtrees and frees memory using the destroy_tree() function.
 */
btree::~btree()
{
    destroy_tree();
}

/**
 * @brief Public insert function to add a node given a key
 * @details Function first searches for the existence of a given node and inserts if not found, or increments count if found.
 * @param [in] key - The node key (long integer) to add or count (frequency) to increment if found.
 */
void btree::insert( const long key )
{
    // If the tree exists (root is not null) then find where to insert the node
    if ( root != nullptr )
        insert( key, root );
 
    // Otherwise it's the start of a new tree and set the root node counter to 1
    else
    {
        root = new node;
        root->key_value = key;

        root->count = 1;
        node_count = 1;
    } 
}

/**
 * @brief Public search function which looks for key entry
 * @details Searches the btree object for a node key and returns the count if found, or 0 if not found.
 * @param [in] key - The node key (long integer) to search for.
 * @return long - Return the count (frequency) of the node.
 */
long btree::search( long key ) const
{
    node *n_ptr = search( key, root );

    // If the node was found return the count
    if ( n_ptr )
        return n_ptr->count;

    // Otherwise return zero indicating no key found
    else
        return 0;
}

/**
 * @brief Iterates over the binary tree in forward sort order
 * @param [in] func - Optional function pointer accepting long node key
 * and count values which can be used for node handling during forward iteration.
 * @return long - Returns the sum of counts of all nodes in the binary tree.
 */
long btree::constForwardIterator( void (*func)( long key, long count )  ) const
{
    // Call the protected traverse function by passing in the root node as a starting point
    long sum = 0;
    return traverse( root, sum, func, true );
}

/**
 * @brief Iterates over the binary tree in reverse sort order
 * @param [in] func - Optional function pointer accepting long node key
 * and count values which can be used for node handling during reverse iteration.
 * @return long - Returns the sum of counts of all nodes in the binary tree.
 */
long btree::constReverseIterator( void (*func)( long key, long count )  ) const
{
    // Call the protected traverse function by passing in the root node as a starting point
    long sum = 0;
    return traverse( root, sum, func, false );
}

/**
 * @brief Assigns (copies) one tree to another
 * @details Destroys any existing tree and creates a clone of a binary tree allocating new memory for copy.
 * @param [in] tree - Constant reference to a btree to be cloned.
 * @return btree& - Returns a reference to the cloned tree.
 */
btree& btree::operator=( const btree &tree )
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
 * @brief Function which returns the total number of nodes in the btree
 * @return long - Return the number of nodes in the binary tree
 */
long btree::nodes() const
{
    return node_count;
}

/**
 * @brief Destroys the tree and frees up memory, then zeroizes the btree to initial state
 */
void btree::destroy_tree()
{
    destroy_tree(root);
    zeroize();
}


// btree protected member functions

/**
 * @brief Protected insert function which inserts node if non-existent, or increments counter if found
 * @details The protected insert begins with the root node which is passed in as the starting point by the public
 * insert function.  The function first tries to locate the node and inserts it (in order) if it is not found.  If the
 * node is found then the count (frequency) of the node is incremented.  The function is recursive until it is known
 * that the node does not exist in the tree in which case it is added with an initial count of 1.
 * @param [in] key - The key to insert if not found, or the count to increment if found
 * @param [in] leaf - The current node being searched.
 */
void btree::insert( long key, node *leaf )
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
            leaf->right = new node;
            leaf->right->key_value = key;

            leaf->right->count = 1;
            node_count++;
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
            leaf->left=new node;
            leaf->left->key_value = key;

            leaf->left->count = 1;
            node_count++;
        }  
    }
}

/**
 * @brief Protected search function which return a pointer to the node if found, or nullptr if not
 * @param [in] key - The key to search for in the tree.
 * @param [in] leaf - The current node being searched.
 * @return node* - Pointer to the located node if found, or nullptr if not.
 */
node *btree::search( long key, node *leaf ) const
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
 * @param [in] node - The current node being traversed
 * @param [in] sum - The cumulative sum of counts for this subtree
 * @param [in] func - Optional function which can be used for custom node processing during traversal
 * @param [in] forward - Boolean value indicating if forward (true) or reverse (false) traversal is required
 * @return long - Returns the total number of counts from all nodes in the tree
 */
long btree::traverse( node *node, long &sum, void (*func)( const long key, long count ), bool forward ) const
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
 * @details This function can be used to duplicate any subtree of a binary tree.  If called using the root node as
 * the starting point it will replicate the entire tree.  New memory is allocated for the node and then values are
 * copied to the new node.  It then recursively calls for replication of the left and right subtrees.
 * @param [in] nptr - The current node (and subtree) being duplicated.
 * @return node* - Return a pointer to the node (and subtree) just duplicated.
 */
node* btree::duplicate( node *nptr )
{
    // Check for leaf nodes
    if ( nptr == nullptr )
        return nullptr;

    // Create a new node and copy the contents
    node *node_copy = new node;
    node_copy->count = nptr->count;
    node_copy->key_value = nptr->key_value;

    // Now copy the left and right subtrees
    node_copy->left = duplicate( nptr->left );
    node_copy->right = duplicate( nptr->right );

    return node_copy;
}

/**
 * @brief Destroys current node and subtending nodes
 * @details This function can be used to destroy any subtree of a binary tree.  If called using the root node as
 * the starting point it will destroy the entire tree.  The function recursively calls for the destruction of the
 * left and right subtrees and upon return frees the current node.
 * @param [in] leaf - The current node (and subtree) freed.
 */
void btree::destroy_tree( node *leaf )
{
    // If the current node is not null then destruction is required
    if( leaf != nullptr )
    {
        // Prior to destroying the current node recursively destroy left and right subtrees
        destroy_tree(leaf->left);
        destroy_tree(leaf->right);

        // Once subtrees are destroyed free up the memory allocated for the current node
        delete leaf;
    }
}

/**
 * @brief Initializes the class by setting the root node to nullptr and node count to 0.
 */
void btree::zeroize()
{
    root = nullptr;
    node_count = 0;
}
