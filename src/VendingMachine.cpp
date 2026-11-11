#include "VendingMachine.h"
#include "Exceptions.h"
#include <iostream>
#include <iomanip>   // Dung std::setw, std::left de can chinh bang hien thi dep mat (Bai 8 - CLO4)
#include <algorithm> // Dung std::find_if (Bai 9 - STL Algorithm - CLO4)
#include <fstream>   // Dung std::ifstream / std::ofstream de doc/ghi file (Bai 8 - File I/O - CLO4)
#include <sstream>   // Dung std::stringstream de phan tach dong du lieu tu file cau hinh

// ============================================================================
// PHAN 1: CAI DAT CAC PHUONG THUC CUA LOP VENDING MACHINE
// ============================================================================

VendingMachine::VendingMachine(const std::string& logPath)
    : currentState{std::make_unique<IdleState>()},          // May khoi dong luon o trang thai Ranh (IdleState)
      paymentStrategy{std::make_unique<CashPayment>()},     // Mac dinh nhan tien mat (CashPayment)
      currentBalance{0},
      selectedSlotCode{""},
      logFilePath{logPath} {}

// Nhan tham chieu rvalue (Slot&&) de di chuyen (move) doi tuong Slot vao vector ma khong can copy
void VendingMachine::addSlot(Slot&& newSlot) {
    slots.push_back(std::move(newSlot));
}

// Doc danh muc san pham tu file data/inventory.txt (Bai 8 - File I/O - CLO4)
// Dinh dang moi dong: TYPE|CODE|NAME|PRICE|EXTRA|QUANTITY
bool VendingMachine::loadInventoryFromFile(const std::string& filePath) {
    std::ifstream inFile{filePath};
    if (!inFile.is_open()) {
        return false;
    }

    slots.clear();
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') {
            continue; // Bo qua dong trong hoac dong comment bat dau bang #
        }
        std::stringstream ss{line};
        std::string type, code, name, priceStr, extraStr, qtyStr;

        if (std::getline(ss, type, '|') &&
            std::getline(ss, code, '|') &&
            std::getline(ss, name, '|') &&
            std::getline(ss, priceStr, '|') &&
            std::getline(ss, extraStr, '|') &&
            std::getline(ss, qtyStr, '|')) {

            long long price = std::stoll(priceStr);
            int extra = std::stoi(extraStr);
            int qty = std::stoi(qtyStr);

            if (type == "DRINK") {
                addSlot(Slot{code, ProductFactory::createDrink(name, price, extra != 0), qty});
            } else if (type == "SNACK") {
                addSlot(Slot{code, ProductFactory::createSnack(name, price, extra), qty});
            }
        }
    }
    return !slots.empty();
}

// Thay doi phuong thuc thanh toan ngay luc chay (Runtime) nho Strategy Pattern
void VendingMachine::setPaymentStrategy(std::unique_ptr<IPaymentStrategy> strategy) {
    if (strategy != nullptr) {
        paymentStrategy = std::move(strategy);
        std::cout << "-> Da chuyen phuong thuc thanh toan sang: " << paymentStrategy->getMethodName() << "\n";
    }
}

// Chuyen doi trang thai cua may: unique_ptr cu tu dong giai phong bo nho trang thai cu (RAII)
void VendingMachine::setState(std::unique_ptr<MachineState> newState) {
    currentState = std::move(newState);
}

// Tim kiem ngan chua bang thuat toan chuan std::find_if ket hop bieu thuc Lambda (Bai 9 - CLO4)
Slot* VendingMachine::findSlot(const std::string& slotCode) {
    // Bieu thuc lambda [&slotCode](const Slot& s) bat bien slotCode theo tham chieu de so sanh ma ngan
    auto it = std::find_if(slots.begin(), slots.end(), [&slotCode](const Slot& s) {
        return s.getCode() == slotCode;
    });

    if (it == slots.end()) {
        return nullptr; // Dung nullptr cua C++11 thay vi NULL cua C cu (Bai 2 - CLO1)
    }
    return &(*it); // Tra ve dia chi cua phan tu Slot tim thay
}

const Slot* VendingMachine::findSlot(const std::string& slotCode) const {
    auto it = std::find_if(slots.begin(), slots.end(), [&slotCode](const Slot& s) {
        return s.getCode() == slotCode;
    });
    return (it != slots.end()) ? &(*it) : nullptr;
}

Money VendingMachine::refundBalance() {
    Money refunded = currentBalance;
    currentBalance = Money{0}; // Reset so du trong may ve 0 sau khi tra lai tien
    return refunded;
}

// Ghi lich su ban hang ra file transactions.log (Bai 8 - Nhap/xuat luong tep - CLO4)
void VendingMachine::logTransaction(const std::string& message) const {
    // Mo file o che do std::ios::app (append - ghi noi tiep vao cuoi file).
    // Nho co che RAII cua std::ofstream, file se tu dong dong lai khi bien logFile ra khoi pham vi ham nay.
    std::ofstream logFile{logFilePath, std::ios::app};
    if (logFile.is_open()) {
        logFile << message << "\n";
    }
}

void VendingMachine::displayInventory() const {
    std::cout << "\n==================== MAY BAN HANG TU DONG ====================\n";
    std::cout << "Trang thai may : " << currentState->getStateName() << "\n";
    std::cout << "Kenh thanh toan: " << paymentStrategy->getMethodName() << "\n";
    std::cout << "So du hien tai : " << currentBalance << " | Mon dang chon: "
              << (selectedSlotCode.empty() ? "Chua chon" : selectedSlotCode) << "\n";
    std::cout << "--------------------------------------------------------------\n";
    std::cout << std::left << std::setw(6) << "Ma"
              << std::setw(28) << "San pham (Da hinh)"
              << std::setw(14) << "Gia tien"
              << "Ton kho\n";
    std::cout << "--------------------------------------------------------------\n";

    // Dung vong lap range-based for ket hop const auto& de duyet vector ma khong sao chep phan tu (Bai 2 - CLO1)
    for (const auto& slot : slots) {
        const Product* prod = slot.getProduct();
        std::cout << std::left << std::setw(6) << slot.getCode()
                  << std::setw(28) << (prod ? prod->getDescription() : "Trong") // Goi da hinh ham getDescription()
                  << std::setw(14) << (prod ? std::to_string(prod->getPrice().getAmount()) + " VND" : "0 VND")
                  << slot.getQuantity() << "/" << slot.getMaxCapacity()
                  << (slot.isEmpty() ? " (HET HANG)" : "") << "\n";
    }
    std::cout << "==============================================================\n";
}

// Cac phuong thuc giao tiep chinh: uy quyen (delegate) hoan toan cho doi tuong trang thai hien tai
void VendingMachine::selectProduct(const std::string& slotCode) {
    currentState->selectSlot(*this, slotCode);
}

void VendingMachine::insertMoney(long long amountVnd) {
    currentState->insertMoney(*this, Money{amountVnd});
}

void VendingMachine::cancelTransaction() {
    currentState->cancel(*this);
}

void VendingMachine::dispenseProduct() {
    currentState->dispense(*this);
}

void VendingMachine::refillSlot(const std::string& slotCode, int quantity) {
    Slot* slot = findSlot(slotCode);
    if (slot == nullptr) {
        throw InvalidSlotException{slotCode};
    }
    slot->refill(quantity);
    std::cout << "[Ky thuat]: Da nap them " << quantity << " san pham vao ngan " << slotCode
              << ". Ton kho moi: " << slot->getQuantity() << "\n";
    logTransaction("[REFILL] Ngan " + slotCode + " + " + std::to_string(quantity) + " san pham.");
}

// ============================================================================
// PHAN 2: CAI DAT CHUYEN DOI TRANG THAI (IDLE -> SELECTING -> PAYING -> DISPENSING)
// ============================================================================

// --- 1. IDLE STATE (MAY DANG RANH) ---
void IdleState::selectSlot(VendingMachine& machine, const std::string& slotCode) {
    Slot* slot = machine.findSlot(slotCode);
    if (slot == nullptr) {
        throw InvalidSlotException{slotCode}; // Nem ngoai le neu nhap sai ma ngan
    }
    if (slot->isEmpty()) {
        throw OutOfStockException{slotCode};  // Nem ngoai le OutOfStock theo dung de bai neu ngan het hang
    }

    machine.setSelectedSlotCode(slotCode);
    std::cout << "[Chon hang]: Ban da chon ngan " << slotCode << " - "
              << slot->getProduct()->getDescription()
              << " (Gia: " << slot->getProduct()->getPrice() << ").\n";

    // Chuyen trang thai tu Idle -> Selecting
    machine.setState(std::make_unique<SelectingState>());
}

void IdleState::insertMoney(VendingMachine& /*machine*/, const Money& /*amount*/) {
    std::cout << "[Thong bao]: Vui long chon san pham truoc khi nap tien!\n";
}

void IdleState::cancel(VendingMachine& /*machine*/) {
    std::cout << "[Thong bao]: May dang ranh, khong co giao dich nao de huy.\n";
}

void IdleState::dispense(VendingMachine& /*machine*/) {
    std::cout << "[Thong bao]: Ban chua chon san pham va thanh toan!\n";
}

// --- 2. SELECTING STATE (DA CHON HANG, CHO NAP TIEN) ---
void SelectingState::selectSlot(VendingMachine& machine, const std::string& slotCode) {
    // Cho phep khach doi y chon san pham khac khi chua nap tien
    Slot* slot = machine.findSlot(slotCode);
    if (slot == nullptr) {
        throw InvalidSlotException{slotCode};
    }
    if (slot->isEmpty()) {
        throw OutOfStockException{slotCode};
    }
    machine.setSelectedSlotCode(slotCode);
    std::cout << "[Doi san pham]: Da doi sang ngan " << slotCode << " - "
              << slot->getProduct()->getDescription()
              << " (Gia: " << slot->getProduct()->getPrice() << ").\n";
}

void SelectingState::insertMoney(VendingMachine& machine, const Money& amount) {
    // Goi da hinh phuong thuc thanh toan thong qua con tro IPaymentStrategy
    if (!machine.getPaymentStrategy()->processPayment(amount)) {
        return; // Neu tien khong hop le thi giu nguyen trang thai
    }

    machine.addBalance(amount);
    const Slot* slot = machine.findSlot(machine.getSelectedSlotCode());
    Money price = slot->getProduct()->getPrice();

    std::cout << "[Nap tien]: Tong so du hien tai: " << machine.getCurrentBalance()
              << " / Gia san pham: " << price << "\n";

    // Chuyen sang trang thai PayingState (neu chua du tien) hoac DispensingState (neu da du tien)
    if (machine.getCurrentBalance() >= price) {
        std::cout << "-> Da du tien thanh toan! Chuyen sang trang thai Xuat hang (Dispensing)...\n";
        machine.setState(std::make_unique<DispensingState>());
    } else {
        machine.setState(std::make_unique<PayingState>());
    }
}

void SelectingState::cancel(VendingMachine& machine) {
    std::cout << "[Huy giao dich]: Da huy chon ngan " << machine.getSelectedSlotCode() << ".\n";
    machine.setSelectedSlotCode("");
    machine.setState(std::make_unique<IdleState>()); // Quay ve trang thai Idle
}

void SelectingState::dispense(VendingMachine& machine) {
    const Slot* slot = machine.findSlot(machine.getSelectedSlotCode());
    throw InsufficientPaymentException{
        "Chua nap tien! Can thanh toan " + std::to_string(slot->getProduct()->getPrice().getAmount()) + " VND."
    };
}

// --- 3. PAYING STATE (DANG THANH TOAN DO DANG) ---
void PayingState::selectSlot(VendingMachine& /*machine*/, const std::string& /*slotCode*/) {
    std::cout << "[Thong bao]: Dang trong qua trinh thanh toan. Neu muon doi mon, vui long Huy (Cancel) truoc!\n";
}

void PayingState::insertMoney(VendingMachine& machine, const Money& amount) {
    if (!machine.getPaymentStrategy()->processPayment(amount)) {
        return;
    }

    machine.addBalance(amount);
    const Slot* slot = machine.findSlot(machine.getSelectedSlotCode());
    Money price = slot->getProduct()->getPrice();

    std::cout << "[Nap them tien]: Tong so du hien tai: " << machine.getCurrentBalance()
              << " / Gia san pham: " << price << "\n";

    // Kiem tra bang toan tu >= da nap chong cua lop Money
    if (machine.getCurrentBalance() >= price) {
        std::cout << "-> Da du tien thanh toan! Chuyen sang trang thai Xuat hang (Dispensing)...\n";
        machine.setState(std::make_unique<DispensingState>());
    }
}

void PayingState::cancel(VendingMachine& machine) {
    Money refunded = machine.refundBalance();
    std::cout << "[Huy & Hoan tien]: Giao dich bi huy. May tra lai ban: " << refunded << ".\n";
    machine.logTransaction("[CANCEL] Hoan tien cho khach: " + std::to_string(refunded.getAmount()) + " VND.");
    machine.setSelectedSlotCode("");
    machine.setState(std::make_unique<IdleState>());
}

void PayingState::dispense(VendingMachine& machine) {
    const Slot* slot = machine.findSlot(machine.getSelectedSlotCode());
    Money price = slot->getProduct()->getPrice();
    Money missing = price - machine.getCurrentBalance(); // Dung toan tu tru (-) da nap chong cua lop Money

    // Nem ngoai le InsufficientPayment dung chuan de bai khi khach bam xuat hang ma chua nap du tien
    throw InsufficientPaymentException{
        "So du hien tai (" + std::to_string(machine.getCurrentBalance().getAmount()) +
        " VND) chua du! Con thieu " + std::to_string(missing.getAmount()) + " VND."
    };
}

// --- 4. DISPENSING STATE (XUAT HANG & TRA TIEN THUA) ---
void DispensingState::selectSlot(VendingMachine& /*machine*/, const std::string& /*slotCode*/) {
    std::cout << "[Thong bao]: May dang chuan bi xuat hang, vui long nhan hang truoc!\n";
}

void DispensingState::insertMoney(VendingMachine& /*machine*/, const Money& /*amount*/) {
    std::cout << "[Thong bao]: Ban da nap du tien roi, khong can nap them!\n";
}

void DispensingState::cancel(VendingMachine& /*machine*/) {
    std::cout << "[Thong bao]: Giao dich da xac nhan du tien, khong the huy luc dang xuat hang!\n";
}

void DispensingState::dispense(VendingMachine& machine) {
    Slot* slot = machine.findSlot(machine.getSelectedSlotCode());
    Money price = slot->getProduct()->getPrice();

    // 1. Tru ton kho va day san pham ra khay
    slot->dispenseOne();
    std::cout << "\n>>> [XUAT HANG THANH CONG]: Ban nhan duoc 1 " << slot->getProduct()->getDescription() << "!\n";

    // 2. Tinh tien thua bang toan tu (-) cua lop Money va hoan tra cho khach
    Money totalPaid = machine.refundBalance();
    Money change = totalPaid - price;
    if (change.getAmount() > 0) {
        std::cout << ">>> [TRA TIEN THUA]: May tra lai ban " << change << ".\n";
    }

    // 3. Ghi log giao dich vao file transactions.log
    machine.logTransaction("[SALE] Da ban 1 " + slot->getProduct()->getName() +
                           " (Ngan " + slot->getCode() + ") - Gia: " +
                           std::to_string(price.getAmount()) + " VND - Tra thua: " +
                           std::to_string(change.getAmount()) + " VND.");

    // 4. Reset ma ngan va dua may quay tro ve trang thai Ranh (IdleState)
    machine.setSelectedSlotCode("");
    machine.setState(std::make_unique<IdleState>());
}
