# Bàn giao công việc tuần 1

## Đối chiếu lịch đã lập

| Đầu việc tuần 1 | Sản phẩm hiện có | Trạng thái |
|---|---|---|
| Chốt 6 use case và quy ước tiền | 01_scope.md | Đã viết phạm vi kỹ thuật; nhóm đăng ký với giảng viên |
| Git và CMake | CMakeLists.txt, .gitignore, git-history.bundle | Repo/commit khởi tạo; build đã kiểm tra |
| Money, Product, Slot, Inventory | include/vending và src | Đã cài đặt và kiểm tra ràng buộc |
| UML v1 và cây lớp | uml_week1.svg, uml_week1.puml, state_week1.puml | Khớp các lớp chính của bản hiện tại |
| Bốn state và container đa hình | MachineState, VendingMachine | Có virtual/override, virtual destructor, unique_ptr |
| Khung menu và demo tối thiểu | ConsoleUI, DemoData, data/demo_week1.txt | Xem, tạo, chọn, hủy, quan sát đa hình |
| Build từ clone mới | docs/04_verification.md và evidence | Kiểm tra bằng Git bundle ở thư mục sạch |

## Phân vai để nhóm điền tên

| Vai trò | Phần phụ trách đọc và phát triển tiếp |
|---|---|
| A — Dữ liệu | Money, Product, Slot, Inventory; giữ invariant khi bổ sung cập nhật kho |
| B — Điều phối | VendingMachine, các state, ConsoleUI và UML |
| C — Giao dịch | Bắt đầu tuần 2 với Payment, IChangeStrategy, PurchaseService và an toàn ngoại lệ |
| D — Kiểm thử và lưu trữ | Duy trì tests/CMake/README; I/O và thống kê ở tuần 3 |

Chưa có tên sinh viên được cung cấp nên không ghi nhận đóng góp cho cá nhân cụ thể. Mỗi người cần đọc, chạy, chỉnh sửa và commit bằng tài khoản của mình. Commit hỗ trợ AI không được dùng làm bằng chứng rằng sinh viên đã tự viết phần tương ứng.

## Demo mốc 1 trong khoảng 4 phút

1. 30 giây: mở sơ đồ; chỉ ra cây MachineState, composition và nơi sở hữu unique_ptr.
2. 1 phút: build/chạy, xem dữ liệu, nhập mã sai và khe A2 hết hàng.
3. 1 phút: chọn A1, chỉ ra Idle → Selecting → Paying và tồn kho vẫn 2; hủy về Idle.
4. 1 phút: dùng menu 6, chỉ ra cùng handle qua base tạo bốn mô tả khác nhau; mở vị trí dispatch trong code.
5. 30 giây: chạy test và trình bày ràng buộc giá/tồn/sức chứa.

## Chuẩn bị cho tuần 2

- Thêm Payment quản lý credit và mệnh giá. Không đưa dữ liệu thanh toán vào ConsoleUI.
- Thêm IChangeStrategy, GreedyChangeStrategy và ExactPaymentStrategy; máy sở hữu policy qua smart pointer, truyền interface cho dịch vụ mua.
- Thêm sự kiện InsertMoney/ConfirmPurchase và xử lý thật tại Paying/Dispensing. Giữ nguyên ý nghĩa của Describe, StartSelection, ChooseSlot, Cancel.
- Thêm PurchaseService và Transaction; mua chỉ commit sau khi kiểm tra tiền, kho, tiền thừa và mô phỏng xuất thành công.
- Định nghĩa OutOfStock/InsufficientPayment/DispenseError; chuyển thông báo nghiệp vụ phù hợp sang ngoại lệ tại tầng cần thiết, không dùng ngoại lệ cho mọi bước chờ tiền.
- Thêm thao tác refill ở Inventory/VendingMachine, có guard Idle; không mở mutable map cho UI.
- Viết ca lỗi xuất hàng: đã nạp 20.000, giá 10.000, tồn 2; thất bại giữ nguyên credit/kho/lịch sử; hủy hoàn đủ.
- Cập nhật UML đồng thời với code; giữ các test tuần 1 làm kiểm tra hồi quy.
