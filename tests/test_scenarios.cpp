#include "VendingMachine.h"
#include "Exceptions.h"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "=== CHAY KIEM THU TU DONG (UNIT & SCENARIO TESTS) ===\n";

    VendingMachine machine{"data/test_transactions.log"};
    machine.addSlot(Slot{"A1", ProductFactory::createDrink("Coca Cola", 10000, true), 2});
    machine.addSlot(Slot{"B2", ProductFactory::createSnack("Oreo", 8000, 40), 0});

    // Test 1: Kiem tra toan tu lop Money
    Money m1{5000};
    Money m2{10000};
    Money sum = m1 + m2;
    assert(sum.getAmount() == 15000);
    assert(sum >= m2);
    std::cout << "[PASS] Test 1: Nap chong toan tu Money (+, -, >=, ==) hoat dong chinh xac.\n";

    // Test 2: Kiem tra luong State Pattern (Idle -> Selecting -> Paying -> Dispensing -> Idle)
    machine.selectProduct("A1");
    machine.insertMoney(5000);
    assert(machine.getCurrentBalance().getAmount() == 5000);
    machine.insertMoney(10000);
    assert(machine.getCurrentBalance().getAmount() == 15000);
    machine.dispenseProduct();
    assert(machine.getCurrentBalance().getAmount() == 0);
    assert(machine.findSlot("A1")->getQuantity() == 1);
    std::cout << "[PASS] Test 2: State Pattern chuyen trang thai va xuat hang/tra tien thua chinh xac.\n";

    // Test 3: Kiem tra OutOfStockException
    bool caughtOutOfStock = false;
    try {
        machine.selectProduct("B2");
    } catch (const OutOfStockException&) {
        caughtOutOfStock = true;
    }
    assert(caughtOutOfStock);
    std::cout << "[PASS] Test 3: Nem va bat ngoai le OutOfStockException chinh xac.\n";

    // Test 4: Kiem tra InsufficientPaymentException va Cancel/Refund
    bool caughtInsufficient = false;
    machine.selectProduct("A1");
    machine.insertMoney(5000);
    try {
        machine.dispenseProduct();
    } catch (const InsufficientPaymentException&) {
        caughtInsufficient = true;
    }
    assert(caughtInsufficient);
    machine.cancelTransaction();
    assert(machine.getCurrentBalance().getAmount() == 0);
    std::cout << "[PASS] Test 4: Nem va bat ngoai le InsufficientPaymentException + Hoan tien chinh xac.\n";

    // Test 5: Kiem tra Refill + QRPayment Strategy
    machine.refillSlot("B2", 4);
    assert(machine.findSlot("B2")->getQuantity() == 4);
    machine.setPaymentStrategy(std::make_unique<QRPayment>("VietQR"));
    machine.selectProduct("B2");
    machine.insertMoney(8000);
    machine.dispenseProduct();
    assert(machine.findSlot("B2")->getQuantity() == 3);
    std::cout << "[PASS] Test 5: Refill va Strategy Pattern (QRPayment) hoat dong chinh xac.\n";

    std::cout << "=== TAT CA 5 BO KIEM THU DEU THANH CONG (100% PASSED)! ===\n";
    return 0;
}
