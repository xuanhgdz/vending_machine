#ifndef PAYMENT_STRATEGY_H
#define PAYMENT_STRATEGY_H

#include "Money.h"
#include <string>
#include <iostream>

// Interface (Lop thuan truu tuong) cho chien luoc thanh toan (Bai 6 & Bai 10 - CLO3, CLO5)
// Giup tuan thu nguyen ly DIP (Dependency Inversion Principle): VendingMachine chi phu thuoc vao Interface nay
class IPaymentStrategy {
public:
    // Luon co virtual destructor cho moi Interface
    virtual ~IPaymentStrategy() = default;

    // Phuong thuc thuan ao xu ly thanh toan mot so tien
    virtual bool processPayment(const Money& amount) const = 0;
    virtual std::string getMethodName() const = 0;
};

// Chien luoc 1: Thanh toan bang Tien mat (Cash)
class CashPayment : public IPaymentStrategy {
public:
    bool processPayment(const Money& amount) const override {
        // Kiem tra menh gia tien mat hop le tai Viet Nam (chia het cho 1000d va > 0)
        if (amount.getAmount() <= 0 || amount.getAmount() % 1000 != 0) {
            std::cout << "  [Canh bao]: May chi nhan tien mat menh gia boi so cua 1.000 VND!\n";
            return false;
        }
        std::cout << "  [Tien mat]: Da nhan " << amount << " vao khay doc tien.\n";
        return true;
    }

    std::string getMethodName() const override {
        return "Tien mat (Cash)";
    }
};

// Chien luoc 2: Thanh toan bang Quet ma QR (Vi dien tu / Ngan hang)
class QRPayment : public IPaymentStrategy {
private:
    std::string providerName; // Ten cong thanh toan (VD: VietQR, MoMo)

public:
    explicit QRPayment(const std::string& provider = "VietQR")
        : providerName{provider} {}

    bool processPayment(const Money& amount) const override {
        if (amount.getAmount() <= 0) {
            return false;
        }
        std::cout << "  [Cong " << providerName << "]: Xac thuc giao dich QR thanh cong so tien " << amount << ".\n";
        return true;
    }

    std::string getMethodName() const override {
        return "Quet ma QR (" + providerName + ")";
    }
};

#endif // PAYMENT_STRATEGY_H
