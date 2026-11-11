#include "VendingMachine.h"
#include "Exceptions.h"
#include <iostream>
#include <limits> // Dung de xoa bo dem std::cin khi nguoi dung nhap sai kieu du lieu

// Ham khoi tao du lieu mau cho may ban hang (Uu tien doc tu file data/inventory.txt)
void initializeMachine(VendingMachine& machine) {
    if (machine.loadInventoryFromFile("data/inventory.txt") ||
        machine.loadInventoryFromFile("../data/inventory.txt")) {
        std::cout << "[He thong]: Da nap danh muc san pham tu file data/inventory.txt thanh cong!\n";
        return;
    }

    // Du phong (Fallback) neu chay o thu muc khac khong tim thay file data/inventory.txt
    std::cout << "[He thong]: Su dung du lieu mac dinh tu ProductFactory.\n";
    machine.addSlot(Slot{"A1", ProductFactory::createDrink("Coca Cola", 10000, true), 3});
    machine.addSlot(Slot{"A2", ProductFactory::createDrink("Tra Xanh 0 Do", 12000, true), 2});
    machine.addSlot(Slot{"B1", ProductFactory::createSnack("Snack Khoai Tay Lay's", 15000, 52), 2});
    machine.addSlot(Slot{"B2", ProductFactory::createSnack("Banh Oreo Socola", 8000, 40), 0}); // Ton kho = 0 de test OutOfStockException
}

// Kich ban chay tu dong (Auto-Demo) giup nhom bieu dien nhanh toan bo tinh nang truoc giang vien chi voi 1 phim bam
void runAutomatedDemo(VendingMachine& machine) {
    std::cout << "\n================ BAT DAU KICH BAN DEMO TU DONG ================\n";

    // Kich ban 1: Mua hang thanh cong + tra tien thua (Idle -> Selecting -> Paying -> Dispensing -> Idle)
    try {
        std::cout << "\n--- [Kich ban 1]: Mua Coca Cola (A1 - 10.000d), nap 2 lan tien (5.000d + 10.000d) ---\n";
        machine.selectProduct("A1");
        machine.insertMoney(5000);  // Chuyen sang PayingState
        machine.insertMoney(10000); // Tong 15.000d >= 10.000d -> Chuyen sang DispensingState
        machine.dispenseProduct();  // Xuat Coca Cola va tra thua 5.000d -> Ve IdleState
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
    }

    // Kich ban 2: Bat ngoai le OutOfStockException khi chon ngan het hang (B2)
    try {
        std::cout << "\n--- [Kich ban 2]: Thu chon ngan B2 dang het hang (Test OutOfStockException) ---\n";
        machine.selectProduct("B2");
    } catch (const OutOfStockException& e) {
        std::cout << "  -> Da bat ngoai le thanh cong: " << e.what() << "\n";
    }

    // Kich ban 3: Bat ngoai le InsufficientPaymentException & Huy giao dich hoan tien
    try {
        std::cout << "\n--- [Kich ban 3]: Chon Snack Lay's (B1 - 15.000d), moi nap 10.000d ma doi xuat hang ---\n";
        machine.selectProduct("B1");
        machine.insertMoney(10000);
        machine.dispenseProduct(); // Se nem InsufficientPaymentException vi con thieu 5.000d
    } catch (const InsufficientPaymentException& e) {
        std::cout << "  -> Da bat ngoai le thanh cong: " << e.what() << "\n";
        std::cout << "  -> Khach quyet dinh huy giao dich de lay lai tien:\n";
        machine.cancelTransaction(); // Hoan lai 10.000d va ve IdleState
    }

    // Kich ban 4: Doi phuong thuc sang QRPayment (Strategy Pattern) va Refill hang cho ngan B2
    try {
        std::cout << "\n--- [Kich ban 4]: Refill ngan B2 va thanh toan bang ma QR (Strategy Pattern) ---\n";
        machine.refillSlot("B2", 5);
        machine.setPaymentStrategy(std::make_unique<QRPayment>("MoMo"));
        machine.selectProduct("B2");
        machine.insertMoney(8000);
        machine.dispenseProduct();
        machine.setPaymentStrategy(std::make_unique<CashPayment>()); // Chuyen lai ve tien mat
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
    }

    std::cout << "\n================ KET THUC KICH BAN DEMO TU DONG ================\n";
}

int main() {
    VendingMachine machine;
    initializeMachine(machine);

    int choice = -1;
    while (choice != 0) {
        machine.displayInventory();
        std::cout << "MENU DIEU KHIEN:\n"
                  << "  1. Chon san pham (Select)\n"
                  << "  2. Nap tien (Insert Money)\n"
                  << "  3. Xuat hang (Dispense)\n"
                  << "  4. Huy giao dich & Hoan tien (Cancel/Refund)\n"
                  << "  5. Doi phuong thuc thanh toan (Cash / QR)\n"
                  << "  6. Nap them hang vao may (Refill - Danh cho Ky thuat vien)\n"
                  << "  7. Chay Kich ban Demo Tu dong (Auto-Demo 4 kich ban loi)\n"
                  << "  0. Thoat chuong trinh\n"
                  << "Nhap lua chon cua ban: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[Loi]: Vui long nhap mot so nguyen!\n";
            continue;
        }

        // Boc toan bo thao tac nguoi dung trong khoi try-catch de chuong trinh khong bao gio bi crash (Bai 7 - CLO4)
        try {
            if (choice == 1) {
                std::string code;
                std::cout << "Nhap ma ngan chua (VD: A1, A2, B1, B2): ";
                std::cin >> code;
                machine.selectProduct(code);
            } else if (choice == 2) {
                long long amount = 0;
                std::cout << "Nhap so tien nap vao (VND): ";
                std::cin >> amount;
                machine.insertMoney(amount);
            } else if (choice == 3) {
                machine.dispenseProduct();
            } else if (choice == 4) {
                machine.cancelTransaction();
            } else if (choice == 5) {
                int method = 1;
                std::cout << "Chon kenh thanh toan (1: Tien mat, 2: Quet ma QR): ";
                std::cin >> method;
                if (method == 2) {
                    machine.setPaymentStrategy(std::make_unique<QRPayment>("VietQR"));
                } else {
                    machine.setPaymentStrategy(std::make_unique<CashPayment>());
                }
            } else if (choice == 6) {
                std::string code;
                int qty = 0;
                std::cout << "Nhap ma ngan can Refill: ";
                std::cin >> code;
                std::cout << "Nhap so luong nap them: ";
                std::cin >> qty;
                machine.refillSlot(code, qty);
            } else if (choice == 7) {
                runAutomatedDemo(machine);
            } else if (choice == 0) {
                std::cout << "Cam on ban da su dung may ban hang tu dong!\n";
            } else {
                std::cout << "[Thong bao]: Lua chon khong hop le!\n";
            }
        } catch (const OutOfStockException& e) {
            std::cout << "\n>>> BAT NGOAI LE HET HANG: " << e.what() << "\n";
        } catch (const InsufficientPaymentException& e) {
            std::cout << "\n>>> BAT NGOAI LE THIEU TIEN: " << e.what() << "\n";
        } catch (const std::exception& e) {
            // Bat da hinh moi ngoai le chuan khac (InvalidSlotException, std::invalid_argument,...)
            std::cout << "\n>>> BAT NGOAI LE HE THONG: " << e.what() << "\n";
        }
    }

    return 0;
}
