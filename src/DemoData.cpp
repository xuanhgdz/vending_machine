#include "vending/DemoData.hpp"

namespace vending {

Inventory makeDemoInventory() {
    Inventory inventory;
    inventory.addSlot(Slot{"A1", Product{"WATER", "Nuoc suoi", Money{10000}}, 2, 10});
    inventory.addSlot(Slot{"A2", Product{"COLA", "Nuoc ngot", Money{15000}}, 0, 10});
    inventory.addSlot(Slot{"B1", Product{"MILK", "Sua hop", Money{20000}}, 5, 8});
    return inventory;
}

} // namespace vending
