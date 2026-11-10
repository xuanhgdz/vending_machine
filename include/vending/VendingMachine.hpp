#pragma once

#include "vending/Inventory.hpp"
#include "vending/MachineState.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace vending {

class VendingMachine {
public:
    explicit VendingMachine(Inventory inventory);
    [[nodiscard]] StateId state() const noexcept;
    [[nodiscard]] const std::optional<std::string>& selectedSlot() const noexcept;
    [[nodiscard]] const Inventory& inventory() const noexcept;
    [[nodiscard]] DispatchResult dispatch(const MachineEvent& event);
    [[nodiscard]] std::vector<StateInfo> describeStates() const;
    void addSlot(Slot slot);

private:
    [[nodiscard]] const MachineState& currentState() const;
    Inventory inventory_;
    std::vector<std::unique_ptr<MachineState>> states_;
    StateId state_{StateId::Idle};
    std::optional<std::string> selectedSlot_;
};

} // namespace vending
