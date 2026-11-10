#pragma once

#include "vending/Money.hpp"
#include <string>

namespace vending {

class Product {
public:
    Product(std::string id, std::string name, Money price);
    [[nodiscard]] const std::string& id() const noexcept;
    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] Money price() const noexcept;

private:
    std::string id_;
    std::string name_;
    Money price_;
};

} // namespace vending
