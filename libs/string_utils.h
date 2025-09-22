/*
 * string_utils.h
 * String utility functions for C++17 compatibility
 * This provides replacements for C++20 string functions
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <string_view>

namespace string_utils {

// C++17 compatible ends_with replacement
inline bool ends_with(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

inline bool ends_with(const std::string_view& str, const std::string_view& suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

// C++17 compatible starts_with replacement
inline bool starts_with(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

inline bool starts_with(const std::string_view& str, const std::string_view& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

} // namespace string_utils

#endif // STRING_UTILS_H
