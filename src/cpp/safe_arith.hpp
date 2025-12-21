/**
 * @file safe_arith.hpp
 * @author Wayne Brassem (wbrassem@rogers.com)
 * @brief This header defines a template struct for safe arithmetic operations that check for overflow.
 * @version 1.0
 * @date 2025-12-20
 * 
 * @copyright Copyright (c) 2025 Wayne Brassem
 * 
 */

#pragma once
#include <type_traits>
#include <stdexcept>
#include <limits>

template <typename T>
struct safe_arith {
    static T add(const T& a, const T& b) {
        if constexpr (std::is_integral_v<T>) {
            if ((b > 0 && a > std::numeric_limits<T>::max() - b) ||
                (b < 0 && a < std::numeric_limits<T>::min() - b)) {
                throw std::overflow_error("Integer addition overflow");
            }
        }
        return a + b;
    }

    static T sub(const T& a, const T& b) {
        if constexpr (std::is_integral_v<T>) {
            if ((b < 0 && a > std::numeric_limits<T>::max() + b) ||
                (b > 0 && a < std::numeric_limits<T>::min() + b)) {
                throw std::overflow_error("Integer subtraction overflow");
            }
        }
        return a - b;
    }

    static T mul(const T& a, const T& b) {
        if constexpr (std::is_integral_v<T>) {
            if (a != 0 && (b > std::numeric_limits<T>::max() / a ||
                           b < std::numeric_limits<T>::min() / a)) {
                throw std::overflow_error("Integer multiplication overflow");
            }
        }
        return a * b;
    }
};
