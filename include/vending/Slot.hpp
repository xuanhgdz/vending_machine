#pragma once

#include "vending/Product.hpp"
#include <string>

namespace vending {

class Slot {
public:
    Slot(std::string code, Product product, int quantity, int capacity);
    [[nodiscard]] const std::string& code() const noexcept;
    [[nodiscard]] const Product& product() const noexcept;
    [[nodiscard]] int quantity() const noexcept;
    [[nodiscard]] int capacity() const noexcept;
    [[nodiscard]] bool inStock() const noexcept;
    void refill(int amount);
    void takeOne();

private:
    std::string code_;
    Product product_;
    int quantity_;
    int capacity_;
};

} // namespace vending
