# Đồ án Lập trình hướng đối tượng (C++) - Chủ đề 26: Máy bán hàng tự động

## 1. Thông tin nhóm và dự án
* **Tên dự án:** Chủ đề 26 - Máy bán hàng tự động (Vending Machine Simulation)
* **Danh sách thành viên (Nhóm 3 sinh viên):**
  1. **(Họ tên SV 1) - (MSSV):** Phụ trách lớp `Money`, cây kế thừa `Product` (`DrinkProduct`, `SnackProduct`), `ProductFactory` và lớp `Slot` (Quản lý bộ nhớ RAII với `std::unique_ptr`, nạp chồng toán tử).
  2. **(Họ tên SV 2) - (MSSV):** Phụ trách thiết kế và cài đặt **State Pattern** (`MachineState` -> `IdleState`, `SelectingState`, `PayingState`, `DispensingState`) và điều phối trạng thái trong `VendingMachine`.
  3. **(Họ tên SV 3) - (MSSV):** Phụ trách **Strategy Pattern** (`IPaymentStrategy` -> `CashPayment`, `QRPayment`), xử lý ngoại lệ (`OutOfStockException`, `InsufficientPaymentException`, `InvalidSlotException`), đọc/ghi File I/O và kịch bản kiểm thử `main.cpp`.

## 2. Cấu trúc thư mục
Dự án tuân thủ cấu trúc chuẩn của học phần:
* `include/`: Chứa các file header (`.h`): `Money.h`, `Product.h`, `Slot.h`, `PaymentStrategy.h`, `MachineState.h`, `VendingMachine.h`, `Exceptions.h`.
* `src/`: Chứa file định nghĩa thực thi (`.cpp`): `VendingMachine.cpp`.
* `data/`: Chứa dữ liệu tồn kho mẫu `inventory.txt` và nhật ký giao dịch `transactions.log`.
* `tests/`: Chứa bộ kiểm thử tự động `test_scenarios.cpp`.
* `docs/`: Chứa Cẩm nang bảo vệ đồ án, sơ đồ lớp UML và câu hỏi vấn đáp (`CAM_NANG_BAO_VE.md`).
* `CMakeLists.txt`: File cấu hình biên dịch tự động bằng CMake.
* `main.cpp`: Điểm bắt đầu chương trình giao diện Console và Auto-Demo.

## 3. Hướng dẫn biên dịch và chạy (Build & Run)

### Cách 1: Sử dụng CMake (Khuyên dùng theo chuẩn đồ án)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
* **Chạy chương trình chính:**
  * Trên Linux/macOS: `./app`
  * Trên Windows: `.\app.exe`
* **Chạy bộ kiểm thử tự động (Unit Tests):**
  * Trên Linux/macOS: `./run_tests`
  * Trên Windows: `.\run_tests.exe`

### Cách 2: Biên dịch trực tiếp bằng g++ (Nhanh gọn khi không cài CMake)
```bash
# Biên dịch và chạy chương trình chính
g++ -std=c++20 -Iinclude main.cpp src/VendingMachine.cpp -o app
./app

# Biên dịch và chạy bộ kiểm thử tự động
g++ -std=c++20 -Iinclude tests/test_scenarios.cpp src/VendingMachine.cpp -o run_tests
./run_tests
```

## 4. Kịch bản Demo cốt lõi (8 - 12 phút khi bảo vệ)
Khi chạy `./app`, bạn có thể bấm **Phím 7** để chạy tự động toàn bộ các kịch bản cốt lõi hoặc thao tác từng bước từ Phím 1 -> 6:
1. **Khởi tạo & File I/O (`std::ifstream`):** Tự động đọc danh mục sản phẩm từ `data/inventory.txt` vào `std::vector<Slot>`.
2. **Luồng chính & State Pattern:** Chọn ngăn `A1` (`Idle` -> `Selecting`) -> Nạp 5.000đ (`Selecting` -> `Paying`) -> Nạp tiếp 10.000đ (`Paying` -> `Dispensing`) -> Xuất 1 lon Coca Cola, trả tiền thừa 5.000đ (nhờ toán tử `-` của `Money`) và quay về `Idle`.
3. **Đa hình (Polymorphism):** Hiển thị mô tả sản phẩm đa hình qua `Product::getDescription()` cho `DrinkProduct` và `SnackProduct`.
4. **Strategy Pattern:** Đổi phương thức thanh toán lúc chạy (Runtime) từ Tiền mặt (`CashPayment`) sang Quét mã QR (`QRPayment`).
5. **STL & Lambda:** Tìm kiếm ngăn chứa bằng `std::find_if` kết hợp biểu thức `lambda` trong `VendingMachine::findSlot()`.
6. **Xử lý Ngoại lệ (Exceptions):** Bắt ngoại lệ `OutOfStockException` khi chọn ngăn `B2` đang hết hàng, và bắt `InsufficientPaymentException` khi bấm xuất hàng lúc chưa nạp đủ tiền.
7. **Ghi nhật ký (`std::ofstream`):** Mọi giao dịch bán hàng, hủy hoàn tiền và nạp hàng (Refill) đều được ghi tự động vào `data/transactions.log`.

## 5. Khai báo việc sử dụng AI
* **Công cụ sử dụng:** Google Gemini.
* **Phạm vi áp dụng:** Hỗ trợ gợi ý khung mã nguồn (boilerplate), cấu hình `CMakeLists.txt` và hệ thống hóa tài liệu ôn tập bảo vệ.
* **Cách thức kiểm duyệt:** Nhóm 3 sinh viên đã đọc hiểu từng dòng code, kiểm tra ràng buộc RAII (`std::unique_ptr`), chạy thử nghiệm thực tế toàn bộ kịch bản ngoại lệ và nắm vững nguyên lý SOLID để bảo vệ trước giảng viên.
