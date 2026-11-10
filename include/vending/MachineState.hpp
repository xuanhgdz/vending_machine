#pragma once

#include "vending/MachineTypes.hpp"
#include <string_view>

namespace vending {

class MachineState {
public:
    virtual ~MachineState() = default;
    [[nodiscard]] virtual StateId id() const noexcept = 0;
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;
    [[nodiscard]] virtual StateDecision handle(
        const StateContext& context, const MachineEvent& event) const = 0;
};

class IdleState final : public MachineState {
public:
    [[nodiscard]] StateId id() const noexcept override;
    [[nodiscard]] std::string_view name() const noexcept override;
    [[nodiscard]] StateDecision handle(const StateContext&, const MachineEvent&) const override;
};

class SelectingState final : public MachineState {
public:
    [[nodiscard]] StateId id() const noexcept override;
    [[nodiscard]] std::string_view name() const noexcept override;
    [[nodiscard]] StateDecision handle(const StateContext&, const MachineEvent&) const override;
};

class PayingState final : public MachineState {
public:
    [[nodiscard]] StateId id() const noexcept override;
    [[nodiscard]] std::string_view name() const noexcept override;
    [[nodiscard]] StateDecision handle(const StateContext&, const MachineEvent&) const override;
};

// Week 1: complete interface/dispatch shell. Real dispensing belongs to week 2.
class DispensingState final : public MachineState {
public:
    [[nodiscard]] StateId id() const noexcept override;
    [[nodiscard]] std::string_view name() const noexcept override;
    [[nodiscard]] StateDecision handle(const StateContext&, const MachineEvent&) const override;
};

} // namespace vending
