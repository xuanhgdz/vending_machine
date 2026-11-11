#ifndef SLOT_H
#define SLOT_H

#include "Product.h"
#include "Exceptions.h"
#include <string>
#include <memory>
#include <utility> // Chua std::move phuc vu chuyen quyen so huu unique_ptr (Bai 4 - CLO2)

// Lop Slot dai dien cho mot ngan chua hang trong may (Quan he HAS-A voi Product)
class Slot {
private:
    std::string code;                 // Ma ngan chua (VD: "A1", "A2", "B1")
    std::unique_ptr<Product> product; // Dung unique_ptr vi moi ngan chua so huu doc quyen mau san pham cua ngan do (RAII)
    int quantity;                     // So luong ton kho hien tai
    int maxCapacity;                  // Suc chua toi da cua ngan

public:
    // Nhan std::unique_ptr<Product> va dung std::move de chuyen quyen so huu vao thuoc tinh product cua Slot
    Slot(const std::string& slotCode, std::unique_ptr<Product> prod, int initialQty, int maxCap = 10)
        : code{slotCode}, product{std::move(prod)}, quantity{initialQty}, maxCapacity{maxCap} {}

    // Ap dung Rule of Zero: Vi lop dung std::unique_ptr va std::string tu quan ly bo nho,
    // ta khong can tu viet Destructor, Move Constructor mac dinh van hoat dong an toan.

    std::string getCode() const { return code; }
    int getQuantity() const { return quantity; }
    int getMaxCapacity() const { return maxCapacity; }
    bool isEmpty() const { return quantity <= 0; }

    // Tra ve con tro hang (const Product*) chi de doc thong tin san pham, KHONG chuyen giao quyen so huu
    const Product* getProduct() const {
        return product.get(); // Ham .get() tra ve con tro tho ben trong unique_ptr de quan sat
    }

    // Giam 1 san pham khi xuat hang thanh cong
    void dispenseOne() {
        if (isEmpty()) {
            throw OutOfStockException{code}; // Nem ngoai le neu co tinh xuat hang khi so luong = 0
        }
        --quantity;
    }

    // Nap them hang vao ngan (chuc nang Refill danh cho nhan vien ky thuat)
    void refill(int addedQuantity) {
        if (addedQuantity <= 0) {
            throw std::invalid_argument("So luong nap them phai lon hon 0!");
        }
        if (quantity + addedQuantity > maxCapacity) {
            quantity = maxCapacity; // Gioi han khong vuot qua suc chua toi da cua ngan
        } else {
            quantity += addedQuantity;
        }
    }
};

#endif // SLOT_H
