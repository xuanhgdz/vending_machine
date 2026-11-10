#include "vending/Product.hpp"
#include "vending/Validation.hpp"
#include <stdexcept>
#include <utility>

namespace vending {

Product::Product(std::string id, std::string name, Money price)
    : id_{std::move(id)}, name_{std::move(name)}, price_{price} {
    detail::requireIdentifier(id_);
    detail::requireName(name_);
    if (price_.dong() == 0 || price_.dong() % 1000 != 0) {
        throw std::invalid_argument("Gia phai duong va la boi so cua 1000 VND.");
    }
}

const std::string& Product::id() const noexcept { return id_; }
const std::string& Product::name() const noexcept { return name_; }
Money Product::price() const noexcept { return price_; }

} // namespace vending
