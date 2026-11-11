#ifndef VENDING_MACHINE_H
#define VENDING_MACHINE_H

#include "Slot.h"
#include "PaymentStrategy.h"
#include "MachineState.h"
#include <vector>
#include <memory>
#include <string>

// Lop Context trung tam dieu phoi toan bo hoat dong cua may ban hang
class VendingMachine {
private:
    std::vector<Slot> slots;                           // Dung std::vector de luu danh sach ngan chua lien tiep trong bo nho (Bai 9 - CLO4)
    std::unique_ptr<MachineState> currentState;        // Con tro thong minh giu trang thai hien tai cua may (State Pattern)
    std::unique_ptr<IPaymentStrategy> paymentStrategy; // Con tro thong minh giu phuong thuc thanh toan hien tai (Strategy Pattern)
    Money currentBalance;                              // So tien khach da nap vao trong phien giao dich hien tai
    std::string selectedSlotCode;                      // Ma ngan chua ma khach dang chon
    std::string logFilePath;                           // Duong dan file ghi nhat ky giao dich

public:
    explicit VendingMachine(const std::string& logPath = "data/transactions.log");

    // Cam sao chep (Copy Constructor & Copy Assignment = delete) vi mot cai may ban hang vat ly
    // khong the bi "nhan ban" ngau nhien va ben trong no chua std::unique_ptr (Bai 3 - CLO2)
    VendingMachine(const VendingMachine&) = delete;
    VendingMachine& operator=(const VendingMachine&) = delete;

    // --- Cac ham cau hinh, doc file & hien thi ---
    void addSlot(Slot&& newSlot);
    bool loadInventoryFromFile(const std::string& filePath); // Doc danh muc san pham tu file (Bai 8 - CLO4)
    void setPaymentStrategy(std::unique_ptr<IPaymentStrategy> strategy);
    void setState(std::unique_ptr<MachineState> newState);
    void displayInventory() const;

    // --- 5 Kich ban nghiep vu chinh giao tiep voi nguoi dung (uy quyen cho currentState) ---
    void selectProduct(const std::string& slotCode);
    void insertMoney(long long amountVnd);
    void cancelTransaction();
    void dispenseProduct();
    void refillSlot(const std::string& slotCode, int quantity);

    // --- Cac ham ho tro noi bo cho cac lop State goi ---
    Slot* findSlot(const std::string& slotCode); // Tim ngan chua bang std::find_if + Lambda
    const Slot* findSlot(const std::string& slotCode) const;

    Money getCurrentBalance() const { return currentBalance; }
    void addBalance(const Money& amount) { currentBalance += amount; }
    Money refundBalance();

    std::string getSelectedSlotCode() const { return selectedSlotCode; }
    void setSelectedSlotCode(const std::string& code) { selectedSlotCode = code; }

    const IPaymentStrategy* getPaymentStrategy() const { return paymentStrategy.get(); }
    std::string getCurrentStateName() const { return currentState->getStateName(); }

    // Ghi nhat ky giao dich ra file van ban (Bai 8 - Nhap/xuat tep - CLO4)
    void logTransaction(const std::string& message) const;
};

#endif // VENDING_MACHINE_H
