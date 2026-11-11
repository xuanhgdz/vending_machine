#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#include "Money.h"
#include <string>

// Khai bao truoc (Forward Declaration) lop VendingMachine de pha vo su phu thuoc vong (circular dependency)
// giua MachineState va VendingMachine ma khong can #include "VendingMachine.h" tai day.
class VendingMachine;

// Lop co so truu tuong dai dien cho trang thai cua may ban hang (State Pattern)
class MachineState {
public:
    // Ham huy ao bat buoc de tranh memory leak khi VendingMachine thay doi trang thai (xoa state cu qua unique_ptr<MachineState>)
    virtual ~MachineState() = default;

    // 4 hanh vi tuong ung voi cac su kien tren may ban hang
    virtual void selectSlot(VendingMachine& machine, const std::string& slotCode) = 0;
    virtual void insertMoney(VendingMachine& machine, const Money& amount) = 0;
    virtual void cancel(VendingMachine& machine) = 0;
    virtual void dispense(VendingMachine& machine) = 0;

    // Tra ve ten trang thai hien tai de hien thi len man hinh console
    virtual std::string getStateName() const = 0;
};

// Trang thai 1: May dang ranh (Idle) - Cho khach chon san pham
class IdleState : public MachineState {
public:
    void selectSlot(VendingMachine& machine, const std::string& slotCode) override;
    void insertMoney(VendingMachine& machine, const Money& amount) override;
    void cancel(VendingMachine& machine) override;
    void dispense(VendingMachine& machine) override;
    std::string getStateName() const override { return "IDLE (San sang phuc vu)"; }
};

// Trang thai 2: Da chon san pham (Selecting) - Cho khach nap khoan tien dau tien hoac doi sang mon khac
class SelectingState : public MachineState {
public:
    void selectSlot(VendingMachine& machine, const std::string& slotCode) override;
    void insertMoney(VendingMachine& machine, const Money& amount) override;
    void cancel(VendingMachine& machine) override;
    void dispense(VendingMachine& machine) override;
    std::string getStateName() const override { return "SELECTING (Da chon mon - Cho nap tien)"; }
};

// Trang thai 3: Dang thanh toan (Paying) - Khach dang nap tien, kiem tra du tien de chuyen sang xuat hang
class PayingState : public MachineState {
public:
    void selectSlot(VendingMachine& machine, const std::string& slotCode) override;
    void insertMoney(VendingMachine& machine, const Money& amount) override;
    void cancel(VendingMachine& machine) override;
    void dispense(VendingMachine& machine) override;
    std::string getStateName() const override { return "PAYING (Dang nap tien)"; }
};

// Trang thai 4: Dang xuat hang & tra tien thua (Dispensing)
class DispensingState : public MachineState {
public:
    void selectSlot(VendingMachine& machine, const std::string& slotCode) override;
    void insertMoney(VendingMachine& machine, const Money& amount) override;
    void cancel(VendingMachine& machine) override;
    void dispense(VendingMachine& machine) override;
    std::string getStateName() const override { return "DISPENSING (Dang xuat hang)"; }
};

#endif // MACHINE_STATE_H
