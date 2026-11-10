#include "vending/VendingMachine.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace vending {

VendingMachine::VendingMachine(Inventory inventory) : inventory_{std::move(inventory)} {
    states_.reserve(4);
    states_.push_back(std::make_unique<IdleState>());
    states_.push_back(std::make_unique<SelectingState>());
    states_.push_back(std::make_unique<PayingState>());
    states_.push_back(std::make_unique<DispensingState>());
}

StateId VendingMachine::state() const noexcept { return state_; }
const std::optional<std::string>& VendingMachine::selectedSlot() const noexcept { return selectedSlot_; }
const Inventory& VendingMachine::inventory() const noexcept { return inventory_; }

const MachineState& VendingMachine::currentState() const {
    const auto it = std::find_if(states_.begin(), states_.end(), [this](const auto& candidate) {
        return candidate->id() == state_;
    });
    if (it == states_.end()) {
        throw std::logic_error("Trang thai may khong duoc dang ky.");
    }
    return **it;
}

DispatchResult VendingMachine::dispatch(const MachineEvent& event) {
    const StateContext context{inventory_, selectedSlot_};
    // Dynamic dispatch through an abstract base reference; states never own context.
    auto decision = currentState().handle(context, event);
    const auto before = state_;
    if (decision.accepted) {
        // String allocation happened inside handle(); commit uses non-throwing swaps.
        selectedSlot_.swap(decision.nextSelectedSlot);
        state_ = decision.nextState;
    }
    return {decision.accepted, before, state_, std::move(decision.message)};
}

std::vector<StateInfo> VendingMachine::describeStates() const {
    const StateContext context{inventory_, selectedSlot_};
    std::vector<StateInfo> result;
    result.reserve(states_.size());
    for (const auto& state : states_) {
        const auto description = state->handle(context, {EventType::Describe});
        result.push_back({state->id(), state->name(), description.message});
    }
    return result;
}

void VendingMachine::addSlot(Slot slot) {
    if (state_ != StateId::Idle) {
        throw std::logic_error("Chi them khe hang khi may o Idle.");
    }
    inventory_.addSlot(std::move(slot));
}

} // namespace vending
