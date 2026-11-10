#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

namespace vending::detail {

inline void requireIdentifier(std::string_view id) {
    const auto validChar = [](unsigned char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
               (c >= '0' && c <= '9') || c == '-' || c == '_';
    };
    if (id.empty() || id.size() > 20 || !std::all_of(id.begin(), id.end(), validChar)) {
        throw std::invalid_argument("Ma can 1-20 ky tu chu, so, gach ngang hoac gach duoi.");
    }
}

inline void requireName(std::string_view name) {
    const auto controlChar = [](unsigned char c) { return c < 32 || c == 127; };
    const auto visibleChar = [](unsigned char c) { return c != ' '; };
    if (name.empty() || name.size() > 80 ||
        std::any_of(name.begin(), name.end(), controlChar) ||
        !std::any_of(name.begin(), name.end(), visibleChar)) {
        throw std::invalid_argument("Ten can 1-80 byte, khong rong va khong co ky tu dieu khien.");
    }
}

} // namespace vending::detail
