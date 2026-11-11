#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept> // Thu vien chuan chua std::runtime_error (Bai 7 - CLO4)
#include <string>

// Ke thua tu std::runtime_error de tan dung co che da hinh cua he thong ngoai le chuan C++ (ham what())
class OutOfStockException : public std::runtime_error {
public:
    // Dung tu khoa explicit de ngan trinh bien dich tu dong ep kieu ngam dinh tu std::string sang Exception
    // Dung const std::string& de truyen tham chieu hang, tranh viec copy chuoi gay ton bo nho (Bai 2 - CLO1)
    explicit OutOfStockException(const std::string& slotCode)
        : std::runtime_error("[Loi Nghiep Vu - OutOfStock]: Ngan chua '" + slotCode + "' da het hang!") {}
};

// Ngoai le nem ra khi so tien khach nap vao chua du de mua san pham da chon
class InsufficientPaymentException : public std::runtime_error {
public:
    explicit InsufficientPaymentException(const std::string& detail)
        : std::runtime_error("[Loi Nghiep Vu - InsufficientPayment]: " + detail) {}
};

// Ngoai le nem ra khi nguoi dung nhap ma ngan chua khong ton tai (VD: nhap Z99)
class InvalidSlotException : public std::runtime_error {
public:
    explicit InvalidSlotException(const std::string& slotCode)
        : std::runtime_error("[Loi Nghiep Vu - InvalidSlot]: Ma ngan '" + slotCode + "' khong ton tai!") {}
};

#endif // EXCEPTIONS_H
