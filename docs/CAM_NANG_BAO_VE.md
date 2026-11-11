# CẨM NANG BẢO VỆ ĐỒ ÁN OOP - CHỦ ĐỀ 26: MÁY BÁN HÀNG TỰ ĐỘNG

## 1. Sơ đồ Cấu trúc Lớp (UML Class Diagram)
```text
+-----------------------------------------------------------------------------------+
|                                  <<interface>>                                    |
|                                IPaymentStrategy                                   |
|-----------------------------------------------------------------------------------|
| + virtual ~IPaymentStrategy() = default                                           |
| + processPayment(amount: const Money&): bool {pure virtual}                       |
| + getMethodName(): string {pure virtual}                                          |
+-----------------------------------------------------------------------------------+
                     ^                                        ^
                     | (IS-A)                                 | (IS-A)
        +-------------------------+              +-------------------------+
        |       CashPayment       |              |        QRPayment        |
        +-------------------------+              +-------------------------+

+---------------------------------+              +----------------------------------+
|          Product                |              |             Money                |
|---------------------------------|              |----------------------------------|
| # name: string                  |              | - amount: long long              |
| # price: Money                  |              |----------------------------------|
|---------------------------------|              | + operator+(other): Money        |
| + virtual ~Product() = default  |              | + operator-(other): Money        |
| + getDescription(): string {pv} |              | + operator>=(other): bool        |
+---------------------------------+              +----------------------------------+
         ^               ^
  (IS-A) |               | (IS-A)
+----------------+ +---------------+             +----------------------------------+
|  DrinkProduct  | | SnackProduct  |             |              Slot                |
+----------------+ +---------------+             |----------------------------------|
                                                 | - code: string                   |
                                                 | - product: unique_ptr<Product>   |
                                                 | - quantity: int                  |
                                                 +----------------------------------+
                                                                  ^
                                                                  | (HAS-A: vector<Slot>)
+-----------------------------------------------------------------------------------+
|                                  VendingMachine                                   |
|-----------------------------------------------------------------------------------|
| - slots: vector<Slot>                                                             |
| - currentState: unique_ptr<MachineState>                                          |
| - paymentStrategy: unique_ptr<IPaymentStrategy>                                   |
| - currentBalance: Money                                                           |
| - selectedSlotCode: string                                                        |
+-----------------------------------------------------------------------------------+
         | (HAS-A: unique_ptr<MachineState>)
         v
+-----------------------------------------------------------------------------------+
|                                 <<abstract>>                                      |
|                                 MachineState                                      |
|-----------------------------------------------------------------------------------|
| + virtual ~MachineState() = default                                               |
| + selectSlot(machine: VendingMachine&, code: string): void {pure virtual}         |
| + insertMoney(machine: VendingMachine&, amount: Money): void {pure virtual}       |
| + cancel(machine: VendingMachine&): void {pure virtual}                           |
| + dispense(machine: VendingMachine&): void {pure virtual}                         |
+-----------------------------------------------------------------------------------+
       ^                       ^                        ^                      ^
       | (IS-A)                | (IS-A)                 | (IS-A)               | (IS-A)
+--------------+      +------------------+     +----------------+     +-------------------+
|  IdleState   |      |  SelectingState  |     |  PayingState   |     |  DispensingState  |
+--------------+      +------------------+     +----------------+     +-------------------+
```

## 2. Phân công Nhiệm vụ Bảo vệ cho 3 Thành viên
1. **Thành viên 1 (`Money.h`, `Product.h`, `Slot.h`):** Giải thích đóng gói, nạp chồng toán tử trong `Money`, kế thừa đa hình trong `Product`, Simple Factory và quản lý bộ nhớ bằng `std::unique_ptr` trong `Slot`.
2. **Thành viên 2 (`MachineState.h`, phần State trong `VendingMachine.cpp`):** Giải thích luồng chuyển trạng thái của **State Pattern** (`Idle` -> `Selecting` -> `Paying` -> `Dispensing`), cơ chế VTable và Forward Declaration.
3. **Thành viên 3 (`PaymentStrategy.h`, `Exceptions.h`, `VendingMachine.h`, `main.cpp`):** Giải thích **Strategy Pattern**, xử lý ngoại lệ `try-catch-throw`, thuật toán `std::find_if` + `lambda`, và đọc/ghi file (`std::ifstream` / `std::ofstream`).

## 3. Tóm tắt 7 Câu hỏi Vấn đáp Trọng tâm
1. **VTable hoạt động thế nào trong State Pattern? Bỏ `virtual` thì sao?**
   - Có `virtual`: C++ dùng VTable và con trỏ ẩn `vptr` để gọi đúng hàm của trạng thái hiện tại lúc chạy (Dynamic Dispatch).
   - Bỏ `virtual`: Gắn kết tĩnh (Static Binding) lúc biên dịch, con trỏ `MachineState*` chỉ gọi hàm của lớp cha, máy không thể hoạt động theo trạng thái.
2. **Tại sao bắt buộc có `virtual ~Product() = default;` và `virtual ~MachineState() = default;`?**
   - Để khi `unique_ptr<Product>` hoặc `unique_ptr<MachineState>` hủy đối tượng lớp con thông qua con trỏ lớp cha, hàm hủy của lớp con được gọi đầy đủ, ngăn chặn rò rỉ bộ nhớ (memory leak).
3. **Tại sao dùng `std::unique_ptr` thay vì con trỏ thô `*` hoặc `std::shared_ptr`?**
   - Tuân thủ RAII, tự động thu hồi bộ nhớ ngay cả khi có ngoại lệ (`throw`), thể hiện đúng quyền sở hữu độc quyền (Exclusive Ownership) và không tốn chi phí đếm tham chiếu như `shared_ptr`.
4. **Tại sao phải dùng `std::move` với `unique_ptr` và `= delete` ở `VendingMachine`?**
   - `unique_ptr` cấm copy nên phải dùng `std::move` để chuyển quyền sở hữu. `VendingMachine` chứa `unique_ptr` và đại diện cho một máy vật lý duy nhất nên xóa Copy Constructor (`= delete`).
5. **Tại sao dùng `std::vector<Slot>` và `std::find_if` + Lambda?**
   - `std::vector` lưu bộ nhớ liên tiếp tối ưu bộ nhớ đệm CPU. `std::find_if` kết hợp lambda `[&slotCode](const Slot& s)` giúp code ngắn gọn, hiện đại, chuẩn STL.
6. **Khi ném `InsufficientPaymentException`, tiền khách nạp có bị mất không?**
   - Không. Lệnh `throw` xảy ra trước khi trừ tồn kho và trước khi trừ tiền, nên trạng thái `PayingState` và số dư `currentBalance` được giữ nguyên an toàn.
7. **Làm sao thêm phương thức thanh toán Thẻ (`CardPayment`) mà không sửa code `VendingMachine`?**
   - Nhờ nguyên lý OCP (Open/Closed Principle) và Strategy Pattern, chỉ cần tạo lớp `CardPayment : public IPaymentStrategy` rồi truyền vào `machine.setPaymentStrategy(std::make_unique<CardPayment>())`.
