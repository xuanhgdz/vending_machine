#pragma once

#include "vending/VendingMachine.hpp"
#include <iosfwd>

namespace vending {

class ConsoleUI {
public:
    ConsoleUI(VendingMachine& machine, std::istream& input, std::ostream& output);
    int run();

private:
    void printMenu() const;
    void showInventory() const;
    void showStates() const;
    void showResult(const DispatchResult& result) const;
    bool addSlot();
    VendingMachine& machine_;
    std::istream& input_;
    std::ostream& output_;
};

} // namespace vending
