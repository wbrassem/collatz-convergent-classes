/**
 * @file format.hpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief General-purpose, type-safe formatting utilities for streams.
 * @date 2026-03-11
 *
 * Provides ephemeral helpers for formatting floats, integers, strings, and GMP types.
 * Supports optional width, precision, and fill characters where appropriate.
 *
 * Example usage:
 * @code
 * csv(fout,
 *     fmt::d(3, n),             // width 3, default fill
 *     fmt::f(9, 7, ratio),      // width 9, precision 7
 *     fmt::s(name),
 *     fmt::d(5, bigInt, '0')    // zero-padded
 * );
 * @endcode
 */

// Original non-templated version below, which was more verbose and less flexible.  This new version is more concise and type-safe, but the original is left here for reference.

// #pragma once

// #include <iostream>
// #include <iomanip>
// #include <string>

// #ifdef gnu_mp
// #include <gmpxx.h>
// #endif

// namespace fmt {

//     // -------------------
//     // Float/Double formatting
//     // -------------------
//     struct F {
//         int width{};
//         int prec{};
//         double value{};
//         char fill{};
//         F(int w, int p, double v, char f = ' ') : width(w), prec(p), value(v), fill(f) {}
//         friend std::ostream& operator<<(std::ostream& os, const F& f) {
//             return os << std::setw(f.width) << std::setfill(f.fill)
//                       << std::fixed << std::setprecision(f.prec) << f.value;
//         }
//     };
//     inline F f(int width, int prec, double val, char fill = ' ') { return F(width, prec, val, fill); }

//     // -------------------
//     // Integer formatting
//     // -------------------
//     struct D {
//         int width{};
//         long long value{};
//         char fill{};
//         D(int w, long long v, char f = ' ') : width(w), value(v), fill(f) {}
//         friend std::ostream& operator<<(std::ostream& os, const D& d) {
//             return os << std::setw(d.width) << std::setfill(d.fill) << d.value;
//         }
//     };
//     inline D d(int width, long long val, char fill = ' ') { return D(width, val, fill); }

//     // -------------------
//     // String formatting
//     // -------------------
//     struct S {
//         std::string value;
//         explicit S(std::string v) : value(std::move(v)) {}
//         friend std::ostream& operator<<(std::ostream& os, const S& s) {
//             return os << s.value;
//         }
//     };
//     inline S s(const std::string& val) { return S(val); }
//     inline S s(std::string&& val) { return S(std::move(val)); }

//     // -------------------
//     // Scientific formatting
//     // -------------------
//     struct E {
//         int width{};
//         int prec{};
//         double value{};
//         char fill{};
//         E(int w, int p, double v, char f = ' ') : width(w), prec(p), value(v), fill(f) {}
//         friend std::ostream& operator<<(std::ostream& os, const E& e) {
//             return os << std::setw(e.width) << std::setfill(e.fill)
//                       << std::scientific << std::setprecision(e.prec) << e.value;
//         }
//     };
//     inline E e(int width, int prec, double val, char fill = ' ') { return E(width, prec, val, fill); }

// #ifdef gnu_mp
//     // -------------------
//     // GMP support
//     // -------------------

//     // -------------------
//     // MPF formatting
//     // -------------------
//     struct MPF {
//         int width{};
//         int prec{};
//         mpf_class value{};
//         char fill{};
//         MPF(int w, int p, const mpf_class& v, char f = ' ') : width(w), prec(p), value(v), fill(f) {}
//         friend std::ostream& operator<<(std::ostream& os, const MPF& m) {
//             return os << std::setw(m.width) << std::setfill(m.fill)
//                       << std::fixed << std::setprecision(m.prec) << m.value.get_d();
//         }
//     };
//     inline MPF f(int width, int prec, const mpf_class& val, char fill = ' ') { return MPF(width, prec, val, fill); }

//     // -------------------
//     // MPZ formatting
//     // -------------------
//     struct MPZ {
//         int width{};
//         mpz_class value{};
//         char fill{};
//         MPZ(int w, const mpz_class& v, char f = ' ') : width(w), value(v), fill(f) {}
//         friend std::ostream& operator<<(std::ostream& os, const MPZ& m) {
//             return os << std::setw(m.width) << std::setfill(m.fill) << m.value;
//         }
//     };
//     inline MPZ d(int width, const mpz_class& val, char fill = ' ') { return MPZ(width, val, fill); }

//     // -------------------
//     // Scientific formatting for mpf_class
//     // -------------------
//     struct MPFE {
//         int width{};
//         int prec{};
//         mpf_class value{};
//         char fill{};
//         MPFE(int w, int p, const mpf_class& v, char f = ' ')
//             : width(w), prec(p), value(v), fill(f) {}

//         friend std::ostream& operator<<(std::ostream& os, const MPFE& m) {
//             char* s = nullptr;
//             gmp_asprintf(&s, "%.*Fe", m.prec, m.value.get_mpf_t()); // GMP scientific format
//             if (m.width > 0)
//                 os << std::setw(m.width) << std::setfill(m.fill) << s;
//             else
//                 os << s;
//             gmp_free(s);
//             return os;
//         }
//     };
//     inline MPFE e(int width, int prec, const mpf_class& val, char fill = ' ') {
//         return MPFE(width, prec, val, fill);
//     }

//     // -------------------
//     // Scientific formatting for mpz_class (convert via mpf_class)
//     // -------------------
//     struct MPZE {
//         int width{};
//         int prec{};
//         mpz_class value{};
//         char fill{};
//         MPZE(int w, int p, const mpz_class& v, char f = ' ')
//             : width(w), prec(p), value(v), fill(f) {}

//         friend std::ostream& operator<<(std::ostream& os, const MPZE& m) {
//             mpf_class temp(m.value); // convert integer -> mpf_class
//             char* s = nullptr;
//             gmp_asprintf(&s, "%.*Fe", m.prec, temp.get_mpf_t());
//             if (m.width > 0)
//                 os << std::setw(m.width) << std::setfill(m.fill) << s;
//             else
//                 os << s;
//             gmp_free(s);
//             return os;
//         }
//     };
//     inline MPZE e(int width, int prec, const mpz_class& val, char fill = ' ') {
//         return MPZE(width, prec, val, fill);
//     }    

// #endif // gnu_mp

// } // namespace fmt

// // -------------------
// // CSV helper function
// // -------------------
// template<typename First, typename... Rest>
// void csv(std::ostream& os, First&& first, Rest&&... rest)
// {
//     os << std::forward<First>(first);
//     ((os << "," << std::forward<Rest>(rest)), ...);
//     os << '\n';
// }


#pragma once

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>

#ifdef gnu_mp
#include <gmpxx.h>
#include <gmp.h>
#endif

namespace fmt {

    // ===================
    // STRING (unchanged)
    // ===================
    struct S {
        std::string value;
        explicit S(std::string v) : value(std::move(v)) {}
        friend std::ostream& operator<<(std::ostream& os, const S& s) {
            return os << s.value;
        }
    };
    inline S s(const std::string& val) { return S(val); }
    inline S s(std::string&& val) { return S(std::move(val)); }

    // ===================
    // INTEGER TEMPLATE
    // ===================
    template<typename T>
    struct D {
        int width{};
        T value;
        char fill{};

        D(int w, const T& v, char f = ' ')
            : width(w), value(v), fill(f) {}
    };

    template<typename T>
    inline std::ostream& operator<<(std::ostream& os, const D<T>& d) {
        return os << std::setw(d.width) << std::setfill(d.fill) << d.value;
    }

    template<typename T>
    inline D<T> d(const T& val) {
        return D<T>(0, val, ' ');
    }

    template<typename T>
    inline D<T> d(int width, const T& val, char fill = ' ') {
        return D<T>(width, val, fill);
    }

    // ===================
    // FIXED FLOAT TEMPLATE
    // ===================
    template<typename T>
    struct F {
        int width{};
        int prec{};
        T value;
        char fill{};

        F(int w, int p, const T& v, char f = ' ')
            : width(w), prec(p), value(v), fill(f) {}
    };

    // ---- double specialization
    inline std::ostream& operator<<(std::ostream& os, const F<double>& f) {
        return os << std::setw(f.width) << std::setfill(f.fill)
                  << std::fixed << std::setprecision(f.prec)
                  << f.value;
    }

    template<typename T>
    inline F<T> f(const T& val) {
        return F<T>(0, 6, val, ' ');   // default precision = 6
    }

#ifdef gnu_mp
    // ---- mpf_class specialization (precision-safe)
    inline std::ostream& operator<<(std::ostream& os, const F<mpf_class>& f) {
        char* s = nullptr;
        gmp_asprintf(&s, "%.*Ff", f.prec, f.value.get_mpf_t());
        os << std::setw(f.width) << std::setfill(f.fill) << s;
        free(s);
        return os;
    }
#endif

    template<typename T>
    inline F<T> f(int width, int prec, const T& val, char fill = ' ') {
        return F<T>(width, prec, val, fill);
    }

    // ===================
    // SCIENTIFIC TEMPLATE
    // ===================
    template<typename T>
    struct E {
        int width{};
        int prec{};
        T value;
        char fill{};

        E(int w, int p, const T& v, char f = ' ')
            : width(w), prec(p), value(v), fill(f) {}
    };

    // ---- double specialization
    inline std::ostream& operator<<(std::ostream& os, const E<double>& e) {
        return os << std::setw(e.width) << std::setfill(e.fill)
                  << std::scientific << std::setprecision(e.prec)
                  << e.value;
    }

    template<typename T>
    inline E<T> e(const T& val) {
        return E<T>(0, 6, val, ' ');
    }

#ifdef gnu_mp
    // ---- mpf_class specialization
    inline std::ostream& operator<<(std::ostream& os, const E<mpf_class>& e) {
        char* s = nullptr;
        gmp_asprintf(&s, "%.*Fe", e.prec, e.value.get_mpf_t());
        os << std::setw(e.width) << std::setfill(e.fill) << s;
        free(s);
        return os;
    }

    // ---- mpz_class specialization (via mpf_class)
    inline std::ostream& operator<<(std::ostream& os, const E<mpz_class>& e) {
        mpf_class temp(e.value);
        char* s = nullptr;
        gmp_asprintf(&s, "%.*Fe", e.prec, temp.get_mpf_t());
        os << std::setw(e.width) << std::setfill(e.fill) << s;
        free(s);
        return os;
    }
#endif

    template<typename T>
    inline E<T> e(int width, int prec, const T& val, char fill = ' ') {
        return E<T>(width, prec, val, fill);
    }

} // namespace fmt


// ===================
// CSV helper function
// ===================
template<typename First, typename... Rest>
void csv(std::ostream& os, First&& first, Rest&&... rest)
{
    os << std::forward<First>(first);
    ((os << "," << std::forward<Rest>(rest)), ...);
    os << '\n';
}