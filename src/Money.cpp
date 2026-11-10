#include "vending/Money.hpp"

#include <limits>
#include <ostream>
#include <stdexcept>

namespace vending {

Money::Money(std::int64_t dong) : dong_{dong} {
    if (dong < 0) {
        throw std::invalid_argument("Tien khong duoc am.");
    }
}

std::int64_t Money::dong() const noexcept { return dong_; }

Money Money::operator+(Money other) const {
    if (other.dong_ > std::numeric_limits<std::int64_t>::max() - dong_) {
        throw std::overflow_error("Tong tien vuot gioi han int64_t.");
    }
    return Money{dong_ + other.dong_};
}

Money Money::operator-(Money other) const {
    if (other.dong_ > dong_) {
        throw std::invalid_argument("Phep tru lam so tien am.");
    }
    return Money{dong_ - other.dong_};
}

bool Money::operator<(Money other) const noexcept { return dong_ < other.dong_; }

std::ostream& operator<<(std::ostream& out, Money money) {
    return out << money.dong_ << " VND";
}

} // namespace vending
