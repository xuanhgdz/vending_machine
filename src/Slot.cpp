#include "vending/Slot.hpp"
#include "vending/Validation.hpp"
#include <stdexcept>
#include <utility>

namespace vending {

Slot::Slot(std::string code, Product product, int quantity, int capacity)
    : code_{std::move(code)}, product_{std::move(product)},
      quantity_{quantity}, capacity_{capacity} {
    detail::requireIdentifier(code_);
    if (capacity_ <= 0 || quantity_ < 0 || quantity_ > capacity_) {
        throw std::invalid_argument("Can capacity > 0 va 0 <= quantity <= capacity.");
    }
}

const std::string& Slot::code() const noexcept { return code_; }
const Product& Slot::product() const noexcept { return product_; }
int Slot::quantity() const noexcept { return quantity_; }
int Slot::capacity() const noexcept { return capacity_; }
bool Slot::inStock() const noexcept { return quantity_ > 0; }

void Slot::refill(int amount) {
    if (amount <= 0 || amount > capacity_ - quantity_) {
        throw std::invalid_argument("So luong them phai duong va khong vuot suc chua.");
    }
    quantity_ += amount;
}

void Slot::takeOne() {
    if (!inStock()) {
        throw std::logic_error("Khe hang da het hang.");
    }
    --quantity_;
}

} // namespace vending
