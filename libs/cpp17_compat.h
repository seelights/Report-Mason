/*
 * C++17 compatibility header for ReportMason project
 * Provides C++20 features for C++17 compilation
 */

#ifndef CPP17_COMPAT_H
#define CPP17_COMPAT_H

#include <string>
#include <string_view>
#include <cstring>
#include <array>
#include <iterator>
#include <cstddef>

// C++20 string methods compatibility for C++17
namespace std {
    // For std::string
    inline bool starts_with(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && 
               str.compare(0, prefix.size(), prefix) == 0;
    }
    
    inline bool starts_with(const std::string& str, const char* prefix) {
        return str.size() >= strlen(prefix) && 
               str.compare(0, strlen(prefix), prefix) == 0;
    }
    
    inline bool ends_with(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && 
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    inline bool ends_with(const std::string& str, const char* suffix) {
        size_t suffix_len = strlen(suffix);
        return str.size() >= suffix_len && 
               str.compare(str.size() - suffix_len, suffix_len, suffix) == 0;
    }
    
    // For std::string_view
    inline bool starts_with(const std::string_view& str, const std::string_view& prefix) {
        return str.size() >= prefix.size() && 
               str.compare(0, prefix.size(), prefix) == 0;
    }
    
    inline bool starts_with(const std::string_view& str, const char* prefix) {
        size_t prefix_len = strlen(prefix);
        return str.size() >= prefix_len && 
               str.compare(0, prefix_len, prefix) == 0;
    }
    
    inline bool ends_with(const std::string_view& str, const std::string_view& suffix) {
        return str.size() >= suffix.size() && 
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    inline bool ends_with(const std::string_view& str, const char* suffix) {
        size_t suffix_len = strlen(suffix);
        return str.size() >= suffix_len && 
               str.compare(str.size() - suffix_len, suffix_len, suffix) == 0;
    }
}

// std::span compatibility is handled by span_compat.h
// This file only provides string compatibility functions

#endif // CPP17_COMPAT_H
