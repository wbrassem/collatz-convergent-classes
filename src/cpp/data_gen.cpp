/**
 * @file data_gen.cpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief Calls the data file generation functions
 * @version 1.0
 * @date 2025-12-20
 * 
 * @copyright Copyright (c) 2025 Wayne Brassem
 * 
 */

 #include <filesystem>
#include <fstream>

#include "common.hpp"
#include "oeis.hpp"
#include "path.hpp"

/**
 * @brief Calculates the sum of novel convergence fractions for a range of terms.
 * @details This functions accepts two input arguments which is the starting term number and the number of terms.
 * The terms are composed of novel convergence fractions which are represented as discrete integers for numerator
 * and denominator.  The sum is returned in the numer and denom multiple precision integers passed by reference.
 * If the input arguments start and/or term are invalid, then the values returned by reference is 1 over 1 (unity).
 * @param [in] start - The term numer where to begin the summation.
 * @param [in] terms - The number of terms to summate.
 * @param [out] numer - The multiple precision numerator of the summation of novel terms.
 * @param [out] denom - The multiple precision denominator of the summation of novel terms.
 */
void novel_sum(int start, int terms, mpz_class& numer, mpz_class& denom)
{
    // Initialize the returned values
    numer = 1;
    denom = 1;

    // If no terms needed then return immediately with the modified reference parameters
    // If the start term is less than 0 then also return immediately as this does not make semantic sense
    if ( (terms < 1) || (start < 0) )
        return;

    A186009 a186009{start+1};               // Stores the numerator of the dropping pattern
    A022921 a022921{start+1};               // Holds the first difference of A020914 sequence

    A020914 a020914{start+1};               // Calculate the starting denominator exponent of 2
    A000079 a000079{a020914()};             // Use the exponent to calculate the starting denominator

    // Initialize the starting value for the summation
    denom=a000079();
    numer=a186009();

    // Generate the numerators and denominators of the summation
    // Enter the loop only if there is more than one term in the summation
    for (uint16_t n = start+1; n < start+terms; ++n)
    {
        uint16_t factor;                    // Growth factor for the term

        // Position to next novel convergence numerator
        ++a186009;

        // Calculate the new denominator
        if (a022921() == 1)
            factor = 2;
        else
            factor = 4;

        // Scale the denominator and numerator by the same amount (fraction remains equivalent)
        denom *= factor;
        numer *= factor;

        // Add in the new novel convergence numerator to the summation (faction shares the denominator)
        numer += a186009();

        // Move to the next exponent of 2 increment
        ++a022921;
    }
}

/**
 * @brief Calculates the sum of novel convergence fractions for a range of terms.
 * @details This functions accepts two input arguments which is the starting term number and the number of terms.
 * The terms are composed of novel convergence fractions which are represented as discrete integers for numerator
 * and denominator.  The sum is returned in the numer and denom multiple precision integers passed by reference.
 * If the input arguments start and/or term are invalid, then the values returned by reference is 1 over 1 (unity).
 * @param [in] start - The term numer where to begin the summation.
 * @param [in] terms - The number of terms to summate.
 * @param [out] numer - The multiple precision numerator of the summation of novel terms.
 * @param [out] denom - The multiple precision denominator of the summation of novel terms.
 */
void novel_sum_2(int start, int terms, mpz_class& numer, mpz_class& denom)
{
    // Initialize the returned values
    numer = 1;
    denom = 1;

    // If no terms needed then return immediately with the modified reference parameters
    // If the start term is less than 0 then also return immediately as this does not make semantic sense
    if ( (terms < 1) || (start < 0) || (terms > 15) )
        return;

    // Variables to hold the values as we iterate through list
    uint32_t growth[30] = {0};
    mpz_class numers[30], denoms[30];
    uint16_t range = 2*terms, begin = start+1, end = start + range;      // Twice the term length because we need the ratio of two sums

    // Initialize the parallel arrays
    for (uint16_t i = 0; i < end; i++ )
        numers[i] = denoms[i] = growth[i] = 1;    

    A186009 a186009{start+1};               // Stores the numerator of the dropping pattern
    A022921 a022921{start+1};               // Holds the first difference of A020914 sequence

    A020914 a020914{start+1};               // Calculate the starting denominator exponent of 2
    A000079 a000079{a020914()};             // Use the exponent to calculate the starting denominator

    // Initialize the starting value for the summation
    denoms[0]=a000079();
    denom=a000079();
    numer=a186009();

    // Generate the numerators and denominators of the summation
    // Enter the loop only if there is more than one term in the summation
    // for (uint16_t n = start+1; n < start+terms; ++n)
    for (uint16_t n = begin; n < end; ++n)
    {
        uint16_t factor;                    // Growth factor for the term

        growth[n] = 1;

        // Calculate the new denominator
        if (a022921() == 1)
            factor = 2;
        else
            factor = 4;

        // Mimic that which was done for single values
        numers[n] = numers[n-1] * factor;
        denoms[n] = denoms[n-1] * factor;

        numers[n] += ++a186009;

        // So now grow all the growth array for all terms by the growth factor
        for (uint16_t i = n+1; i < n+end; i++ )
            growth[i%range] *= factor;

        // Scale the denominator and numerator by the same amount (fraction remains equivalent)
        // denom *= factor;
        // numer *= factor;

        // Add in the new novel convergence numerator to the summation (faction shares the denominator)
        // numer += a186009();

        // Move to the next exponent of 2 increment
        ++a022921;
    }

    // So the initial cache of numerators and denominators is constructed now take differential sums using the growth matrix
    for (uint16_t n = 0; n < 10; ++n)
    {
        uint16_t curr_lower_index = (terms+n) % range;      // Needed?
        uint16_t curr_upper_index = (range+n) % range;

        uint16_t prev_lower_index = (terms+n-1) % range;
        uint16_t prev_upper_index = (range+n-1) % range;

        mpz_class upper_numer_sum = numers[prev_upper_index] - growth[prev_lower_index] * numers[prev_lower_index];
        mpz_class upper_denom_sum = denoms[prev_upper_index];
        mpz_class lower_numer_sum = numers[prev_lower_index];
        mpz_class lower_denom_sum = denoms[prev_lower_index];


        uint16_t factor;                    // Growth factor for the term

        growth[curr_upper_index] = 1;

        // Calculate the new denominator
        if (a022921() == 1)
            factor = 2;
        else
            factor = 4;

        // Mimic that which was done for single values
        numers[curr_upper_index] = numers[prev_upper_index] * factor;
        denoms[curr_upper_index] = denoms[prev_upper_index] * factor;

        numers[curr_upper_index] += ++a186009;

        // So now grow all the growth array for all terms by the growth factor
        for (uint16_t i = n+1; i < n+end; i++ )
            growth[i%range] *= factor;

        // Scale the denominator and numerator by the same amount (fraction remains equivalent)
        // denom *= factor;
        // numer *= factor;

        // Add in the new novel convergence numerator to the summation (faction shares the denominator)
        // numer += a186009();

        // Move to the next exponent of 2 increment
        ++a022921;
    }
}

/**
 * @brief Calculates the sum of novel convergence fractions for a range of terms.
 * @details This functions accepts two input arguments which is the starting term number and the number of terms.
 * The terms are composed of novel convergence fractions which are represented as discrete integers for numerator
 * and denominator.  The sum is returned in the numer and denom multiple precision integers passed by reference.
 * If the input arguments start and/or term are invalid, then the values returned by reference is 1 over 1 (unity).
 * @param [in] terms - The number of terms to summate.
 * @param [in] t - The maximum number of elements to evaluate.
 * @param [in] fptr - A pointer to an open file to write results to.
 */
// void novel_sum_3(int start, int terms, mpz_class& numer, mpz_class& denom)
void novel_sum_3(int terms, int t, FILE *fptr)
{
    char buffer[80];

    // If no terms needed then return immediately with the modified reference parameters
    if ( (terms < 2) || (terms > 15) )
        return;

    Cumulative cumulative;
    mpz_class numers[30], denoms[30], differ[30];

    uint32_t range = 2*terms;      // Twice the term length because we need the ratio of two sums
    mpz_class power_of_2;

    // Initialize the parallel arrays with novel convergence factors
    for (uint32_t n = 0; n < t+range; ++n) {

        // These values are always the same for a given value for n
        denoms[n%range] = cumulative.denominator();
        numers[n%range] = cumulative.numerator();

        // If index is less that one group of terms, just store values literally
        if ( n < terms ) {
            differ[n%range] = cumulative.numerator();
        }

        // Otherwise there is at least one group of terms, so compute the differential
        else {
            power_of_2 = denoms[n%range] / denoms[(n-terms)%range];
            differ[n%range] = cumulative.numerator() - power_of_2 * numers[(n-terms)%range];    

            // If the index has at least two complete groups you can begin to compute the ratio between groups
            if ( n >= range) {

                // 53=cycle ratios provide for greater smoothness
                if ( !( (n-range) % 10) ) {
                    uint16_t curr_sum_index = (n-1) % range;
                    uint16_t prev_sum_index = (n-terms-1) % range;
    
                    mpz_class ratio_num = differ[curr_sum_index] * denoms[prev_sum_index];
                    mpz_class ratio_den = differ[prev_sum_index] * denoms[curr_sum_index];
            
                    // gmp_printf("terms = %d, n = %d: ratio = %Zd/%Zd = %9.7Ff\n", terms, n-range, ratio_num, ratio_den, ratio);
                    mpf_class ratio = mpf_class(ratio_num) / mpf_class(ratio_den);
                    gmp_printf("terms = %02d, n = %5d: ratio = %9.7Ff\n", terms, n-range, ratio);

                    if (fptr) {
                        gmp_sprintf(buffer, "%d,%d,%d,%9.7Ff", n-range, n-terms, n, ratio);
                        fprintf(fptr,"%s\n", buffer);
                    }
                }
            }
        }

        // Increment the cumulative ratio from which all values are derived
        ++cumulative;
    }

    // Drop a newline as a separator
    printf("\n");
}

/**
 * @brief Calculates the sum of novel convergence fractions for a range of terms.
 * @details This functions accepts two input arguments which is the starting term number and the number of terms.
 * The terms are composed of novel convergence fractions which are represented as discrete integers for numerator
 * and denominator.  The sum is returned in the numer and denom multiple precision integers passed by reference.
 * If the input arguments start and/or term are invalid, then the values returned by reference is 1 over 1 (unity).
 * @param [in] terms - The number of terms to summate.
 * @param [in] t - The maximum number of elements to evaluate.
 * @param [in] fout - A reference to an output stream to write results to.
 * @param [in] cycle_elem_41 - An array of int8_t representing the 41-cycle elements.
 * @param [in] cycle_elem_53 - An array of int8_t representing the 53-cycle elements.
 */
void novel_sum_4(int terms, int t, FILE *fptr, int8_t *cycle_elem_41, int8_t *cycle_elem_53)
// void novel_sum_4(int terms, int t, std::ostream& fout, int8_t *cycle_elem_41, int8_t *cycle_elem_53)
{
    char buffer[80];

    // If no terms needed then return immediately with the modified reference parameters
    if ( (terms < 2) || (terms > 15) )
        return;

    Cumulative cumulative;
    mpz_class numers[60], denoms[60], differ[60];

    uint32_t range = 2*terms;      // Twice the term length because we need the ratio of two sums
    mpz_class power_of_2;

    // Initialize the parallel arrays with novel convergence factors
    for (uint32_t n = 0; n < t+range; ++n) {

        // These values are always the same for a given value for n
        denoms[n%range] = cumulative.denominator();
        numers[n%range] = cumulative.numerator();

        // If index is less that one group of terms, just store values literally
        if ( n < terms ) {
            differ[n%range] = cumulative.numerator();
        }

        // Otherwise there is at least one group of terms, so compute the differential
        else {
            power_of_2 = denoms[n%range] / denoms[(n-terms)%range];
            differ[n%range] = cumulative.numerator() - power_of_2 * numers[(n-terms)%range];    

            // If the index has at least two complete groups you can begin to compute the ratio between groups
            if ( n >= range) {

                uint16_t curr_sum_index = (n-1) % range;
                uint16_t prev_sum_index = (n-terms-1) % range;

                mpz_class ratio_num = differ[curr_sum_index] * denoms[prev_sum_index];
                mpz_class ratio_den = differ[prev_sum_index] * denoms[curr_sum_index];
        
                // gmp_printf("terms = %d, n = %d: ratio = %Zd/%Zd = %9.7Ff\n", terms, n-range, ratio_num, ratio_den, ratio);
                mpf_class ratio = mpf_class(ratio_num) / mpf_class(ratio_den);

                // Write to file if you have a file pointer
                if (fptr) {

                    // Choose which values and column to populate for 41-cycle or 53-cycle
                    if ( cycle_elem_53[n] < 0 )
                        gmp_sprintf(buffer, "%d,%d,%d,%9.7Ff,", n-range, n-terms, n, ratio, cycle_elem_41[n]);
                    else
                        gmp_sprintf(buffer, "%d,%d,%d,,%9.7Ff", n-range, n-terms, n, ratio, cycle_elem_53[n]);
                    // gmp_sprintf(buffer, "%d,%d,%d,%9.7Ff", n-range, n-terms, n, ratio);
                    fprintf(fptr,"%s\n", buffer);
                }

                // 53=cycle ratios provide for greater smoothness
                if ( !( (n-range) % 10) ) {
                    gmp_printf("terms = %02d, n = %5d: ratio = %9.7Ff, 53[] = %d, 41[] = %d\n",
                                terms, n-range, ratio, cycle_elem_53[n], cycle_elem_41[n]);
                }
            }
        }

        // Increment the cumulative ratio from which all values are derived
        ++cumulative;
    }

    // Drop a newline as a separator
    printf("\n");
}

/**
 * @brief Calculates and returns in a buffer the ratios of novel sums for a range of terms.
 * @details This functions accepts two input arguments which is the starting term number and
 * the number of terms.  Writes to a buffer the ratios of novel sums for the specified range.
 * @param [in,out] buffer - A character buffer to hold the output string.
 * @param [in] start - The term numer where to begin the summation. 
 * @param [in] terms - The number of terms to summate.
 */
char* ratios(char* buffer, uint32_t start, uint32_t terms)
// char* ratios(char* buffer, uint16_t start, uint16_t terms)
{
    mpz_class n1, d1;
    mpz_class n2, d2;
    uint16_t len = sprintf(buffer, "  %3d   ", start);

    // Write to a buffer a given number of consecutive ratios
    for (uint16_t n=start; n<start+terms; n++)
    {
        novel_sum(n-1, 1, n1, d1);
        novel_sum(n, 1, n2, d2);

        mpz_class top{ n2*d1 }, bottom{ n1*d2 };
        mpf_class ratio = mpf_class(top) / mpf_class(bottom);

        len += gmp_sprintf(buffer+len, "%7.5Ff ", ratio);
    }

    // Return a pointer to the start of the buffer
    return buffer;
}

void ratio_table()
{
    char buffer[200] = {};
    uint16_t len = sprintf(buffer, "   n+   ");

    // Add the column headings
    for (uint16_t i=0; i<12; ++i)
    {
        len += sprintf(buffer+len, "   %02d   ", i);
    }
    printf("%s\n", buffer);

    // 53-cycle
    printf("%s\n", std::string(103, '-').c_str());
    printf("%s\n", ratios(buffer,   0, 12));
    printf("%s\n", ratios(buffer,  12, 12));
    printf("%s\n", ratios(buffer,  24, 12));
    printf("%s\n", ratios(buffer,  36, 12));
    printf("%s\n", ratios(buffer,  48,  5));

    // 53-cycle
    printf("%s\n", std::string(103, '-').c_str());
    printf("%s\n", ratios(buffer,  53, 12));
    printf("%s\n", ratios(buffer,  65, 12));
    printf("%s\n", ratios(buffer,  77, 12));
    printf("%s\n", ratios(buffer,  89, 12));
    printf("%s\n", ratios(buffer, 101,  5));

    // 53-cycle
    printf("%s\n", std::string(103, '-').c_str());
    printf("%s\n", ratios(buffer, 106, 12));
    printf("%s\n", ratios(buffer, 118, 12));
    printf("%s\n", ratios(buffer, 130, 12));
    printf("%s\n", ratios(buffer, 142, 12));
    printf("%s\n", ratios(buffer, 154,  5));

    // 53-cycle
    printf("%s\n", std::string(103, '-').c_str());
    printf("%s\n", ratios(buffer, 159, 12));
    printf("%s\n", ratios(buffer, 171, 12));
    printf("%s\n", ratios(buffer, 183, 12));
    printf("%s\n", ratios(buffer, 195, 12));
    printf("%s\n", ratios(buffer, 207,  5));

    // 53-cycle
    printf("%s\n", std::string(103, '-').c_str());
    printf("%s\n", ratios(buffer, 212, 12));
    printf("%s\n", ratios(buffer, 224, 12));
    printf("%s\n", ratios(buffer, 236, 12));
    printf("%s\n", ratios(buffer, 248, 12));
    printf("%s\n", ratios(buffer, 260,  5));

    // 53-cycle
    printf("%s\n", std::string(103, '-').c_str());
    printf("%s\n", ratios(buffer, 265, 12));
    printf("%s\n", ratios(buffer, 277, 12));
    printf("%s\n", ratios(buffer, 289, 12));
    printf("%s\n", ratios(buffer, 301, 12));
    printf("%s\n", ratios(buffer, 313,  5));
}

void enough(uint16_t n)
{
    char buffer[200] = {};
    mpz_class n1, d1;
    mpz_class n2, d2;
    mpz_class top, bottom;
    mpf_class ratio;
    double threshold = 1.5;

    // The first novel sum becomes the demon of ratio and includes all of the prior N(n) so the second arg is +1
    novel_sum(n, 11, n1, d1);     // Includes all of previous interval to maximize denominator
    novel_sum(n, 22, n2, d2);

    printf("************ Processing for n = %d ******************\n", n);
    gmp_printf("n = %d, n1 = %7.5Fe\n", n, mpf_class(n1));
    gmp_printf("n = %d, d1 = %7.5Fe\n", n, mpf_class(d1));
    gmp_printf("n = %d, n2 = %7.5Fe\n", n+11, mpf_class(n2));
    gmp_printf("n = %d, d2 = %7.5Fe\n", n+11, mpf_class(d2));

    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);
    gmp_printf("R(%d,%d,%d) = %7.5Fe\n", n, 22, 11, ratio);

    gmp_sprintf(buffer, "%7.5Ff", ratio);
    if ( ratio > threshold ) {
        printf("n = %d. The ratio of 22 over 11 is %s > %6.3f - it's enough\n", n, buffer, threshold);
        // return;
    }
    else
        printf("n = %d. The ratio of 22 over 11 is %s < %6.3f - it's not enough\n", n, buffer, threshold);

    novel_sum(n, 23, n2, d2);
    gmp_printf("n = %d, n2 = %7.5Fe\n", n+12, mpf_class(n2));
    gmp_printf("n = %d, d2 = %7.5Fe\n", n+12, mpf_class(d2));

    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);
    gmp_printf("R(%d,%d,%d) = %7.5Fe\n", n, 23, 11, ratio);

    gmp_sprintf(buffer, "%7.5Ff", ratio);
    if ( ratio > 1.5 ) {
        printf("n = %d. The ratio of 23 over 11 is %s - it's enough\n", n, buffer);
        // return;
    }
    else
        printf("n = %d. The ratio of 23 over 11 is %s - it's not enough\n", n, buffer);

    novel_sum(n, 34, n2, d2);
    gmp_printf("n = %d, n2 = %7.5Fe\n", n+23, mpf_class(n2));
    gmp_printf("n = %d, d2 = %7.5Fe\n", n+23, mpf_class(d2));

    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);
    gmp_printf("R(%d,%d,%d) = %7.5Fe\n", n, 34, 11, ratio);

    gmp_sprintf(buffer, "%7.5Ff", ratio);
    if ( ratio > 1.75 ) {
        printf("n = %d. The ratio of 34 over 11 is %s - it's enough\n", n, buffer);
        // return;
    }
    else
        printf("n = %d. The ratio of 34 over 11 is %s - it's not enough\n", n, buffer);

    novel_sum(n, 35, n2, d2);
    gmp_printf("n = %d, n2 = %7.5Fe\n", n+34, mpf_class(n2));
    gmp_printf("n = %d, d2 = %7.5Fe\n", n+34, mpf_class(d2));

    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);
    gmp_printf("R(%d,%d,%d) = %7.5Fe\n", n, 35, 11, ratio);

    gmp_sprintf(buffer, "%7.5Ff", ratio);
    if ( ratio > 1.75 ) {
        printf("n = %d. The ratio of 35 over 11 is %s - it's enough\n", n, buffer);
        // return;
    }
    else
        printf("n = %d. The ratio of 35 over 11 is %s - it's not enough\n", n, buffer);

    novel_sum(n, 46, n2, d2);
    gmp_printf("n = %d, n2 = %7.5Fe\n", n+35, mpf_class(n2));
    gmp_printf("n = %d, d2 = %7.5Fe\n", n+35, mpf_class(d2));

    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);
    gmp_printf("R(%d,%d,%d) = %7.5Fe\n", n, 46, 11, ratio);

    gmp_sprintf(buffer, "%7.5Ff", ratio);
    if ( ratio > 1.875 ) {
        printf("n = %d. The ratio of 46 over 11 is %s - it's enough\n", n, buffer);
        return;
    }
    else
        printf("n = %d. The ratio of 46 over 11 is %s - it's not enough\n", n, buffer);
}

void latex11term(uint16_t n)
{
    mpz_class n1, d1;
    mpz_class n2, d2;
    mpz_class top, bottom;
    mpf_class ratio;

    novel_sum(n, 11, n1, d1);
    novel_sum(n, 22, n2, d2);

    printf("\n\\midrule\n");
    gmp_printf("%d & %d  &     &     &     &  %7.5Fe / %7.5Fe &    -    &      -    \\\\ \n", n, 11, mpf_class(n1), mpf_class(d1));

    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.5 ) {
        gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.500$ \\\\ \n", n, 11, 11, mpf_class(n2), mpf_class(d2), ratio);
        return;
    }
    else {
        gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.500$ \\\\ \n", n, 11, 11, mpf_class(n2), mpf_class(d2), ratio);
    }
    
    novel_sum(n, 23, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.5 ) {
        gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.500$ \\\\ \n", n, 11, 12, mpf_class(n2), mpf_class(d2), ratio);
        // return;
    }
    else {
        gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.500$ \\\\ \n", n, 11, 12, mpf_class(n2), mpf_class(d2), ratio);
    }

    novel_sum(n, 34, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.75 ) {
        gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.750$ \\\\ \n", n, 11, 12, 11, mpf_class(n2), mpf_class(d2), ratio);
        return;
    }
    else {
        gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.750$ \\\\ \n", n, 11, 12, 11, mpf_class(n2), mpf_class(d2), ratio);
    }

    novel_sum(n, 35, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.75 ) {
        gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.750$ \\\\ \n", n, 11, 12, 12, mpf_class(n2), mpf_class(d2), ratio);
        // return;
    }
    else {
        gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.750$ \\\\ \n", n, 11, 12, 12, mpf_class(n2), mpf_class(d2), ratio);
    }

    novel_sum(n, 46, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.875 ) {
        gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.875$ \\\\ \n", n, 11, 12, 12, 11, mpf_class(n2), mpf_class(d2), ratio);
        return;
    }
    else {
        gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.875$ \\\\ \n", n, 11, 12, 12, 11, mpf_class(n2), mpf_class(d2), ratio);
    }
}

void latex12term(uint16_t n)
{
    mpz_class n1, d1;
    mpz_class n2, d2;
    mpz_class top, bottom;
    mpf_class ratio;

    novel_sum(n, 12, n1, d1);
    novel_sum(n, 24, n2, d2);

    printf("\n\\midrule\n");
    gmp_printf("%d & %d  &     &     &     &  %7.5Fe / %7.5Fe &    -    &      -    \\\\ \n", n, 12, mpf_class(n1), mpf_class(d1));

    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.5 ) {
        gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.500$ \\\\ \n", n, 12, 12, mpf_class(n2), mpf_class(d2), ratio);
        return;
    }
    else {
        gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.500$ \\\\ \n", n, 12, 12, mpf_class(n2), mpf_class(d2), ratio);
    }
    
    novel_sum(n, 25, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.5 ) {
        gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.500$ \\\\ \n", n, 12, 13, mpf_class(n2), mpf_class(d2), ratio);
        // return;
    }
    else {
        gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.500$ \\\\ \n", n, 12, 13, mpf_class(n2), mpf_class(d2), ratio);
    }
    
    // novel_sum(n, 26, n2, d2);
    // top = n2*d1;
    // bottom = n1*d2;
    // ratio = mpf_class(top) / mpf_class(bottom);

    // if ( ratio > 1.5 ) {
    //     gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.500$ \\\\ \n", n, 12, 14, mpf_class(n2), mpf_class(d2), ratio);
    //     // return;
    // }
    // else {
    //     gmp_printf("%d & %d  & %d  &     &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.500$ \\\\ \n", n, 12, 14, mpf_class(n2), mpf_class(d2), ratio);
    // }

    novel_sum(n, 37, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.75 ) {
        gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.750$ \\\\ \n", n, 12, 13, 12, mpf_class(n2), mpf_class(d2), ratio);
        return;
    }
    else {
        gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.750$ \\\\ \n", n, 12, 13, 12, mpf_class(n2), mpf_class(d2), ratio);
    }

    novel_sum(n, 38, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.75 ) {
        gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.750$ \\\\ \n", n, 12, 13, 13, mpf_class(n2), mpf_class(d2), ratio);
        // return;
    }
    else {
        gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.750$ \\\\ \n", n, 12, 13, 13, mpf_class(n2), mpf_class(d2), ratio);
    }

    // novel_sum(n, 38, n2, d2);
    // top = n2*d1;
    // bottom = n1*d2;
    // ratio = mpf_class(top) / mpf_class(bottom);

    // if ( ratio > 1.75 ) {
    //     gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.750$ \\\\ \n", n, 12, 14, 12, mpf_class(n2), mpf_class(d2), ratio);
    //     // return;
    // }
    // else {
    //     gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.750$ \\\\ \n", n, 12, 14, 12, mpf_class(n2), mpf_class(d2), ratio);
    // }

    // novel_sum(n, 39, n2, d2);
    // top = n2*d1;
    // bottom = n1*d2;
    // ratio = mpf_class(top) / mpf_class(bottom);

    // if ( ratio > 1.75 ) {
    //     gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.750$ \\\\ \n", n, 12, 14, 13, mpf_class(n2), mpf_class(d2), ratio);
    //     // return;
    // }
    // else {
    //     gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.750$ \\\\ \n", n, 12, 14, 13, mpf_class(n2), mpf_class(d2), ratio);
    // }

    // novel_sum(n, 40, n2, d2);
    // top = n2*d1;
    // bottom = n1*d2;
    // ratio = mpf_class(top) / mpf_class(bottom);

    // if ( ratio > 1.75 ) {
    //     gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.750$ \\\\ \n", n, 12, 14, 14, mpf_class(n2), mpf_class(d2), ratio);
    //     // return;
    // }
    // else {
    //     gmp_printf("%d & %d  & %d  & %d  &     &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.750$ \\\\ \n", n, 12, 14, 14, mpf_class(n2), mpf_class(d2), ratio);
    // }

    novel_sum(n, 50, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.875 ) {
        gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.875$ \\\\ \n", n, 12, 13, 13, 12, mpf_class(n2), mpf_class(d2), ratio);
        return;
    }
    else {
        gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.875$ \\\\ \n", n, 12, 13, 13, 12, mpf_class(n2), mpf_class(d2), ratio);
    }

    novel_sum(n, 51, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.875 ) {
        gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.875$ \\\\ \n", n, 12, 13, 13, 13, mpf_class(n2), mpf_class(d2), ratio);
        // return;
    }
    else {
        gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.875$ \\\\ \n", n, 12, 13, 13, 13, mpf_class(n2), mpf_class(d2), ratio);
    }

    // novel_sum(n, 52, n2, d2);
    // top = n2*d1;
    // bottom = n1*d2;
    // ratio = mpf_class(top) / mpf_class(bottom);

    // if ( ratio > 1.875 ) {
    //     gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.875$ \\\\ \n", n, 12, 14, 14, 12, mpf_class(n2), mpf_class(d2), ratio);
    //     return;
    // }
    // else {
    //     gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.875$ \\\\ \n", n, 12, 14, 14, 12, mpf_class(n2), mpf_class(d2), ratio);
    // }

    // novel_sum(n, 53, n2, d2);
    // top = n2*d1;
    // bottom = n1*d2;
    // ratio = mpf_class(top) / mpf_class(bottom);

    // if ( ratio > 1.875 ) {
    //     gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.875$ \\\\ \n", n, 12, 14, 14, 13, mpf_class(n2), mpf_class(d2), ratio);
    //     // return;
    // }
    // else {
    //     gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.875$ \\\\ \n", n, 12, 14, 14, 13, mpf_class(n2), mpf_class(d2), ratio);
    // }

    // novel_sum(n, 54, n2, d2);
    // top = n2*d1;
    // bottom = n1*d2;
    // ratio = mpf_class(top) / mpf_class(bottom);

    // if ( ratio > 1.875 ) {
    //     gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.875$ \\\\ \n", n, 12, 14, 14, 14, mpf_class(n2), mpf_class(d2), ratio);
    //     return;
    // }
    // else {
    //     gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.875$ \\\\ \n", n, 12, 14, 14, 14, mpf_class(n2), mpf_class(d2), ratio);
    // }

    novel_sum(n, 65, n2, d2);
    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    if ( ratio > 1.9375 ) {
        gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $> 1.9375$ \\\\ \n", n, 12, 13, 13, 27, mpf_class(n2), mpf_class(d2), ratio);
        return;
    }
    else {
        gmp_printf("%d & %d  & %d  & %d  & %d  &  %7.5Fe / %7.5Fe & %7.5Ff & $< 1.9375$ \\\\ \n", n, 12, 13, 13, 27, mpf_class(n2), mpf_class(d2), ratio);
    }
}

// For Reference Only - Copied from above
// char* ratios(char* buffer, uint16_t start, uint16_t terms)
// {
//     mpz_class n1, d1;
//     mpz_class n2, d2;
//     uint16_t len = sprintf(buffer, "  %3d   ", start);

//     // Write to a buffer a given number of consecutive ratios
//     for (uint16_t n=start; n<start+terms; n++)
//     {
//         novel_sum(n-1, 1, n1, d1);
//         novel_sum(n, 1, n2, d2);

//         mpz_class top{ n2*d1 }, bottom{ n1*d2 };
//         mpf_class ratio = mpf_class(top) / mpf_class(bottom);

//         len += gmp_sprintf(buffer+len, "%7.5Ff ", ratio);
//     }

//     // Return a pointer to the start of the buffer
//     return buffer;
// }

// For Reference o=Only - Copied from below
// double partial_threshold(const mpz_class& numer, const mpz_class& denom, const mpz_class& threshold)
// {
//     mpz_class partial_denom = denom - threshold;
//     mpz_class partial_numer = numer - threshold + partial_denom;
//     mpf_class delta = mpf_class( partial_numer ) / mpf_class( partial_denom );

//     return delta.get_d();
// }


// Try to build a function to calculate consecutive ratios
double find_ratio(mpz_class n1, mpz_class d1, mpz_class n2, mpz_class d2)
{
    // mpz_class n1, d1;
    // mpz_class n2, d2;
    mpz_class top, bottom;
    mpf_class ratio;

    top = n2*d1;
    bottom = n1*d2;
    ratio = mpf_class(top) / mpf_class(bottom);

    return ratio.get_d();
}

void capped(uint16_t k, uint16_t n, uint16_t t)
{
    mpz_class n1, d1;
    A000079 a000079(k+1);

    novel_sum(n, t, n1, d1);
    mpf_class ratio = mpf_class(n1)/ mpf_class(d1);
    mpf_class fraction = 1 / mpf_class(a000079());

    gmp_printf("n=%d, terms = %d, ratio = %8.6Fe / %8.6Fe = %8.6Fe \n", n, t, mpf_class(n1), mpf_class(d1), ratio);
    gmp_printf("2^(-%d) = %8.6Fe \n\n", k, fraction);
}

void eleven_or_twelve()
{
    mpz_class n1, d1;
    mpz_class n2, d2;

    // novel_sum(4, 3, n1, d1);
    // novel_sum(4, 8, n2, d2);

    ratio_table();

    for (uint16_t n=424; n<476; ++n)
    // for (uint16_t n=212; n<265; ++n)
        // enough( n );
        // latex11term( n );
        latex12term( n );
}

void Cumulative_seq2( Cumulative *c, int t )
{
    mpz_class numer, denom, power;
    mpf_class ratio, fraction, bracket;
    A000079 a000079(2);
    uint16_t term = 0;

    // Print out the first t elements of the sequence
    for ( int i=1; i<=t; ++i ) {
        numer = c->numerator();
        denom = c->denominator();
        ratio = mpf_class(numer) / mpf_class(denom);
        power = a000079();
        fraction = 1 - (1 / mpf_class(power));     // Calculates the cumulative bracket
        bracket = mpf_class(numer * power) / mpf_class(denom);

        std::cout << std::endl << "n = " << c->index() << ", numerator of N(n) = " << c->novel();
        std::cout << "; numerator of C(n) = " << numer << "; denominator = " << denom << std::endl;

        // Compare cumulative to the power of two
        if ( bracket > power-1 ) {

            // Indicate that the cumulative amount has crossed into another 2^-k bracket
            gmp_printf("New bracket required: %8.6Ff over %Zd \n", bracket, power);

            // Double both the bracket and power for the gmp_printf below
            bracket *= 2;
            power *= 2;

            // Increment the power of 2 and reset the term 1 for the bracket
            ++a000079;
            term = 1;
        }
        // Otherwise it's just another term in the bracket
        else {
            ++term;
        }

        // Print this always
        gmp_printf("term = %d, ratio = %20.18Ff, fraction =  %20.18Ff, bracket = %10.8Ff over %Zd \n",
                        term, ratio, fraction, bracket, power);

        // Increment to the next cumulative entry
        c->operator++();
    }
}

double partial_threshold(const mpz_class& numer, const mpz_class& denom, const mpz_class& threshold)
{
    mpz_class partial_denom = denom - threshold;
    mpz_class partial_numer = numer - threshold + partial_denom;
    mpf_class delta = mpf_class( partial_numer ) / mpf_class( partial_denom );

    return delta.get_d();
}

void Cumulative_seq3( Cumulative *c, uint32_t t )
{
    mpz_class old_numer = 1, old_denom = 1, novel = 0;
    mpz_class numer, denom, power, scale, threshold;
    A000079 a000079(2);

    const uint32_t max_terms=15;            // Term array size
    const uint32_t brackets=20;             // Number of ranges in the frequency histogram
    const uint32_t start=90;                // The first bracket using a granularity of width
    const double width=0.01;                // Width of each bracket the frequency histogram

    double new_interval=1, partial=1, last_partial=1, diff=1;
    double minimums[max_terms], maximums[max_terms]={0};
    std::string interval[max_terms];
    uint32_t term=0, frequency[max_terms]={0}, histogram[max_terms][20]={0};
    std::vector<uint32_t> terms, limits;

    // Turn interval into an array of strings [1] to [14] + create a parallel array as a histogram of the frequency of term counts
    FILE *fptr = fopen("partial.txt", "w");

    // Initialize minimums, maximums and frequency to outside limit values
    for ( uint32_t i=0; i<=max_terms; ++i ) {
        minimums[i] = 2;
    }
    
    // Print out the first t elements of the sequence
    for ( uint32_t i=0; i<=t; ++i ) {
        numer = c->numerator();
        denom = c->denominator();
        novel = c->novel();
        power = a000079();
        scale = denom / power;              // This evaluates to an integer power of 2
        threshold = (power-1) * scale;      // The value the numer must exceed in order to reach next bracket

        // A blip every 1000 terms
        if ( !(i % 1000) ) {
            printf("n = %d\n", i);
        }

        // Compare cumulative to the power of two
        if ( numer > threshold ) {
            char buf[32];
            uint32_t bracket;

            // Save the existing threshold so you can display it
            mpz_class oldthresh = threshold;

            // Calculate the coverage of the 2^-k bracket
            diff = 1 - (last_partial - partial);

            // Calculate the bracket index and increment the histogram
            bracket = (int) (diff/width);
            ++histogram[term][bracket-start];

            // Range checks
            if (diff < minimums[term])
                minimums[term] = diff;

            // Range checks
            if (diff > maximums[term])
                maximums[term] = diff;

            // Generate the tuple
            // sprintf(buf, "(%5d, %2d, %8.6f) ", i-term, term, diff);
            sprintf(buf, "%d,%8.6f\n", i-term, diff);

            // Append the string to the array element and increment frequency
            interval[term] += buf;
            ++frequency[term];

            // Save the number of terms required
            terms.push_back(term);

            // A new bracket now, so save the prior value instead
            limits.push_back(i-1);      

            // Calculate the new threshold
            threshold = (power*2-1) * (scale/2);

            // Compute the fractional component to the next threshold
            new_interval = partial_threshold(numer, denom, threshold);

            // Reset the term value to 1 prior to printing
            term = 1;

            // Save the highest percentage achieved from previous bracket
            last_partial = partial;

            // Indicate that the existing threshold has been exceeded and display the new one
            // gmp_printf("\nn = %04d; term = %02d, partial = %8.6f, threshold=%Zd,numer=%Zd,denom=%Zd;new-threshold:%Zd \n", 
            //             i, term, new_interval, oldthresh, numer, denom, threshold);

            // Output the maximum term and the final threshold
            fprintf(fptr, "\nn = %5d; term = %2d, partial = %8.6f \n", i, term, new_interval);

            // Increment the power of 2
            ++a000079;
        }
        // Otherwise it's just another term in the bracket
        else {

            // Compute the fractional component to the next threshold
            partial = partial_threshold(numer, denom, threshold);

            // Increment the term
            ++term;

            // Output the additional term and the partial threshold
            // gmp_printf("n = %04d, term = %02d, partial = %8.6f, threshold=%Zd,numer=%Zd,denom=%Zd \n",
            //             i, term, partial, threshold, numer, denom);
            fprintf(fptr, "n = %5d, term = %2d, partial = %8.6f \n", i, term, partial);
        }

        // Save old value for next iteration
        old_numer = novel;
        old_denom = denom;

        // Increment to the next cumulative summation
        c->operator++();
    }

    // Write out the collection of each term type to the file
    for ( uint32_t i=1; i<=max_terms; ++i ) {
        if ( frequency[i] > 0)
            fprintf(fptr, "\n%2d-terms: \n%s", i, interval[i].c_str());
        else
            fprintf(fptr, "\n%2d-terms: none\n", i);
    }

    // Write out the frequencies of each term type to the file
    for ( uint32_t i=1; i<=max_terms; ++i ) {
        if (minimums[i] > maximums[i])
            fprintf(fptr, "\n%2d-term occurances: %4d\n", i, 0);
        else
            fprintf(fptr, "\n%2d-term occurances: %4d, range %8.6f to %8.6f\n", i, frequency[i], minimums[i], maximums[i]);
    }

    // Output the histogram results
    for ( uint32_t i=1; i<max_terms; ++i ) {
        fprintf(fptr, "\n%2d-term histogram: ", i);
        for (int j=0; j<brackets; ++j ) {
            if ( histogram[i][j]) {
                double bracket = ( j + start ) * width;
                fprintf(fptr, "%4.2f:%04d ", bracket, histogram[i][j]);
            }
        }
        fprintf(fptr, "\n");
    }

    // Output the values from the term vector
    fprintf(fptr,"\nC(n) Terms\n");
    for ( uint32_t i : terms )
        fprintf(fptr, "%d\n", i);

    // Output the values from the limit vector
    fprintf(fptr,"\nC(n) value of n\n");
    for ( uint32_t i : limits )
        fprintf(fptr, "%d\n", i);

    fclose(fptr);
}

// This function find the number of terms of A186009 needed to cover the next 1/2^n interval
void Cumulative_seq4( Cumulative *c, uint32_t t, int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
{
    mpz_class old_numer = 1, old_denom = 1, novel = 0;
    mpz_class numer, denom, power, scale, threshold;
    A000079 a000079(2);

    const uint32_t max_terms=15;            // Term array size
    const uint32_t brackets=20;             // Number of ranges in the frequency histogram
    const uint32_t start=90;                // The first bracket using a granularity of width
    const double width=0.01;                // Width of each bracket the frequency histogram

    double new_interval=1, partial=1, last_partial=1, diff=1;
    double minimums[max_terms], maximums[max_terms]={0};
    std::string interval[max_terms];
    uint32_t last_i=0, term=0, frequency[max_terms]={0}, histogram[max_terms][20]={0};
    std::vector<uint32_t> terms, limits;

    FILE *fptr;

    // Array for holding the mapping between the index and the position in a 41 or 53 cycle.
    // There are up to 14 terms in a bracket and up to 53 elements per cycle of interest.
    std::string cycle_pos_41[15][54];
    std::string cycle_pos_53[15][54];

    // Turn interval into an array of strings [1] to [14] + create a parallel array as a histogram of the frequency of term counts
    fptr = fopen("partial.txt", "w");

    // Initialize minimums, maximums and frequency to outside limit values
    for ( uint32_t i=0; i<=max_terms; ++i ) {
        minimums[i] = 2;
    }
    
    // Print out the first t elements of the sequence
    for ( uint32_t i=0; i<=t; ++i ) {
        numer = c->numerator();
        denom = c->denominator();
        novel = c->novel();
        power = a000079();
        scale = denom / power;              // This evaluates to an integer power of 2
        threshold = (power-1) * scale;      // The value the numer must exceed in order to reach next bracket

        // A blip every 1000 terms
        if ( !(i % 200) ) {
            printf("n = %d\n", i);
        }

        // Compare cumulative to the power of two
        if ( numer > threshold ) {
            char buf[32];
            uint32_t bracket;

            // Save the existing threshold so you can display it
            mpz_class oldthresh = threshold;

            // Calculate the coverage of the 2^-k bracket
            diff = 1 - (last_partial - partial);

            // Calculate the bracket index and increment the histogram
            bracket = (int) (diff/width);
            ++histogram[term][bracket-start];

            // Range checks
            if (diff < minimums[term])
                minimums[term] = diff;

            // Range checks
            if (diff > maximums[term])
                maximums[term] = diff;

            // Generate the tuple
            // sprintf(buf, "(%5d, %2d, %8.6f) ", i-term, term, diff);
            sprintf(buf, "%d,%8.6f\n", i-term, diff);

            // Append the string to the array element and increment frequency
            interval[term] += buf;
            ++frequency[term];

            // Try to store the string tied to the position in 41 or 53 cycle
            uint8_t pos;
            if ( cycle_elem_53[last_i] >= 0 ) {
                pos = cycle_elem_53[last_i];
                cycle_pos_53[term][pos] += buf;
            }
            else {
                pos = cycle_elem_41[last_i];
                cycle_pos_41[term][pos] += buf;
            }

            // Save the number of terms required
            terms.push_back(term);

            // A new bracket now, so save the prior value instead
            limits.push_back(i-1);      

            // Calculate the new threshold
            threshold = (power*2-1) * (scale/2);

            // Compute the fractional component to the next threshold
            new_interval = partial_threshold(numer, denom, threshold);

            // Store the starting index position for the next interval
            last_i = i;

            // Reset the term value to 1 prior to printing
            term = 1;

            // Save the highest percentage achieved from previous bracket
            last_partial = partial;

            // Indicate that the existing threshold has been exceeded and display the new one
            // gmp_printf("\nn = %04d; term = %02d, partial = %8.6f, threshold=%Zd,numer=%Zd,denom=%Zd;new-threshold:%Zd \n", 
            //             i, term, new_interval, oldthresh, numer, denom, threshold);

            // Output the maximum term and the final threshold
            fprintf(fptr, "\nn = %5d; term = %2d, partial = %8.6f \n", i, term, new_interval);

            // Increment the power of 2
            ++a000079;
        }
        // Otherwise it's just another term in the bracket
        else {

            // Compute the fractional component to the next threshold
            partial = partial_threshold(numer, denom, threshold);

            // Increment the term
            ++term;

            // Output the additional term and the partial threshold
            // gmp_printf("n = %04d, term = %02d, partial = %8.6f, threshold=%Zd,numer=%Zd,denom=%Zd \n",
            //             i, term, partial, threshold, numer, denom);
            fprintf(fptr, "n = %5d, term = %2d, partial = %8.6f \n", i, term, partial);
        }

        // Save old value for next iteration
        old_numer = novel;
        old_denom = denom;

        // Increment to the next cumulative summation
        c->operator++();
    }

    // Write out the collection of each term type to the file
    for ( uint32_t i=1; i<=max_terms; ++i ) {
        if ( frequency[i] > 0)
            fprintf(fptr, "\n%2d-terms: \n%s", i, interval[i].c_str());
        else
            fprintf(fptr, "\n%2d-terms: none\n", i);
    }

    // Write out the frequencies of each term type to the file
    for ( uint32_t i=1; i<=max_terms; ++i ) {
        if (minimums[i] > maximums[i])
            fprintf(fptr, "\n%2d-term occurances: %4d\n", i, 0);
        else
            fprintf(fptr, "\n%2d-term occurances: %4d, range %8.6f to %8.6f\n", i, frequency[i], minimums[i], maximums[i]);
    }

    // Output the histogram results
    for ( uint32_t i=1; i<max_terms; ++i ) {
        fprintf(fptr, "\n%2d-term histogram: ", i);
        for (int j=0; j<brackets; ++j ) {
            if ( histogram[i][j]) {
                double bracket = ( j + start ) * width;
                fprintf(fptr, "%4.2f:%04d ", bracket, histogram[i][j]);
            }
        }
        fprintf(fptr, "\n");
    }

    // Output the values from the term vector
    fprintf(fptr,"\nC(n) Terms\n");
    for ( uint32_t i : terms )
        fprintf(fptr, "%d\n", i);

    // Output the values from the limit vector
    fprintf(fptr,"\nC(n) value of n\n");
    for ( uint32_t i : limits )
        fprintf(fptr, "%d\n", i);

    // Close the file
    fclose(fptr);

    // Now dump the cycle position specific information
    fptr = fopen("cycle_pos.txt", "w");

    // Outer loop iterates through the term groupings of different length
    for (uint8_t i = 1; i<15; ++i) {
        fprintf(fptr,"\n%d-term Groups\n", i);

        // Inner loop iterates through the positions in a 53-cycle
        for (uint8_t j = 0; j<53; ++j) {

            // If it's not an empty string at the position, dump it out
            if ( !cycle_pos_41[i][j].empty() ) {
                fprintf(fptr,"\n[%d][%d]:41-cycle\n%s", i, j, cycle_pos_41[i][j].c_str());
            }

            // If it's not an empty string at the position, dump it out
            if ( !cycle_pos_53[i][j].empty() ) {
                fprintf(fptr,"\n[%d][%d]:53-cycle\n%s", i, j, cycle_pos_53[i][j].c_str());
            }
        }
    }

    // Close the file
    fclose(fptr);
}

void load_array( int8_t* cycle_elem, uint32_t last, uint32_t elements)
{
    // Now populate the array with the positions in a cycle of length elements 
    uint32_t first = last-elements, pos=0;

    // Load each element with it's position in the cycle
    for (uint32_t i = first; i<last; ++i) {
        cycle_elem[i] = pos++;
    }
}

bool found_5_cycle( A022921& a022921 )
{
    const uint8_t pattern[] = {1,2,1,2,2};
    A022921 copy = a022921;

    for ( int64_t i=0; i<5; ++i ) {
        if ( pattern[i] != copy++ )
            return false;
    }

    a022921 = copy;
    return true;
}

bool found_7_cycle( A022921& a022921 )
{
    const uint8_t pattern[] = {1,2,1,2,1,2,2};
    A022921 copy = a022921;

    for ( int64_t i=0; i<7; ++i ) {
        if ( pattern[i] != copy++ )
            return false;
    }

    a022921 = copy;
    return true;
}

bool found_12_cycle( A022921& a022921 )
{
    A022921 copy = a022921;

    if ( found_7_cycle(copy) && found_5_cycle(copy) ) {
        a022921 = copy;
        return true;
    }
    else
        return false;
}

bool found_41_53_cycle( A022921& a022921, uint16_t subcycles )
{
    A022921 copy = a022921;

    // Cycle through the first 12-cycles and return if not matching
    for ( uint8_t i = 0; i<subcycles; ++i ) {
        if ( !found_12_cycle(copy) ) {
            printf("Failure matching 12-cycle!!! Index = %d\n", copy.index());
            return false;
        }
    }

    if ( !found_5_cycle(copy) ) {
        printf("Failure matching a 5-cycle. Index = %d\n", copy.index());
        return false;
    }

    a022921 = copy;
    return true;
}

bool found_41_cycle( A022921& a022921, int8_t *cycle_elem )
{
    A022921 copy = a022921;

    // Cycle through the first 3 12-cycles and return if not matching
    if ( !found_41_53_cycle( copy, 3 ) ) {
        return false;
    }

    // Load array with 41 elements from 0 to 41
    load_array( cycle_elem, copy.index(), 41 );

    printf("Found a 41-cycle. Index = %d\n", copy.index());
    a022921 = copy;
    return true;
}

bool found_53_cycle( A022921& a022921, int8_t *cycle_elem )
{
    A022921 copy = a022921;

    // Cycle through the first 4 12-cycles and return if not matching
    if ( !found_41_53_cycle( copy, 4 ) ) {
        return false;
    }

    // Load array with 53 elements from 0 to 52
    load_array( cycle_elem, copy.index(), 53 );

    printf("Found a 53-cycle. Index = %d\n", copy.index());
    a022921 = copy;
    return true;
}

bool found_306_359_cycle( A022921& a022921, uint16_t subcycles, int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
{
    A022921 copy = a022921;

    // Cycle through the first 53-cycles and return if not matching
    for ( uint8_t i = 0; i<subcycles; ++i ) {
        if ( !found_53_cycle(copy, cycle_elem_53) ) {
            printf("Failure matching 53-cycle!!! Index = %d\n", copy.index());
            return false;
        }
    }

    if ( !found_41_cycle(copy, cycle_elem_41) ) {
        printf("Not a 41-cycle. Index = %d\n", copy.index());
        return false;
    }

    a022921 = copy;
    return true;
}

bool found_306_cycle( A022921& a022921, int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
{
    A022921 copy = a022921;

    // Cycle through the first 5 53-cycles and return if not matching
    if ( !found_306_359_cycle( copy, 5, cycle_elem_41, cycle_elem_53 ) ) {
        return false;
    }

    printf("Found a 306-cycle. Index = %d\n", copy.index());
    a022921 = copy;
    return true;
}

bool found_359_cycle( A022921& a022921, int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
{
    A022921 copy = a022921;

    // Cycle through the first 6 53-cycles and return if not matching
    if ( !found_306_359_cycle( copy, 6, cycle_elem_41, cycle_elem_53 ) ) {
        return false;
    }

    printf("Found a 359-cycle. Index = %d\n", copy.index());
    a022921 = copy;
    return true;
}

bool found_665_cycle( A022921& a022921,  int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
{
    A022921 copy = a022921;

    // printf("inside found_665_cycle()\n");
    if ( found_359_cycle(copy, cycle_elem_41, cycle_elem_53) && found_306_cycle(copy, cycle_elem_41, cycle_elem_53) ) {
        printf("Found a 665-cycle. Index = %d\n", copy.index());
        a022921 = copy;
        return true;
    }
    else
        return false;
}

bool found_15601_16266_cycle( A022921& a022921, uint16_t subcycles,  int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
{
    A022921 copy = a022921;

    // Cycle through the first 665-cycles and return if not matching
    for ( uint8_t i = 0; i<subcycles; ++i ) {
        if ( !found_665_cycle(copy, cycle_elem_41, cycle_elem_53 ) ) {
            printf("Failure matching 359-cycle!!! Index = %d\n", copy.index());
            return false;
        }
    }

    if ( !found_306_cycle(copy, cycle_elem_41, cycle_elem_53 ) ) {
        printf("Not a 306-cycle. Index = %d\n", copy.index());
        return false;
    }

    a022921 = copy;
    return true;
}

bool found_15601_cycle( A022921& a022921, int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
{
    A022921 copy = a022921;

    // Cycle through the first 23 665-cycles and return if not matching
    if ( !found_15601_16266_cycle( copy, 23, cycle_elem_41, cycle_elem_53 ) ) {
        return false;
    }

    printf("Found a 15601-cycle. Index = %d\n", copy.index());
    a022921 = copy;
    return true;
}

bool found_16266_cycle( A022921& a022921,int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
{
    A022921 copy = a022921;

    // Cycle through the first 24 665-cycles and return if not matching
    if ( !found_15601_16266_cycle( copy, 24, cycle_elem_41, cycle_elem_53 ) ) {
        return false;
    }

    printf("Found a 16266-cycle. Index = %d\n", copy.index());
    a022921 = copy;
    return true;
}

bool found_cycle( A022921& a022921, int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
{
    A022921 copy = a022921;

    // printf("inside found_665_cycle()\n");
    if ( found_16266_cycle(copy, cycle_elem_41, cycle_elem_53) && found_15601_cycle(copy, cycle_elem_41, cycle_elem_53) ) {
        printf("Found a 31867-cycle. Index = %d\n", copy.index());
        a022921 = copy;
        return true;
    }
    else
        return false;
}

void find_cycles( uint8_t* cycle_elem )
{

}

double sum_consecutive_ratios( uint16_t terms, double *ratios )
{
    double sum = ratios[terms];
    for (int j=terms-1; j>=0; --j) {
        sum = ratios[j] * ( 1 + sum );
    }
    return sum;
}

void consecutive_novel_ratios( uint32_t terms )
{
    Cumulative sum;

    // Initializing last to 1 is equivalent to indicating the entire set of integers
    mpz_class last_numer = 1;
    mpz_class last_denom = 1;

    // Gather the first entry to compare with the set of all integers
    mpz_class next_numer = sum.novel();
    mpz_class next_denom = sum.denominator();

    // Initialize these so the ratios is not undefined and zero
    mpz_class ratio_num  = 0;
    mpz_class ratio_den  = 1;

    // Initialize the multiple precision ratio (0 to 1) to 0
    mpf_class ratio = 0;

    // File to write novel convergence ratios to
    FILE *fptr = fopen("novel_ratios.txt", "w");

    // Iterate over the entire range of n given
    for ( uint32_t n; n<=terms; ++n )
    {
        char buffer[32];

        // A blip every 100 terms
        if ( !(n % 100) ) {
            printf("n = %d\n", n);
        }

        // First compute the ratio numerator and denominator in absolute precision
        ratio_num = next_numer * last_denom;
        ratio_den = next_denom * last_numer;

        // Calculate the ratio as precisely as possible
        ratio = mpf_class(ratio_num) / mpf_class(ratio_den);

        // Write ratio to buffer for a given value of n, and then write result to file
        gmp_sprintf(buffer, "%d,%9.7Ff\n", n, ratio);
        fprintf(fptr, buffer);

        // Save the last values for next iteration
        last_numer = next_numer;
        last_denom = next_denom;

        // Increment to the next values
        ++sum;

        // Store the next numerator and denominator
        next_numer = sum.novel();
        next_denom = sum.denominator();
    }

    // Close the file
    fclose(fptr);
}

namespace fs = std::filesystem;             // Create an alias for the filesystem namespace

/**
 * @brief The main() entry point is used to call menu() and also for testing components \b before calling menu().
 * @details Everything needs a starting point
 * @return int - Executable currently does not return any values to caller.
 */

int main(int argc, char* argv[])
{
    bool stop_in_main = true;

    fs::path outdir = ".";

    // If an output directory is given as an argument, attempt to create it and use it for output. Otherwise, use the current directory.
    if (argc > 1) {
        outdir = argv[1];
        if (!fs::create_directories(outdir) && !fs::exists(outdir)) {
            std::cerr << "Cannot create output directory: " << outdir << "\n";
            return 1;
        }
    }

    A100982 rangecheck;
    rangecheck[7];
    int size = rangecheck.size();

    // Size of the arrays holding the position in the 41 or 53 cycles
    uint32_t asize = 800;
    uint32_t csize = 100000;

    A022921 test;

    // In general 26-term over previous 26-term is less than 0.25 = (3/8)/(3/2), but this doesn't matter
    // for ( int i=26; i<=26; ++i ) {
    //     char filename[40];
    //     sprintf(filename,"group_ratios_%d.txt",i);
    //     FILE *fptr = fopen(filename, "w");
    //     printf("%d term consecutive group ratios\n", i);
    //     novel_sum_4(i, asize, fptr);  // start with 2 and for now ignore other parameters
    //     fclose(fptr);
    // }

    // Compute the consecutive ratios of N(n) up to argument given
    // consecutive_novel_ratios(80000);

    // // How many digits in denominator when n=80000?
    // Cumulative tester( asize );
    // mpz_class testval;
    // testval = tester.denominator();

    // int i=0;
    // do {
    //     testval /= 10;      // Power of ten
    //     ++i;
    // } while ( testval );

    // printf("Cumulative[%d] has a %d digit denominator\n", asize, i);

// this approach sucks - takes forever...
// void novel_sum(int start, int terms, mpz_class& numer, mpz_class& denom)
// double find_ratio(mpz_class n1, mpz_class d1, mpz_class n2, mpz_class d2)
    mpz_class n1, n2, d1, d2;
    // novel_sum( 79800, 13, n1, d1 );
    // novel_sum( 79800, 26, n2, d2 );
    // double ratio = find_ratio( n1, d1, n2, d2 );
    // novel_sum(0,13,n1,d1);

    // Signed integer arrays which hold up to n=100,000 to a position in a 53 or 41 cycle.
    int8_t cycle_elem_41[csize];
    int8_t cycle_elem_53[csize];

    // Init each array to -1 which is used to indicate whether the value found is in 41 or 53 cycle
    std::fill( cycle_elem_41, cycle_elem_41+asize, -1 );
    std::fill( cycle_elem_53, cycle_elem_41+asize, -1 );

    // This computes all the non-linear cycles up to the first three 31,867 cycles (95601)
    for ( int i = 0; i<1; ++i )         // The number of large (31867) cycles to find
    // for ( int i = 0; i<3; ++i )
    {
        if ( !found_cycle( test, cycle_elem_41, cycle_elem_53 ) ) {
            printf("Not a known cycle !!!\n\n");
            break;
        }
    }

    for ( int i=11; i<=15; ++i ) {
        char filename[40];
        sprintf(filename,"group_ratios_%d.txt",i);
        FILE *fptr = fopen(filename, "w");
        printf("%d term consecutive group ratios\n", i);
        // novel_sum_3(0,i,n1,d1);  // start with 2 and for now ignore other parameters
        // novel_sum_3(i, asize, fptr);  // start with 2 and for now ignore other parameters
        novel_sum_4(i, asize, fptr, cycle_elem_41, cycle_elem_53);  // start with 2 and for now ignore other parameters
        fclose(fptr);
    }

    // for (int i = 11; i <= 15; ++i)
    // {
        // fs::path filename = outdir / ("group_ratios_" + std::to_string(i) + ".txt");

    //     std::ofstream fout(filename);

    //     if (!fout)
    //     {
    //         std::cerr << "Cannot open " << filename << "\n";
    //         continue;
    //     }

    //     std::cout << i << " term consecutive group ratios\n";

    //     novel_sum_4(i, asize, fout, cycle_elem_41, cycle_elem_53);
    // }

    // eleven_or_twelve();

    // The following determines if 11 terms is sufficient to cover the next 2^{-k} interval
    // capped(1,0,1);
    // capped(2,1,1);
    // capped(3,3,3);
    // capped(4,6,4);

    capped(26,199,11);
    capped(27,210,11);
    capped(28,222,11);
    capped(29,233,11);
    capped(30,245,11);

    uint32_t seqno[200];
    double   value[200];

    // Number of elements in Cumulative series to calculate
    uint32_t max = 80000;

    Cumulative c,d;
    // Cumulative_seq2( &c, 100 );
    // Cumulative_seq3( &d, 80000 );
    // Cumulative_seq3( &d, 79999 );
    // Cumulative_seq3( &d, max-1 );
    Cumulative_seq4( &d, max-1, cycle_elem_41, cycle_elem_53 );

    // // Write ratios out to file
    // FILE *fptr = fopen("last200.txt", "w");
    // printf("Writing ratio of last 200 to file.\n");
    // for (int i=0; i<200; ++i) {
    //     fprintf(fptr, "n = %05d, ratio = %9.7f\n", seqno[i], value[i]);
    // }

    long integer = -321;        // A very cool negative integer starting point

    mpz_class an(1), last_an(1);
    mpz_class numer = 0, denom = 1, last_denom = 1;
    // int neg = -1;
    // path broken( 13, neg );

    A020914 exp_of_2;
    A000079 power_of_2;
    A186009 a186009;

    // Generate the denominators an, d numerators for C(n)
    // for ( int n = 1; n <= 40; n++ )
    // {
    //     last_an = an;
    //     last_denom = denom;

    //     an = a186009.term();

    //     mpf_class ratio = mpf_class(an)/mpf_class(last_an);
    //     denom = power_of_2[ exp_of_2() ];
    //     numer *= denom / last_denom;
    //     numer += an;
    //     mpf_class error = 1.0 - mpf_class(numer)/ mpf_class(denom);

    //     // Now try to replicate the output of the elements
    //     std::vector< mpz_class > copy = a186009.elements();

    //     // Output the term details
    //     std::cout << "n = " << a186009.index() << ", j = " << copy.size() << ", a(n) = " << an;
    //     std::cout << ", a(n)/a(n-1) = " << ratio;
    //     std::cout << ", exponent = " << exp_of_2 << ", non-convergent = " << error;
    //     std::cout << "; numerator = " << numer << "; denominator = " << denom << std::endl;
        
    //     // Spew out the generating vector
    //     for ( std::vector< mpz_class >::const_iterator iter = copy.cbegin(); iter != copy.cend(); ++iter )
    //     {
    //         std::cout << *iter << " ";
    //     }

    //     std::cout << std::endl;

    //     // Move to next terms in sequences
    //     ++a186009;
    //     ++exp_of_2;
    // }

    // Display the selection menu.  Once you return from this you're done.
    // menu();

    // That's it.
    printf("all done.\n");
}

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
