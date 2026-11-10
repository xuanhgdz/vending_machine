#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace vending {

class Inventory;

enum class StateId { Idle, Selecting, Paying, Dispensing };
enum class EventType { StartSelection, ChooseSlot, Cancel, Describe };

struct MachineEvent {
    EventType type;
    std::string slotCode{};
};

// Borrowed read-only references, valid only during a synchronous handle() call.
struct StateContext {
    const Inventory& inventory;
    const std::optional<std::string>& selectedSlot;
};

struct StateDecision {
    bool accepted;
    StateId nextState;
    std::optional<std::string> nextSelectedSlot;
    std::string message;
};

struct DispatchResult {
    bool accepted;
    StateId from;
    StateId to;
    std::string message;
};

struct StateInfo {
    StateId id;
    std::string_view name;
    std::string description;
};

[[nodiscard]] std::string_view stateName(StateId state) noexcept;

} // namespace vending
