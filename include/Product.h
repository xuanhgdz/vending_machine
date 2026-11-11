#ifndef PRODUCT_H
#define PRODUCT_H

#include "Money.h"
#include <string>
#include <memory> // Thu vien chua std::unique_ptr va std::make_unique (Bai 3 - CLO2)

// Lop co so truu tuong (Abstract Base Class) dai dien cho moi san pham trong may (Bai 6 - CLO3)
class Product {
protected:
    // De protected de cac lop con (DrinkProduct, SnackProduct) co the truy cap truc tiep khi can
    std::string name;
    Money price;

public:
    Product(const std::string& productName, Money productPrice)
        : name{productName}, price{productPrice} {}

    // QUAN TRONG: Dung virtual destructor (ham huy ao) o lop cha.
    // Ly do: Khi xoa mot doi tuong lop con thong qua con tro lop cha (unique_ptr<Product>),
    // neu khong co virtual, trinh bien dich chi goi ham huy cua Product ma bo qua ham huy cua lop con, gay ro ri bo nho (memory leak).
    virtual ~Product() = default;

    std::string getName() const { return name; }
    Money getPrice() const { return price; }

    // Ham thuan ao (pure virtual function = 0): bien Product thanh lop truu tuong.
    // Bat buoc moi lop con ke thua tu Product phai tu dinh nghia lai (override) cach mo ta san pham.
    virtual std::string getDescription() const = 0;
};

// Lop con DrinkProduct ke thua cong khai (public inheritance - quan he IS-A) tu Product
class DrinkProduct : public Product {
private:
    bool isChilled; // Thuoc tinh rieng cua do uong: co uop lanh hay khong

public:
    DrinkProduct(const std::string& name, Money price, bool chilled)
        : Product{name, price}, isChilled{chilled} {} // Goi ham tao cua lop cha trong danh sach khoi tao

    // Tu khoa override giup trinh bien dich kiem tra xem ham nay co khop chu ky voi ham ao o lop cha khong
    std::string getDescription() const override {
        return name + (isChilled ? " [Uop lanh]" : " [Binh thuong]");
    }
};

// Lop con SnackProduct ke thua cong khai tu Product
class SnackProduct : public Product {
private:
    int weightGrams; // Thuoc tinh rieng cua do an vat: khoi luong tinh bang gram

public:
    SnackProduct(const std::string& name, Money price, int weight)
        : Product{name, price}, weightGrams{weight} {}

    std::string getDescription() const override {
        return name + " [" + std::to_string(weightGrams) + "g]";
    }
};

// Ap dung Simple Factory Pattern de tao san pham tap trung, tach biet logic khoi tao khoi VendingMachine
class ProductFactory {
public:
    // Tra ve std::unique_ptr<Product> de chuyen giao quyen so huu doc quyen doi tuong vua tao cho noi goi
    static std::unique_ptr<Product> createDrink(const std::string& name, long long priceVnd, bool chilled) {
        return std::make_unique<DrinkProduct>(name, Money{priceVnd}, chilled);
    }

    static std::unique_ptr<Product> createSnack(const std::string& name, long long priceVnd, int weightGrams) {
        return std::make_unique<SnackProduct>(name, Money{priceVnd}, weightGrams);
    }
};

#endif // PRODUCT_H
