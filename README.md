# Máy bán hàng tự động — Mốc tuần 1

Dự án C++20 cho chủ đề 26 môn OOP. Bản này hoàn thành phần kỹ thuật tuần 1: lớp dữ liệu có ràng buộc, cây kế thừa 5 lớp, container đa hình, menu console, UML v1 và build bằng CMake.

**Chạy được:** xem hàng, thêm khe hàng, bắt đầu chọn, chọn một khe còn hàng, hủy lựa chọn và xem cách cùng một sự kiện được xử lý bởi bốn lớp trạng thái. Chọn hàng chưa trừ tồn kho. Phiên bản này chưa nhận tiền, trả tiền, xuất hàng hoặc lưu tệp nghiệp vụ.

## Bắt đầu nhanh

Cần trình biên dịch hỗ trợ C++20 và CMake từ 3.16. Python 3 là tùy chọn để chạy thêm các kịch bản console; kiểm thử C++ vẫn chạy khi không có Python. Không cần thư viện C++ bên ngoài.

Linux hoặc macOS với trình biên dịch phù hợp:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/vending_app
```

Windows với Visual Studio và bộ công cụ C++ đã cài, chạy trong Developer PowerShell:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
.\build\Debug\vending_app.exe
```

Nếu dùng MinGW với generator đơn cấu hình, file chạy thường ở `build/vending_app.exe`. Phần xác minh đi kèm được thực hiện trên Linux/GCC; hướng dẫn Windows chưa được chạy trực tiếp trong môi trường đó.

Có thể build ứng dụng trực tiếp bằng GCC mà không cần CMake:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Iinclude src/*.cpp -o vending_app
./vending_app
```

## Thử ngay

1. Chọn `1` để xem A1, A2, B1. A2 cố ý có tồn kho bằng 0.
2. Chọn `2`: máy chuyển từ `Idle` sang `Selecting`.
3. Chọn `3`, nhập `A2`: máy báo hết hàng, vẫn `Selecting`.
4. Chọn `3`, nhập `A1`: máy sang `Paying`, ghi nhớ khe A1, tồn kho vẫn là 2.
5. Chọn `6`: cùng sự kiện `Describe` được gọi qua bốn đối tượng `MachineState`; mỗi lớp trả mô tả khác nhau, không đổi phiên đang chọn.
6. Chọn `4`: hủy, về `Idle`, xóa khe đã chọn, tồn kho không đổi.
7. Chọn `5` để tạo thêm một khe khi `Idle`; ví dụ `C1`, `COOKIE`, `Banh quy`, `15000`, `4`, `10`.
8. Chọn `0` để thoát. Dữ liệu thêm trong phiên chỉ ở bộ nhớ.

Chạy kịch bản nhập sẵn trên Linux/macOS:

```bash
./build/vending_app < data/demo_week1.txt
./build/vending_app < data/invalid_input.txt
```

Console dùng tiếng Việt không dấu để dễ chạy trên các terminal Windows khác bảng mã. Mã khe và mã hàng phân biệt hoa/thường.

## Tổ chức mã

| Đường dẫn | Nội dung |
|---|---|
| `include/vending/` | Khai báo các lớp, enum và cấu trúc dữ liệu |
| `src/` | Cài đặt .cpp, dữ liệu khởi tạo và main |
| `tests/` | 18 nhóm kiểm thử C++ và 5 kịch bản console |
| `data/` | Dữ liệu nhập bàn phím cho demo và lỗi |
| `docs/01_scope.md` | Sáu use case, quy ước và phần hoàn thành ở tuần 1 |
| `docs/02_design.md` | Trách nhiệm lớp, ownership, invariant, bảng trạng thái |
| `docs/03_week1_handover.md` | Đối chiếu đầu việc, phân vai và việc chuyển sang tuần 2 |
| `docs/04_verification.md` | Môi trường và kết quả kiểm tra thực tế |
| `docs/05_ai_disclosure.md` | Khai báo phần hỗ trợ AI, cần bổ sung xác nhận của sinh viên |
| `docs/uml_week1.svg` | Sơ đồ lớp có thể mở bằng trình duyệt |
| `docs/uml_week1.puml` | Nguồn PlantUML của sơ đồ lớp |
| `docs/state_week1.puml` | Nguồn sơ đồ trạng thái của phiên bản hiện tại |
| `docs/evidence/` | Log kiểm thử và kết quả chạy demo |

## Kiến thức OOP có thể chỉ ra trong mã

- `Money`, `Product`, `Slot` dùng thuộc tính private và kiểm tra trong constructor; `Slot::refill` kiểm tra trước khi thay đổi tồn kho.
- `MachineState` là lớp trừu tượng có hàm hủy ảo, `IdleState`/`SelectingState`/`PayingState`/`DispensingState` dùng `override`.
- `VendingMachine` sở hữu `vector<unique_ptr<MachineState>>`. `dispatch()` gọi `handle()` qua `const MachineState&`, không phân nhánh theo loại lớp con.
- `describeStates()` gửi cùng sự kiện tới bốn state qua con trỏ base. `DispensingState` mới là khung, chưa có đường chuyển từ giao dịch vào trạng thái này.
- Quan hệ `Slot` có `Product`, `Inventory` có các `Slot`, `VendingMachine` có `Inventory` là composition, không phải kế thừa.
- `map` tra mã khe, `vector` chứa đa hình; `find_if` và lambda tìm state; `Money` có toán tử và xuất luồng; dùng `make_unique`, `std::move` và Rule of Zero.

Các ngoại lệ nghiệp vụ tự định nghĩa, Strategy thanh toán, hoàn tiền, commit giao dịch và I/O là phần tuần 2–3. `State` hiện tại không thay thế yêu cầu phải có `Strategy` hoặc `Factory` trong đồ án cuối.

## Làm việc với Git

Gói bàn giao có `git-history.bundle` chứa commit khởi tạo được ghi bằng tác giả `Codex (AI assistant)`. Đây không phải lịch sử đóng góp của sinh viên. Có thể tạo bản clone độc lập:

```bash
git clone git-history.bundle VendingMachineWeek1-clone
cd VendingMachineWeek1-clone
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

File bundle không tự chứa chính nó. Bản clone có toàn bộ mã, sơ đồ và tài liệu đã commit. Nhóm cần cấu hình tên/email Git thật của từng thành viên và tự commit những thay đổi tiếp theo. Chưa tạo remote hoặc gửi nội dung cho giảng viên.

## Căn cứ

- `Ke_hoach_do_an_OOP_Chu_de_26.docx`, phần lộ trình tuần 1 và thiết kế đã thống nhất trong cuộc trao đổi.
- `ThucHanh_DoAn_OOP_Cpp(2).docx`, Lab 01–06 là trọng tâm của mốc đầu.
- `Đồ án môn OOP(1).docx`, yêu cầu Mốc 1: UML v1, cây lớp, khung kế thừa, container đa hình, demo tối thiểu.
