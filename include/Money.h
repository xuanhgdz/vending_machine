#ifndef MONEY_H
#define MONEY_H

#include <iostream>
#include <stdexcept>

// Lop value-type bieu dien tien te (VND), dap ung yeu cau nap chong toan tu cua Bai 4 (CLO2)
class Money {
private:
    long long amount; // Dong goi du lieu o pham vi private de bao ve tinh toan ven (khong cho phep tien am tu do)

public:
    // Ham tao mac dinh ket hop tham so mac dinh, dung danh sach khoi tao thanh vien (member initializer list)
    // de khoi tao truc tiep bien amount nhanh hon viec gan trong than ham (Bai 3 - CLO2)
    explicit Money(long long val = 0) : amount{val} {
        if (val < 0) {
            throw std::invalid_argument("So tien khoi tao khong duoc am!");
        }
    }

    // Getter co tu khoa const o cuoi: cam ket ham nay chi doc du lieu, khong lam thay doi doi tuong (const-correctness)
    long long getAmount() const {
        return amount;
    }

    // Nap chong toan tu cong (+) de cong hai khoan tien: tra ve mot doi tuong Money moi
    Money operator+(const Money& other) const {
        return Money{this->amount + other.amount};
    }

    // Nap chong toan tu tru (-) de tinh tien thua tra lai khach
    Money operator-(const Money& other) const {
        if (this->amount < other.amount) {
            throw std::underflow_error("Khong the tru ra so tien am!");
        }
        return Money{this->amount - other.amount};
    }

    // Nap chong toan tu cong gan (+=) de cong don tien khach nap vao may
    Money& operator+=(const Money& other) {
        this->amount += other.amount;
        return *this; // Tra ve tham chieu toi chinh doi tuong hien tai thong qua con tro this
    }

    // Nap chong cac toan tu so sanh (==, <, >=) phuc vu kiem tra du tien hay chua
    bool operator==(const Money& other) const { return this->amount == other.amount; }
    bool operator<(const Money& other) const { return this->amount < other.amount; }
    bool operator>=(const Money& other) const { return this->amount >= other.amount; }

    // Dung ham friend de nap chong toan tu xuat luong (<<) vi ve trai cua << la std::ostream chu khong phai Money
    friend std::ostream& operator<<(std::ostream& os, const Money& m) {
        os << m.amount << " VND";
        return os; // Tra ve tham chieu luong de co the noi chuoi: std::cout << m1 << m2;
    }
};

#endif // MONEY_H
