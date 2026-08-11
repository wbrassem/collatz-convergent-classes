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
#include <iomanip>  // for std::setprecision, std::fixed, std::setw
#include <cmath>     // for std::log
#include <set>
#include <unordered_map>

#include "common.hpp"
#include "format.hpp"
#include "oeis.hpp"
#include "path.hpp"

// Global debug flag, set by command line argument
bool debug = false;

// Global debug output stream, can be set to a file if needed
std::ostream* debug_stream = &std::cout;  // default to console

// Debug output function that only prints when debug mode is enabled
template<typename... Args>
inline void debug_out(Args&&... args) {
    if (debug) {
        (*debug_stream << ... << std::forward<Args>(args)) << '\n';
    }
}

// Macro which can be used for debug output, will only print if debug mode is enabled
#define DEBUG_OUT(...) \
    debug_out("[", __func__, ":", __LINE__, "] ", __VA_ARGS__)
    
namespace cycle_output
{
    int max_depth = 1;          // Default to top level cycle output only
    std::ofstream cycle_fout;    // Output file stream for cycle output
    // ostream &csv(ostream &os, auto&&... args) {
    //     // Fold expression to write each argument followed by a comma, except for the last one
    //     ((os << args << ","), ...);
    //     os << "\n"; // End the line after all arguments are written
    //     return os; // Return the stream to allow chaining
    // }

    //     // fs::path filename = outdir / ("group_ratios_" + std::to_string(i) + ".txt");

    //     // std::ofstream fout(filename);
    // fs::path cycle_filename = outdir / "cycle_output.csv";
    // std::ofstream cycle_fout(cycle_filename);

    // cycle_output::max_depth = 6;


    void cycle_print(int len, int index, int depth)
    {
        if (depth > max_depth) return;

        // // To std::cout with formatting
        // std::cout << std::setw(6) << index
        //           << std::string(depth*2+1,' ')
        //           << len << "-cycle"
        //           << "\n";

        // To file with csv formatting
        // std::cout << std::setw(6) << index
        //           << std::string(depth+1,',')
        //           << len << "-cycle"
        //           << "\n";
        cycle_fout << index
        // std::cout << index
                << "," << depth
                << "," << len
                << "\n";
    }
            // printf("Failure matching 359-cycle!!! Index = %d\n", copy.index());
    void cycle_error(int len, int index, int depth)
    {
        if (depth > max_depth) return;

        std::cout << std::string(depth*2,' ')
                  << "Failure matching a " << len << "-cycle"
                  << ". Index = " << index
                  << "\n";
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
 * @param [in] fout - A reference to an output stream to write results to.
 * @param [in] terms - The number of terms to summate.
 * @param [in] t - The maximum number of elements to evaluate.
 */
void novel_sum_4( std::ostream& fout, int terms, int t )
{
    // Step 1: check stream health immediately
    if (!fout) {
        std::cerr << "novel_sum_4: output stream not ready\n";
        return;
    }

    // If no terms needed then return immediately with the modified reference parameters
    if ( (terms < 1) || (terms > 15) )
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

// gmp_printf("novel_sum_4: n = %d, n%range = %d, numers[n%range] = %Zd, denoms[n%range] = %Zd\n", n, n%range, numers[n%range], denoms[n%range]);

        // If index is less that one group of terms, just store values literally
        if ( n < terms ) {
            differ[n%range] = cumulative.numerator();           // The modulo is used to wrap around the array for the next group of terms
        }

        // Otherwise there is at least one group of terms, so compute the differential
        else {
            power_of_2 = denoms[n%range] / denoms[(n-terms)%range];
            differ[n%range] = cumulative.numerator() - power_of_2 * numers[(n-terms)%range];    

// gmp_printf("novel_sum_4: n = %d, power_of_2 = %Zd, differ[n%range] = %Zd\n", n, power_of_2, differ[n%range]);

            // If the index has at least two complete groups you can begin to compute the ratio between groups
            if ( n >= range) {

                uint16_t curr_sum_index = (n-1) % range;
                uint16_t prev_sum_index = (n-terms-1) % range;

// gmp_printf("novel_sum_4: n = %d, curr_sum_index = %d, prev_sum_index = %d\n", n, curr_sum_index, prev_sum_index);

                mpz_class ratio_num = differ[curr_sum_index] * denoms[prev_sum_index];
                mpz_class ratio_den = differ[prev_sum_index] * denoms[curr_sum_index];

// gmp_printf("novel_sum_4: n = %d, differ[curr_sum_index] = %Zd, denoms[prev_sum_index] = %Zd\n", n, differ[curr_sum_index], denoms[prev_sum_index]);
// gmp_printf("novel_sum_4: n = %d, differ[prev_sum_index] = %Zd, denoms[curr_sum_index] = %Zd\n", n, differ[prev_sum_index], denoms[curr_sum_index]);
// gmp_printf("novel_sum_4: n = %d, ratio_num = %Zd, ratio_den = %Zd\n", n, ratio_num, ratio_den);
        
                // gmp_printf("terms = %d, n = %d: ratio = %Zd/%Zd = %9.7Ff\n", terms, n-range, ratio_num, ratio_den, ratio);
                mpf_class ratio = mpf_class(ratio_num) / mpf_class(ratio_den);

// gmp_printf("novel_sum_4: n = %d, ratio = %9.7Ff\n", n, ratio);

                // Output to console with formatting every 10 terms
                if ( !( (n-range) % 10) ) {
                    csv(fout,
                        n - range,          // ratio first appears at n = range
                        fmt::f(9,7,ratio)   // ratio of terms
                    );
                }

                // Blip
                if ( !( (n-range) % 100) ) {
                    gmp_printf("terms = %2d, n = %5d: ratio = %9.7Ff\n", terms, n-range, ratio);
                }
            }
        }

        // Increment the cumulative ratio from which all values are derived
        ++cumulative;
    }
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

double partial_threshold2(const mpz_class& numer, const mpz_class& denom, const mpz_class& threshold)
{
    mpz_class partial_numer = numer - threshold;
    mpz_class partial_denom = denom - threshold;
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

        // A blip every 200 terms
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
            fprintf(fptr, "\n%2d-term occurrences: %4d\n", i, 0);
        else
            fprintf(fptr, "\n%2d-term occurrences: %4d, range %8.6f to %8.6f\n", i, frequency[i], minimums[i], maximums[i]);
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

// This function find the number of terms of A186009 needed to cover the next 1/2^n interval
void Cumulative_seq5( std::ostream& fout, Cumulative    c, uint32_t t )
{
    mpz_class numer, denom, power, scale, threshold;
    mpz_class newt=1, oldt=1, next_denom;
    A000079 a000079(2);

    double new_interval=1, partial=1, last_partial=1, diff=1;
    uint32_t last_i=0, term=1, exponent=1, last_exponent=1;

    // Turn interval into an array of strings [1] to [14] + create a parallel array as a histogram of the frequency of term counts
    FILE *fptr = fopen("partial-pos.txt", "w");

    // Print out the first t elements of the sequence
    for ( uint32_t i=0; i<=t; ++i ) {
        numer = c.numerator();
        denom = c.denominator();
        // novel = c.novel();
        exponent = c.exponent();
        power = a000079();
        scale = denom / power;              // This evaluates to an integer power of 2
        threshold = (power-1) * scale;      // The value the numer must exceed in order to reach next bracket
// gmp_printf("n = %5d; numer = %Zd, denom = %Zd, novel = %Zd, threshold = %Zd, power = %Zd, scale = %Zd\n", i, numer, denom, novel, threshold, power, scale);
gmp_printf("n = %5d; numer = %Zd, denom = %Zd, exponent = %d, threshold = %Zd, power = %Zd, scale = %Zd\n", i, numer, denom, exponent, threshold, power, scale);


// New approach to calculating the next threshold is to look at the exponent and denominator of the next term, which gives a more accurate picture of
// where the next threshold will be than simply doubling the previous threshold.  This is because the terms are not always powers of 2, so the next term
// may not be exactly double the previous one.
        uint32_t next_exponent = c.exponent();
        mpz_class next_denom = c.denominator();
        // Take the midpoint between the old threshold and the denominator as the new threshold,
        // which is a more accurate reflection of the point at which the next term would be required.
        uint32_t pow = next_exponent - last_exponent;
        // mpz_class oldt = threshold << pow;
        // mpz_class newt = (next_denom + oldt) >> 1;
        // newt <<= pow;           // Scale the old threshold by the power difference to get the new threshold
gmp_printf("n = %5d; pow = %d, old threshold = %Zd, new threshold = %Zd, denom = %Zd, next_exponent = %d, last_exponent = %d, next_denom = %Zd\n", i, pow, oldt, newt, denom, next_exponent, last_exponent, next_denom);
        last_exponent = next_exponent;



        // A blip every 200 terms
        if ( !(i % 200) ) {
            printf("n = %d\n", i);
        }

        // Compute the fractional component to the next threshold
        partial = partial_threshold(numer, denom, threshold);

        // Compare cumulative to the power of two
        if ( numer >= threshold ) {
            c++;
            next_denom = c.denominator();
            next_exponent = c.exponent();
            pow = next_exponent - last_exponent;
            oldt <<= pow;
            newt = (next_denom + oldt) >> 1;
gmp_printf("n = %5d; pow = %d, old threshold = %Zd, new threshold = %Zd, denom = %Zd, next_exponent = %d, last_exponent = %d, next_denom = %Zd\n", i, pow, oldt, newt, denom, next_exponent, last_exponent, next_denom);


            // mpz_class oldthresh = threshold;
            // Calculate the amount the numerator has crossed into another 2^-k bracket
            mpz_class overshoot = numer - threshold;

            // Calculate the coverage of the 2^-k bracket
            diff = 1 - (last_partial - partial);

            // Debug output for the current term and the diff to the next threshold
            // DEBUG_OUT("n = ", fmt::d(5,i), 
            //           ", term = ", fmt::d(2,term), 
            //           ", partial = ", fmt::f(8,6,partial),
            //           "; last_partial = ", fmt::f(8,6,last_partial),
            //           ", diff = ", fmt::f(8,6,diff)
            // );

            // Calculate the new threshold
            threshold = (power*2-1) * scale;
            // mpz_class partial_thresh = threshold - oldthresh;

            // Compute the fractional component to the next threshold
            new_interval = partial_threshold(numer*2, denom*2, threshold);

             // Generate the tuple
            csv(fout, 
                i,
                term,
                fmt::e(8,6,overshoot),
                // fmt::e(8,6,partial_thresh),
                fmt::f(8,6,diff),
                fmt::f(8,6,new_interval)
            );

            // Debug output for the new threshold and the new interval to the next threshold.  Overshoot is the amount by which the
            // current numerator exceeds the threshold, which is a measure of how far into the new bracket we are.
            DEBUG_OUT("n = ", fmt::d(5,i), 
                      ", term = ", fmt::d(2,term), 
                      ", partial = ", fmt::f(8,6,partial),
                      "; last_partial = ", fmt::f(8,6,last_partial),
                      "; numer = ", fmt::e(8,6,numer),
                      ", denom = ", fmt::e(8,6,denom),
                      ", threshold = ", fmt::e(8,6,threshold),
                      ", overshoot = ", fmt::e(8,6,overshoot),
                      ", diff = ", fmt::f(8,6,diff),
                      ", power = ", fmt::e(8,6,power),
                      ", scale = ", fmt::e(8,6,scale),
                      ", new_interval = ", fmt::f(8,6,new_interval)
            );

            // Store the starting index position for the next interval
            last_i = i;

            // Reset the term value to 1 prior to printing
            term = 1;

            // Save the highest percentage achieved from previous bracket
            last_partial = partial;
oldt = newt;
            // Increment the power of 2
            ++a000079;
        }
        // Otherwise it's just another term in the bracket
        else {
            c++;
            ++term;
            next_exponent = c.exponent();
            pow = next_exponent - last_exponent;
        newt <<= pow;           // Scale the old threshold by the power difference to get the new threshold
gmp_printf("n = %5d; pow = %d, old threshold = %Zd, new threshold = %Zd, denom = %Zd, next_exponent = %d, last_exponent = %d, next_denom = %Zd\n", i, pow, oldt, newt, denom, next_exponent, last_exponent, next_denom);
oldt = newt;

            // Output the additional term and the partial threshold
            // fprintf(fptr, "n = %5d, term = %2d, partial = %8.6f \n", i, term, partial);
            // fout    << "n = " << fmt::d(5,i)
            //         << ", term = " << fmt::d(2,term)
            //         << ", partial = " << fmt::f(8,6,partial)
            //         << std::endl;
            DEBUG_OUT("n = ", fmt::d(5,i), 
                      ", term = ", fmt::d(2,term), 
                      ", partial = ", fmt::f(8,6,partial)
            );
        }

        // Increment to the next cumulative summation
        // c.operator++();
    }

    // Close the file
    fclose(fptr);
}

// This function find the number of terms of A186009 needed to cover the next 1/2^n interval
// This version is the same as seq5 but going to carve out the unnecessary calculations.
void Cumulative_seq6( std::ostream& fout_py, std::ostream& fout_rd, uint32_t t )
{
    Cumulative c;

    // Generate the header for the output
    if ( fout_py ) {
        csv(fout_py, 
            "n",
            "term",
            "coverage",
            "overshoot"
        );
    }

    // mpz_class numer=c.numerator(), denom=c.denominator();
    mpz_class threshold=1, last_threshold=0;

    // double new_interval=1, partial=1, last_partial=1, diff=1;
    uint32_t term=1, exponent=c.exponent(), last_exponent=exponent;

    // Print out the first t elements of the sequence
    for ( uint32_t n=0; n<=t; ++n ) {
        mpz_class numer = c.numerator();
        mpz_class denom = c.denominator();

        // The bit shift is the difference between power of 2 exponents
        uint32_t shift = exponent - last_exponent;

        // save the last exponent for the next iteration
        last_exponent = exponent;

        DEBUG_OUT("n = ", fmt::d(3,n), 
                ", term = ", fmt::d(2,term), 
                ", numer = ", fmt::e(numer), 
                ", denom = ", fmt::e(denom), 
                ", threshold = ", fmt::e(threshold),
                ", last_threshold = ", fmt::e(last_threshold),
                ", exponent = ", fmt::d(3,exponent), 
                ", last_exponent = ", fmt::d(3,last_exponent)
        );

        // A blip every 200 terms
        if ( !(n % 200) ) {
            printf("n = %d\n", n);
        }

        // Compare cumulative to the power of two
        if ( numer >= threshold ) {
            // Calculate the amount the numerator has crossed into another 2^-k bracket
            mpz_class overshoot = numer - threshold;
            // mpq_class ratio(overshoot, denom - threshold);
            mpq_class ratio(numer - last_threshold, denom - threshold);

            // Generate the tuple
            if ( fout_py ) {
                csv(fout_py, 
                    n,
                    term,
                    fmt::f(8,6,ratio.get_d()),
                    fmt::e(overshoot)
                );
            }

            // gmp_fprintf(fptr, "n = %5d, term = %2d, coverage = %8.6f, overshoot = %Fe\n", n, term, ratio.get_d(), mpf_class(overshoot));
            if ( fout_rd ) {
                fout_rd << "n = " << fmt::d(5,n) 
                        << ", term = " << fmt::d(2,term) 
                        << ", coverage = " << fmt::f(8,6,ratio.get_d()) 
                        << ", overshoot = " << fmt::e(overshoot) 
                        << std::endl;
            }

            // Go to next cumulative term
            c++;

            // Retrieve the exponent and calculate the power difference for the next term
            exponent = c.exponent();
            shift = exponent - last_exponent;

            // Update the numerator, denominator and threshold for the next term
            numer = c.numerator();
            denom = c.denominator();
            threshold <<= shift;
            last_threshold = threshold;
            threshold = (denom + threshold) >> 1;      // Find the midpoint between the old threshold and the denominator

            // Debug output for the new threshold and the new interval to the next threshold.  Overshoot is the amount by which the
            DEBUG_OUT("n = ", fmt::d(3,n), 
                    ", term = ", fmt::d(2,term), 
                    ", numer = ", fmt::e(numer), 
                    ", denom = ", fmt::e(denom), 
                    ", threshold = ", fmt::e(threshold),
                    ", last_threshold = ", fmt::e(last_threshold),
                    ", exponent = ", fmt::d(3,exponent), 
                    ", last_exponent = ", fmt::d(3,last_exponent),
                    ", shift = ", fmt::d(shift), 
                    ", partial = ", fmt::f(8,6,ratio.get_d()),
                    ", overshoot = ", fmt::e(overshoot)
            );

            // Reset the term value to 1 prior to printing
            term = 1;
        }

        // Otherwise it's just another term in the bracket
        else {
            // Calculate the amount the numerator falls short of the 2^-k bracket
            mpz_class gap = threshold - numer;
            mpz_class overshoot = numer - last_threshold;
            mpq_class ratio(overshoot, denom - threshold);

            // Generate the tuple
            if ( fout_py ) {
                csv(fout_py, 
                    n,
                    term,
                    fmt::f(8,6,ratio.get_d()),
                    fmt::e(overshoot)
                );
            }

            // Go to next cumulative term
            c++;

            // Retrieve the exponent and calculate the power difference for the next term
            exponent = c.exponent();
            shift = exponent - last_exponent;

            // Update the numerator, denominator and threshold for the next term
            ++term;
            threshold <<= shift;           // Scale the old threshold by the shift difference to get the new threshold
            last_threshold <<= shift;           // Scale the old threshold by the shift difference to get the new threshold

            gap <<= shift;                 // Scale the gap by the shift difference to get the new gap
            numer <<= shift;              // Scale the numerator by the shift difference to get the new numerator
            denom <<= shift;              // Scale the denominator by the shift difference to get the new denominator

            DEBUG_OUT("n = ", fmt::d(3,n), 
                    ", term = ", fmt::d(2,term), 
                    ", numer = ", fmt::e(numer), 
                    ", denom = ", fmt::e(denom), 
                    ", threshold = ", fmt::e(threshold),
                    ", last_threshold = ", fmt::e(last_threshold),
                    ", exponent = ", fmt::d(3,exponent), 
                    ", last_exponent = ", fmt::d(3,last_exponent),
                    ", shift = ", fmt::d(shift), 
                    ", partial = ", fmt::f(8,6,ratio.get_d()),
                    ", gap = ", fmt::e(gap)
            );
        }
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

bool found_41_53_cycle( A022921& a022921, uint16_t subcycles, int depth )
{
    A022921 copy = a022921;

    // Cycle through the first 12-cycles and return if not matching
    for ( uint8_t i = 0; i<subcycles; ++i ) {
        if ( !found_12_cycle(copy) ) {
            cycle_output::cycle_error(12, copy.index(), depth);
            a022921 = copy;
            return false;
        }
    }

    if ( !found_5_cycle(copy) ) {
        cycle_output::cycle_error(5, copy.index(), depth);
        a022921 = copy;
        return false;
    }

    a022921 = copy;
    return true;
}

// bool found_41_cycle( A022921& a022921, int8_t *cycle_elem, int depth )
bool found_41_cycle( A022921& a022921, int depth )
{
    int subcycles = 3;         // There are 3 12-cycles in a 41-cycle
    A022921 copy = a022921;

    // Cycle through the first 3 12-cycles and return if not matching
    if ( !found_41_53_cycle( copy, subcycles, depth+1 ) ) {
        cycle_output::cycle_error(41, copy.index(), depth);
        return false;
    }

    // Load array with 41 elements from 0 to 41
    // load_array( cycle_elem, copy.index(), 41 );

    cycle_output::cycle_print(41, copy.index(), depth);
    a022921 = copy;
    return true;
}

// bool found_53_cycle( A022921& a022921, int8_t *cycle_elem, int depth )
bool found_53_cycle( A022921& a022921, int depth )
{
    int subcycles = 4;         // There are 4 12-cycles in a 53-cycle
    A022921 copy = a022921;

    // Cycle through the first 4 12-cycles and return if not matching
    if ( !found_41_53_cycle( copy, subcycles, depth+1 ) ) {
        cycle_output::cycle_error(53, copy.index(), depth);
        return false;
    }

    // Load array with 53 elements from 0 to 52
    // load_array( cycle_elem, copy.index(), 53 );

    cycle_output::cycle_print(53, copy.index(), depth);
    a022921 = copy;
    return true;
}

// bool found_306_359_cycle( A022921& a022921, uint16_t subcycles, int8_t *cycle_elem_41, int8_t *cycle_elem_53, int depth )
bool found_306_359_cycle( A022921& a022921, uint16_t subcycles, int depth )
{
    A022921 copy = a022921;

    // Cycle through the first 53-cycles and return if not matching
    for ( uint8_t i = 0; i<subcycles; ++i ) {
        // if ( !found_53_cycle(copy, cycle_elem_53, depth+1) ) {
        if ( !found_53_cycle(copy, depth+1) ) {
            cycle_output::cycle_error(665, copy.index(), depth);
            a022921 = copy;
            return false;
        }
    }

    // if ( !found_41_cycle(copy, cycle_elem_41, depth+1) ) {
    if ( !found_41_cycle(copy, depth+1) ) {
        cycle_output::cycle_error(41, copy.index(), depth);
        a022921 = copy;
        return false;
    }

    a022921 = copy;
    return true;
}

// bool found_306_cycle( A022921& a022921, int8_t *cycle_elem_41, int8_t *cycle_elem_53, int depth )
bool found_306_cycle( A022921& a022921, int depth )
{
    int subcycles = 5;         // There are 5 53-cycles in a 306-cycle plus an additional 41-cycle
    A022921 copy = a022921;

    // Cycle through the first 5 53-cycles and return if not matching
    // if ( !found_306_359_cycle( copy, subcycles, cycle_elem_41, cycle_elem_53, depth ) ) {
    if ( !found_306_359_cycle( copy, subcycles, depth ) ) {
        a022921 = copy;
        return false;
    }

    cycle_output::cycle_print(306, copy.index(), depth);
    a022921 = copy;
    return true;
}

// bool found_359_cycle( A022921& a022921, int8_t *cycle_elem_41, int8_t *cycle_elem_53, int depth )
bool found_359_cycle( A022921& a022921, int depth )
{
    A022921 copy = a022921;
    int subcycles = 6;         // There are 6 53-cycles in a 359-cycle plus an additional 41-cycle

    // Cycle through the first 6 53-cycles and return if not matching
    // if ( !found_306_359_cycle( copy, subcycles, cycle_elem_41, cycle_elem_53, depth ) ) {
    if ( !found_306_359_cycle( copy, subcycles, depth ) ) {
        a022921 = copy;
    }

    cycle_output::cycle_print(359, copy.index(), depth);
    a022921 = copy;
    return true;
}

// bool found_665_cycle( A022921& a022921,  int8_t *cycle_elem_41, int8_t *cycle_elem_53, int depth )
bool found_665_cycle( A022921& a022921, int depth )
{
    A022921 copy = a022921;

    // Cycle through a 665-cycle and return if not matching
    // if ( found_359_cycle(copy, cycle_elem_41, cycle_elem_53, depth+1) && found_306_cycle(copy, cycle_elem_41, cycle_elem_53, depth+1) ) {
    // if ( found_359_cycle(copy, depth+1) && found_306_cycle(copy, cycle_elem_41, cycle_elem_53, depth+1) ) {
    if ( found_359_cycle(copy, depth+1) && found_306_cycle(copy, depth+1) ) {
        cycle_output::cycle_print(665, copy.index(), depth);
        a022921 = copy;
        return true;
    }

    a022921 = copy;
    return false;
}

// bool found_15601_16266_cycle( A022921& a022921, uint16_t subcycles,  int8_t *cycle_elem_41, int8_t *cycle_elem_53, int depth )
bool found_15601_16266_cycle( A022921& a022921, uint16_t subcycles, int depth )
{
    A022921 copy = a022921;

    // Cycle through the first 665-cycles and return if not matching
    for ( uint8_t i = 0; i<subcycles; ++i ) {
        // if ( !found_665_cycle(copy, cycle_elem_41, cycle_elem_53, depth+1 ) ) {
        if ( !found_665_cycle(copy, depth+1 ) ) {
            cycle_output::cycle_error(665, copy.index(), depth);
            a022921 = copy;
            return false;
        }
    }

    // if ( !found_306_cycle(copy, cycle_elem_41, cycle_elem_53, depth+1 ) ) {
    if ( !found_306_cycle(copy, depth+1 ) ) {
        cycle_output::cycle_error(306, copy.index(), depth);
        a022921 = copy;
        return false;
    }

    a022921 = copy;
    return true;
}

// bool found_15601_cycle( A022921& a022921, int8_t *cycle_elem_41, int8_t *cycle_elem_53, int depth )
bool found_15601_cycle( A022921& a022921, int depth )
{
    A022921 copy = a022921;
    int subcycles = 23;         // There are 23 665-cycles in a 15601-cycle

    // Cycle through the first 23 665-cycles and return if not matching
    // if ( !found_15601_16266_cycle( copy, subcycles, cycle_elem_41, cycle_elem_53, depth ) ) {
    if ( !found_15601_16266_cycle( copy, subcycles, depth ) ) {
        cycle_output::cycle_error(15601, copy.index(), depth);
        a022921 = copy;
        return false;
    }

    cycle_output::cycle_print(15601, copy.index(), depth);
    a022921 = copy;
    return true;
}

// bool found_16266_cycle( A022921& a022921,int8_t *cycle_elem_41, int8_t *cycle_elem_53, int depth )
bool found_16266_cycle( A022921& a022921, int depth )
{
    A022921 copy = a022921;
    int subcycles = 24;         // There are 24 665-cycles in a 16266-cycle

    // Cycle through the first 24 665-cycles and return if not matching
    // if ( !found_15601_16266_cycle( copy, subcycles, cycle_elem_41, cycle_elem_53, depth ) ) {
    if ( !found_15601_16266_cycle( copy, subcycles, depth ) ) {
        cycle_output::cycle_error(16266, copy.index(), depth);
        a022921 = copy;
        return false;
    }

    cycle_output::cycle_print(16266, copy.index(), depth);
    a022921 = copy;
    return true;
}

// bool found_cycle( A022921& a022921, int8_t *cycle_elem_41, int8_t *cycle_elem_53 )
bool found_cycle( A022921& a022921 )
{
    A022921 copy = a022921;
    int depth = 0;

    // Cycle through a 31867-cycles and return if not matching
    // if ( found_16266_cycle(copy, cycle_elem_41, cycle_elem_53, depth+1) && found_15601_cycle(copy, cycle_elem_41, cycle_elem_53, depth+1) ) {
    // if ( found_16266_cycle(copy, depth+1) && found_15601_cycle(copy, cycle_elem_41, cycle_elem_53, depth+1) ) {
    if ( found_16266_cycle(copy, depth+1) && found_15601_cycle(copy, depth+1) ) {
        cycle_output::cycle_print(31867, copy.index(), depth);
        a022921 = copy;
        return true;
    }

    cycle_output::cycle_error(31867, copy.index(), depth);
    a022921 = copy;
    return false;
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
 * @brief Calculate the logarithm of a large integer n using its mantissa and exponent in base 2.
 * @param n The large integer for which to calculate the logarithm.
 * @return double The logarithm of n in base 2.
 */
double mpz_log(const mpz_class& n)
{
    mp_exp_t exp;

    // n = mantissa * 2^exp
    double mantissa = mpz_get_d_2exp(&exp, n.get_mpz_t());

    return std::log(mantissa) + exp * std::log(2.0);
}

void gamma()
{
        // Test
    mpz_class test = 1024;  // 2^10
    double log_test = mpz_log(test);
    std::cout << "Log of 2^10: " << log_test << std::endl;

    // uint32_t max = 80000;           // Default maximum number of terms to calculate for cumulative sequence
    // fs::path outdir = ".";
    int start = 15500, terms = 200; 

    A186009 a186009(start+1);         // Set index to starting point for testing
    A020914 a020914(start);         // Set index to starting point for testing

    int longest_cycle = 53;
    double mpz_log_terms[terms + longest_cycle];
    double mpf_numer_exps[terms + longest_cycle];
    double std_log_two = std::log(2.0);         // ~0.69314718056

    // First built out an array of gamma values (+ more than called for as per longest_cycle)
    for (int i=0; i<terms+longest_cycle; ++i) {
        mpz_log_terms[i] =  mpz_log(a186009.term());
        mpf_numer_exps[i] = a020914.term().get_d();
        a186009++;
        a020914++;
    }

    // Loop starting from starting index for a given number of terms
    for (int i=0; i<terms; ++i) {
        double gamma05 = ( mpz_log_terms[i+05] - mpz_log_terms[i] ) /  5.0 - ( mpf_numer_exps[i+05] - mpf_numer_exps[i] ) * std_log_two /  5.0;
        double gamma12 = ( mpz_log_terms[i+12] - mpz_log_terms[i] ) / 12.0 - ( mpf_numer_exps[i+12] - mpf_numer_exps[i] ) * std_log_two / 12.0;
        double gamma53 = ( mpz_log_terms[i+53] - mpz_log_terms[i] ) / 53.0 - ( mpf_numer_exps[i+53] - mpf_numer_exps[i] ) * std_log_two / 53.0;
        std::cout << "Index: " << start+i << ", Gamma(5): " << fmt::f(8,6,gamma05) << ", Gamma(12): " << fmt::f(8,6,gamma12) << ", Gamma(53): " << fmt::f(8,6,gamma53) << std::endl;
    }
}

void differential()
{
    // int expansive[]   = { 5, 41, 306, 15601,  79335};       // The expansive   cycle lengths to calculate gamma for
    // int contractive[] = {12, 53, 665, 31867, 111202};       // The contractive cycle lengths to calculate gamma for
    int towers[] = { 2, 5, 12, 41, 53, 306, 665, 15601, 31867, 79335, 111202 };   // The tower indices to calculate gamma for
    // int exp_start[sizeof(expansive)/sizeof(expansive[0])];  // Array to hold the starting indices for expansive cycle lengths
    int a186009_doubling_offset = 2;                        // The offset to apply to the index when calculating gamma
    int tower = towers[sizeof(towers)/sizeof(towers[0])-1] + a186009_doubling_offset;            // The index of the tower to calculate gamma up to

    A186009 a186009;                        // Set index to default starting point for testing

    // Build out array of expanive cycle starting indices based on the contractive cycle lengths.
    // for (size_t j=0; j<sizeof(expansive)/sizeof(expansive[0]); ++j) {
    //     exp_start[j] = contractive[j]-expansive[j];     // Calculate the starting index for each expansive cycle length based on the corresponding contractive cycle length and the offset
    // }

    double last_log_tower = 0;                      // Variable to hold the logarithm of the last tower value for calculating gamma
    int last_cycle_index = 1;                      // Variable to hold the last cycle index for calculating gamma

    // The series A186009(n) begins at n=1, with a doubling rule offest by n-2
    for (int i=1; i<=tower; ++i) {

        // Search for contractive cycle terminations and calculate gamma.
        if (std::find(std::begin(towers), std::end(towers), i-a186009_doubling_offset) != std::end(towers)) {
            int cycle_index = i - a186009_doubling_offset;
            int tower_delta = cycle_index - last_cycle_index;
            double log_tower = mpz_log(a186009.term());
            double gamma = ( log_tower - last_log_tower ) / ( cycle_index - last_cycle_index );
            std::cout << "Calculating gamma(" << last_cycle_index << "->" << cycle_index << ") with tower separation " << tower_delta << ": " << fmt::f(8,6,gamma) << std::endl;

            last_log_tower = log_tower;
            last_cycle_index = cycle_index;
        }

        // // Search for contractive cycle terminations and calculate gamma.
        // if (std::find(std::begin(contractive), std::end(contractive), i-a186009_doubling_offset) != std::end(contractive)) {
        //     int cycle_index = i - a186009_doubling_offset;
        //     double log_tower = mpz_log(a186009.term());
        //     double gamma = log_tower /  cycle_index;
        //     std::cout << "Calculating gamma for contractive cycle length: " << cycle_index << ", gamma: " << fmt::f(8,6,gamma) << std::endl;
        // }

        // // Search for expansive cycle beginnings and calculate starting gamma.
        // if (std::find(std::begin(expansive), std::end(expansive), i-a186009_doubling_offset) != std::end(expansive)) {
        //     int cycle_index = i - a186009_doubling_offset;
        //     double log_tower = mpz_log(a186009.term());
        //     double gamma = log_tower /  cycle_index;
        //     std::cout << "Calculating gamma for expansive cycle length: " << cycle_index << ", gamma: " << fmt::f(8,6,gamma) << std::endl;
        // }        ++a186009;      // Increment to the next term in A186009 for the next iteration

        // // Search for expansive cycle terminations and calculate gamma.
        // if (std::find(std::begin(expansive), std::end(expansive), i-a186009_doubling_offset) != std::end(expansive)) {
        //     int cycle_index = i - a186009_doubling_offset;
        //     double log_tower = mpz_log(a186009.term());
        //     double gamma = log_tower /  cycle_index;
        //     std::cout << "Calculating gamma for expansive cycle length: " << cycle_index << ", gamma: " << fmt::f(8,6,gamma) << std::endl;
        // }
        
        ++a186009;      // Increment to the next term in A186009 for the next iteration
    }
}

double calculate_gamma( double log_tower, double last_log_tower, int cycle_index, int last_cycle_index )
{
    return ( log_tower - last_log_tower ) / ( cycle_index - last_cycle_index );
}

void tower_point_cocycle_growth(std::ofstream& A186009_out)
{
    int expansive[]   = {1, 5,  41, 306, 15601, 79335};     // The expansive   cycle lengths to calculate gamma for
    int contractive[] = {2, 12, 53, 665, 31867, 111202};    // The contractive cycle lengths to calculate gamma for
    int a186009_doubling_offset = 2;                        // The offset to apply to the index when calculating gamma
    int last_tower = 143500;                                // The index of the tower to calculate A186009 up to

    A186009 a186009;                        // Set index to default starting point for testing
    std::set<int> a186009_co_cycle_indices;
    std::unordered_map<int,double> log_a186009_map;

    // Initialize set with the value of 0 to represent the starting point of the cycles
    a186009_co_cycle_indices.insert(0);

    // Test out the new getters
    for ( int tower_index : contractive ) {
        mpf_class twos, threes, fidelity;
        A020914 a020914( tower_index );
        twos =  a020914.get_twos() * 2;
        threes = a020914.get_threes() * 3;
        fidelity = twos / threes;
        std::cout << "Tower: " << tower_index << "; Twos = " << fmt::e(10,8,twos) << ", Threes = " << fmt::e(10,8,threes) << ", Fidelity = " << fmt::f(10,8,fidelity) << std::endl;
    }

    // The first iteration of this loop will be to find the cycle start and end points, but not calculate gamma.
    for ( int tower_index : contractive ) {

        // Add the contractive cycle termination point to the set of indices to calculate gamma for
        a186009_co_cycle_indices.insert(tower_index);

        std::cout << "Contractive cycle termination at tower index: " << tower_index << ", (0," << tower_index << ")" << std::endl;

        for ( int termination : expansive ) {
            int exp_cycle_index = tower_index - termination;
            if ( exp_cycle_index >= 0 ) {
                std::cout << "    Expansive cycle length: " << termination << ", (" << exp_cycle_index << "," << tower_index << ")" << std::endl;

                // Add the expansive cycle initiation point to the set of indices to calculate gamma for
                a186009_co_cycle_indices.insert(exp_cycle_index);
            }
        }

        for ( int initiation : contractive ) {
            int exp_cycle_index = tower_index + initiation;
            if ( exp_cycle_index <= 2 * tower_index) {
                std::cout << "    Contractive cycle length: " << initiation << ", (" << tower_index << "," << exp_cycle_index << ")" << std::endl;

                // Add the contractive cycle initiation point to the set of indices to calculate gamma for
                a186009_co_cycle_indices.insert(exp_cycle_index);
            }
        }
    }

    // The second iteration will be to calculate the log of the tower values at the critcal points and tabulate.
    for ( int a186009_index = 1; a186009_index <= last_tower; ++a186009_index ) {

        // Align the tower index with the A186009 index by applying the doubling offset
        int tower_index = a186009_index - a186009_doubling_offset;

        // Check if the current index is in the set of indices to calculate gamma for, and if so,
        // calculate the log of the tower value and store it in the map
        if (a186009_co_cycle_indices.find(tower_index) != a186009_co_cycle_indices.end()) {
            log_a186009_map[tower_index] = mpz_log(a186009.term());
            std::cout << "Tower index: " << fmt::d(6,tower_index) << ", A186009 term: " << fmt::e(a186009.term())
                    << ", log: " << fmt::f(10,8,log_a186009_map[tower_index]) << std::endl;
        }

        A186009_out << fmt::d(a186009_index) << "," <<fmt::d(tower_index) << "," << fmt::e(a186009.term()) << "," << fmt::f(10,8,mpz_log(a186009.term())) << std::endl;
        ++a186009;      // Increment to the next term in A186009 for the next iteration
    }

    // The final iteration will be to calculate gamma for the cycle lengths.
    for ( int tower_index : contractive ) {

        double co_cycle_log;

        // If the tower index is in the map, calculate the log of the tower value at the cycle termination point
        if ( log_a186009_map.contains(tower_index) ) {
            co_cycle_log = log_a186009_map[tower_index] / tower_index;
            std::cout << "Contractive cycle termination at tower index: " << tower_index << "; range(0," << tower_index << ") = " << co_cycle_log << std::endl;
        }

        // Iterate through the expansive cycle lengths and calculate gamma terminating at the tower index
        for ( int termination : expansive ) {
            int exp_cycle_index = tower_index - termination;
            if ( exp_cycle_index >= 0 && log_a186009_map.contains(tower_index) && log_a186009_map.contains(exp_cycle_index) ) {
                co_cycle_log = calculate_gamma( log_a186009_map[tower_index], log_a186009_map[exp_cycle_index], tower_index, exp_cycle_index );
                std::cout << "    Expansive   cycle length: " << termination << "; range(" << exp_cycle_index << "," << tower_index << ") = " << co_cycle_log << std::endl;
            }
        }

        // Iterate through the contractive cycle lengths and calculate gamma initiating at the tower index
        for ( int initiation : contractive ) {
            int exp_cycle_index = tower_index + initiation;
            if ( exp_cycle_index <= 2 * tower_index && log_a186009_map.contains(exp_cycle_index) && log_a186009_map.contains(tower_index) ) {
                co_cycle_log = calculate_gamma( log_a186009_map[exp_cycle_index], log_a186009_map[tower_index], exp_cycle_index, tower_index );
                std::cout << "    Contractive cycle length: " << initiation << "; range(" << tower_index << "," << exp_cycle_index << ") = " << co_cycle_log << std::endl;
            }
        }
    }
}

/**
 * @brief The main() entry point is used to call menu() and also for testing components \b before calling menu().
 * @details Everything needs a starting point
 * @return int - Executable currently does not return any values to caller.
 */

int main(int argc, char* argv[])
{
    fs::path outdir = ".";
    fs::path a186009_file = outdir / "data" / "ln_A186009.csv";
    std::ofstream A186009_out = std::ofstream(a186009_file);

    // Calculate the growth rates of the towers at the critical points of the cycles and print results to console
    tower_point_cocycle_growth(A186009_out);

    std::cout << "Stopping before the rest of the gamma calculations and output formatting is built out..." << std::endl;
exit(0);        // protecting the above code for now as I build out the rest of the gamma calculations and output formatting below

    // Calculate gamma values for the expansive and contractive cycles based on space between towers and print results to console
    differential();

    // Calculate gamma values for the expansive and contractive cycles and print results to console
    gamma();

    uint32_t max = 80000;           // Default maximum number of terms to calculate for cumulative sequence
    int start = 15500, terms = 200; 


    // Stop before hitting this big stuff
    exit(0);
    Cumulative sum(start);         // A large tower exists at 79335

    // Iterate over the large tower
    mpz_class last_novel = sum.novel();
    mpz_class last_denom = sum.denominator();
    std::cout << "Index: " << start << ", Numerator: " << fmt::e(sum.numerator()) << ", Denominator: " << fmt::e(last_denom) << ", Novel: " << fmt::e(last_novel) << std::endl;
    for (int n=start; n<start+terms; ++n) {
        sum++;
        mpz_class novel = sum.novel();
        mpz_class denom = sum.denominator();

        mpz_class ratio_num = novel * last_denom;
        mpz_class ratio_den = denom * last_novel;
        mpf_class ratio = mpf_class(ratio_num) / mpf_class(ratio_den);

// std::cout << "Index: " << n << ", last novel: " << fmt::e(last_novel) << ", novel: " << fmt::e(novel) << ", ratio numer: " << fmt::e(ratio_num) << std::endl;
// std::cout << "Index: " << n << ", last denom: " << fmt::e(last_denom) << ", denom: " << fmt::e(denom) << ", ratio denom: " << fmt::e(ratio_den) << std::endl;
// std::cout << "Index: " << n << ", ratio: " << fmt::f(8,6,ratio.get_d()) << std::endl;

        std::cout << "Index: " << n << ", Numerator: " << fmt::e(sum.numerator()) << ", Denominator: "
                << fmt::e(denom) << ", Novel: " << fmt::e(novel) << ", Ratio: " << fmt::f(8,6,ratio.get_d()) << std::endl;

        last_novel = novel;
        last_denom = denom;
    }

    // Process command line arguments to set debug mode and output directory
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // If the debug flag is set, enable debug mode which may print additional information during execution.
        if (arg == "-d" || arg == "--debug") {
            debug = true;

        // If an output directory is given as an argument, attempt to create it and use it for output. Otherwise, use the current directory.
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            outdir = argv[++i]; // next argument is directory

        } else if ((arg == "-n" || arg == "--max") && i + 1 < argc) {
            try {
                max = std::stoul(argv[++i]);
            } catch (const std::exception& e) {
                std::cerr << "Invalid value for -n: " << argv[i] << "\n";
                return 1;
            }

            // Otherise it's an error so print an error message and exit.
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            return 1;
        }
    }

    // Attempt to create the output directory if it doesn't exist, and check if it was successful. If not, print an error message and exit.
    if (!fs::create_directories(outdir) && !fs::exists(outdir)) {
        std::cerr << "Cannot create output directory: " << outdir << "\n";
        return 1;
    }

    // Size of the arrays holding the position in the 41 or 53 cycles
    uint32_t asize = 800;
    uint32_t csize = 100000;

    fs::path cycle_filename = outdir / "cycle_output.csv";
    cycle_output::cycle_fout = std::ofstream(cycle_filename);
    cycle_output::cycle_fout << "index,depth,cycle\n";
    cycle_output::max_depth = 6;

    A022921 a022921;

    // This computes all the non-linear cycles up to the first three 31,867 cycles (95601)
    for ( int i = 0; i<3; ++i )         // The number of large (31867) cycles to find
    {
        if ( !found_cycle( a022921  ) ) {
            printf("Not a known cycle !!!\n\n");
            break;
        }
    }

    // Close the cycle output file if it's open
    if (cycle_output::cycle_fout.is_open())
        cycle_output::cycle_fout.close();    

    bool run_novel_sum_4 = true;

    // Run the novel sum 4 which calculates the sum of the ratios of the novel term to the prior term for 4 consecutive terms, and write results to file
    if ( run_novel_sum_4 ) {

        // Loop to generate term group ratios for start_terms > 1 to max_terms and write to file
        int start_terms=1, max_terms=1;

        for (int i = start_terms; i <= max_terms; ++i)
        {        
            fs::path filename = outdir / ("group_ratios_" + std::to_string(i) + ".txt");
            std::ofstream fout(filename);

            if (!fout)
            {
                std::cerr << "Cannot open " << filename << "\n";
            }
            else {
                std::cout << i << " term consecutive group ratios\n";
                novel_sum_4(fout, i, 80000);
            }
        }
    }
    // // Loop to generate term group ratios for start_terms > 1 to max_terms and write to file
    // int start_terms=16, max_terms=15;
    // for (int i = start_terms; i <= max_terms; ++i)
    // {
    //     fs::path filename = outdir / ("group_ratios_" + std::to_string(i) + ".txt");
    //     std::ofstream fout(filename);

    //     if (!fout)
    //     {
    //         std::cerr << "Cannot open " << filename << "\n";
    //         continue;
    //     }

    //     std::cout << i << " term consecutive group ratios\n";

    //     novel_sum_4(fout, i, 80000);
    // }

    // eleven_or_twelve();

    // The following determines if 11 terms is sufficient to cover the next 2^{-k} interval
    capped(1,0,1);
    capped(2,1,1);
    capped(3,3,3);
    capped(4,7,5);

    // capped(26,199,11);
    // capped(27,210,11);
    // capped(28,222,11);
    // capped(29,233,11);
    // capped(30,245,11);

    bool run_cumulative_seq6 = true;

    // Run the cumulative sequence 6 which calculates the number of terms of A186009 needed to cover the next 1/2^n interval, and write results to file
    if ( run_cumulative_seq6 ) {

        fs::path filename_py = outdir / "partial_py.txt";
        fs::path filename_rd = outdir / "partial_rd.txt";

        std::ofstream fout_py(filename_py);
        std::ofstream fout_rd(filename_rd);

        std::cout << "Calculating number of terms covering next 2^{-k} interval...\n";
        Cumulative_seq6( fout_py, fout_rd, max-1 );
    }

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
