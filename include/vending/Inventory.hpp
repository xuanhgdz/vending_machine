#pragma once

#include "vending/Slot.hpp"
#include <cstddef>
#include <map>
#include <string>

namespace vending {

class Inventory {
public:
    void addSlot(Slot slot);
    [[nodiscard]] const Slot* find(const std::string& code) const;
    [[nodiscard]] const std::map<std::string, Slot>& slots() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;

private:
    std::map<std::string, Slot> slots_;
};

} // namespace vending
