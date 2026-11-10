#include "vending/Inventory.hpp"
#include <stdexcept>
#include <utility>

namespace vending {

void Inventory::addSlot(Slot slot) {
    // Copy the key before moving slot; never depend on argument evaluation order.
    const auto code = slot.code();
    const auto result = slots_.try_emplace(code, std::move(slot));
    if (!result.second) {
        throw std::invalid_argument("Ma khe hang da ton tai: " + code);
    }
}

const Slot* Inventory::find(const std::string& code) const {
    const auto it = slots_.find(code);
    return it == slots_.end() ? nullptr : &it->second;
}

const std::map<std::string, Slot>& Inventory::slots() const noexcept { return slots_; }
std::size_t Inventory::size() const noexcept { return slots_.size(); }

} // namespace vending
