#include "vending/MachineState.hpp"
#include "vending/Inventory.hpp"
#include <utility>

namespace vending {
namespace {

StateDecision stay(StateId id, const StateContext& context, bool accepted, std::string message) {
    return {accepted, id, context.selectedSlot, std::move(message)};
}

} // namespace

std::string_view stateName(StateId state) noexcept {
    switch (state) {
    case StateId::Idle: return "Idle";
    case StateId::Selecting: return "Selecting";
    case StateId::Paying: return "Paying";
    case StateId::Dispensing: return "Dispensing";
    }
    return "Unknown";
}

StateId IdleState::id() const noexcept { return StateId::Idle; }
std::string_view IdleState::name() const noexcept { return "Idle"; }
StateDecision IdleState::handle(const StateContext& context, const MachineEvent& event) const {
    switch (event.type) {
    case EventType::StartSelection:
        return {true, StateId::Selecting, std::nullopt, "Da bat dau chon hang."};
    case EventType::Describe:
        return stay(id(), context, true, "Cho bat dau mua; cho phep them khe hang.");
    case EventType::Cancel:
        return stay(id(), context, false, "Khong co lua chon dang thuc hien.");
    case EventType::ChooseSlot:
        return stay(id(), context, false, "Hay bat dau chon hang truoc.");
    }
    return stay(id(), context, false, "Su kien khong hop le.");
}

StateId SelectingState::id() const noexcept { return StateId::Selecting; }
std::string_view SelectingState::name() const noexcept { return "Selecting"; }
StateDecision SelectingState::handle(const StateContext& context, const MachineEvent& event) const {
    switch (event.type) {
    case EventType::ChooseSlot: {
        const auto* slot = context.inventory.find(event.slotCode);
        if (slot == nullptr) {
            return stay(id(), context, false, "Khong tim thay khe: " + event.slotCode);
        }
        if (!slot->inStock()) {
            return stay(id(), context, false, "Khe " + event.slotCode + " da het hang.");
        }
        return {true, StateId::Paying, event.slotCode,
                "Da chon " + slot->product().name() + ". Thanh toan chua mo trong phien ban nay."};
    }
    case EventType::Cancel:
        return {true, StateId::Idle, std::nullopt, "Da huy lua chon."};
    case EventType::Describe:
        return stay(id(), context, true, "Nhan ma khe; chi chap nhan hang con ton kho.");
    case EventType::StartSelection:
        return stay(id(), context, false, "May dang cho ban chon ma khe.");
    }
    return stay(id(), context, false, "Su kien khong hop le.");
}

StateId PayingState::id() const noexcept { return StateId::Paying; }
std::string_view PayingState::name() const noexcept { return "Paying"; }
StateDecision PayingState::handle(const StateContext& context, const MachineEvent& event) const {
    switch (event.type) {
    case EventType::Cancel:
        return {true, StateId::Idle, std::nullopt, "Da huy lua chon; ton kho khong thay doi."};
    case EventType::Describe:
        return stay(id(), context, true, "Da chon hang; hien co the huy. Nhan tien se duoc bo sung sau.");
    case EventType::StartSelection:
    case EventType::ChooseSlot:
        return stay(id(), context, false, "Hay huy lua chon hien tai truoc khi chon hang khac.");
    }
    return stay(id(), context, false, "Su kien khong hop le.");
}

StateId DispensingState::id() const noexcept { return StateId::Dispensing; }
std::string_view DispensingState::name() const noexcept { return "Dispensing"; }
StateDecision DispensingState::handle(const StateContext& context, const MachineEvent& event) const {
    if (event.type == EventType::Describe) {
        return stay(id(), context, true, "Khung trang thai xuat hang; chua duoc kich hoat o moc 1.");
    }
    return stay(id(), context, false, "Khong nhan lenh chon/huy khi xuat hang.");
}

} // namespace vending
