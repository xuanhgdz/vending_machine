#pragma once

#include <cstdint>
#include <iosfwd>

namespace vending {

// Value type: non-negative integer VND; no floating-point rounding.
class Money {
public:
    explicit Money(std::int64_t dong = 0);
    [[nodiscard]] std::int64_t dong() const noexcept;
    [[nodiscard]] Money operator+(Money other) const;
    [[nodiscard]] Money operator-(Money other) const;
    [[nodiscard]] bool operator==(const Money&) const noexcept = default;
    [[nodiscard]] bool operator<(Money other) const noexcept;
    friend std::ostream& operator<<(std::ostream& out, Money money);

private:
    std::int64_t dong_;
};

} // namespace vending
