# Collatz Conjecture

This project was written not as proof per se, but due a sincere need for empirical validation of a number of constructs
underlying the formal proof.  The challenge with establishing the Collatz Conjecture is that in order to solidify some
elements of the proof drives a need for manipulating some very large integers - well beyond human and even beyond the
range of commonplace 64-bit or even 128-bit integers.

Tabulating certain parameters such as the histogram distribution of the number of distinct convergent orbits and
their associated path lengths quickly outgrew the capacity to do by hand.  Computing power was needed to provide
the ability to process hundreds of billions of integers in order to show that they behaved exactly as anticipated.

This also called for the development of a number of novel data structures in order to maintain efficiency while
remaining uncompromising in the rigorousness of the approach.  One of the primary performance optimizations was the
development of an array of binary trees where each element of a given tree represents a convergent orbit of a fixed
length.  This allowed for paths of the equal overall length (equal total factors of 2 in a convergent orbit) - but
composed in a different order to be easily compared with one another.

The orbits themselves are represented as the exponent of 2 which can be divided into an integer after a 3n+1 Collatz
Connection which is guaranteed to be an even integer and therefore has at least on factor of 2.  The struct holding the
orbit elements are part of a 64-bit union which facilitates extremely rapid integer comparison operations between any
two orbits with the same total number of factors of two.

Patterns which were predicted to occur in theory do indeed prove to be the case when investigated in this exhaustive
and computationally intensive way.  This was very gratifying to see play out.

## Code for Validating the Collatz Conjecture

The `path` class holds the data structure which includes the number of factors required for convergence and the complete
orbital sequence.  The `path` class can also compute the equivalence class with a default length which represents the minimum
equivalence class length required to guarantee convergence.

The default `path` object is based on the internal long long signed integer (int64_t) representation as defined in <stdint.h>.
The path class however is simply a typedef specialization of the `t_path<>` template - and so can be replaced as needed with
a 32-bit (int32_t) where 32-bit is a requirement.
```C++
    typedef t_path<int64_t> path;
```
The build also accommodates using GNU multiple precision library.  There is switch in common.hpp called gnu_mp.

Multiple Precision compilinig is enabled by default.  Simply comment out the following line to compile without GNU MP.
```C++
    #define gnu_mp              // Comment this out if you do not wish to compile using the GNU mulitple precision libraries
```
The instantiation of the multiple precision variant is also controlled by the define, but when enabled creates the `mp_path` type.
```C++
    typedef t_path<mpz_class> mp_path;
```

In addition, the `tasks.json` file defining the process for creating the executable includes flags for the linker to bind to
the GNU MP libraries.  To disable this, delete or comment out the following lines from the task as required:

    "-lgmpxx",
    "-lgmp",

## OEIS

The online encyclopedia of integer sequences (https://oeis.org) is an enormously helpful resource which was used heavily in
developing this proof and for validating the C++ implementations of a number of relevant OIES sequences.

Within the `oies.cpp` and `oeis.hpp` files are C++ class implementations of a collection of Collatz related integer sequence
implementations.  The gotcha here is that this is part of a sub-menu which is only accessible if you have built with
the GNU MP libraries since many of the required integers in those sequences produce integers that are well beyond the
limits of standard precision integers.
