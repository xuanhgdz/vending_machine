# Phạm vi đề tài và sáu kịch bản lõi

Mục tiêu: mô phỏng một máy bán hàng tự động bằng console C++20. Phần kỹ thuật được chốt để có thể triển khai ngay; việc đăng ký đề tài, danh sách thành viên và phê duyệt phạm vi với giảng viên do nhóm thực hiện.

## Quy ước nghiệp vụ

- Mỗi phiên chọn và mua một sản phẩm; chọn trước, nạp tiền sau; chỉ trừ hàng khi giao dịch hoàn tất.
- Mã khe/mã hàng gồm 1–20 ký tự ASCII chữ, số, `_`, `-`, phân biệt hoa/thường. Mã khe phải duy nhất. Cùng một mã hàng có thể nằm ở nhiều khe.
- Tên sản phẩm có 1–80 byte, có ít nhất một ký tự khác khoảng trắng, không có ký tự điều khiển. Giới hạn byte được dùng thống nhất trong code; chưa chuẩn hóa Unicode.
- `Money` chứa `int64_t` đồng không âm. Giá phải lớn hơn 0 và là bội số 1.000. Không dùng `double` để giữ tiền.
- `capacity > 0`, `0 <= quantity <= capacity`. Thêm khe chỉ được thực hiện tại `Idle`.
- Bản hoàn chỉnh dự kiến nhận mệnh giá 1.000/2.000/5.000/10.000/20.000/50.000 đồng; bản tuần 1 chưa có thao tác nạp tiền.
- Bản lõi giả định đủ tiền lẻ; thanh toán thực, phần cứng, GUI, kho tiền lẻ hữu hạn và mua nhiều hàng trong một lượt nằm ngoài phạm vi lõi.
- Tuần 1 dùng dữ liệu khởi tạo trong `DemoData.cpp`. Dữ liệu thêm chỉ tồn tại trong phiên; file trong `data/` là kịch bản nhập, không phải dữ liệu lưu/đọc nghiệp vụ.

## UC01 Xem và chọn hàng

Tác nhân: khách mua. Tiền điều kiện: máy được khởi tạo, ở `Idle` khi bắt đầu chọn. Luồng: xem danh sách → bắt đầu → nhập mã khe → kiểm tra tồn kho → ghi nhận lựa chọn và chuyển sang `Paying`. Hậu điều kiện: có đúng một mã khe được chọn; tồn kho không thay đổi. Sai mã/hết hàng: giữ `Selecting`, cho chọn lại hoặc hủy. Trạng thái: **đã hoàn thành trong tuần 1**.

## UC02 Nạp tiền và mua hàng

Tác nhân: khách mua. Tiền điều kiện: ở `Paying`, đã chọn hàng còn tồn. Luồng dự kiến: nạp mệnh giá hợp lệ → xác nhận → policy kiểm tra tiền thừa → mô phỏng xuất → commit tồn kho, số dư và lịch sử. Thiếu tiền hoặc lỗi xuất: giữ tiền và kho an toàn để nạp tiếp, thử lại hoặc hủy. Trạng thái: **tuần 2**, chưa triển khai; không có thao tác giả thành công trong bản tuần 1.

## UC03 Hủy và hoàn tiền

Tác nhân: khách mua. Được hủy tại `Selecting` hoặc `Paying`, không hủy trong lúc xuất hàng. Tuần 1 hủy lựa chọn, về `Idle`, giữ nguyên kho; chưa nhận tiền nên chưa có hoàn tiền. Tuần 2 bổ sung hoàn toàn bộ credit đúng một lần. Trạng thái: **đã làm phần hủy lựa chọn; hoàn tiền ở tuần 2**.

## UC04 Tiếp hàng

Tác nhân: người vận hành. Tiền điều kiện: máy `Idle`; mã khe tồn tại. Nhập số lượng → kiểm tra dương và không vượt capacity → cập nhật kho. Trạng thái: **lớp Slot đã có refill và kiểm thử; thao tác tiếp hàng trên máy/menu ở tuần 2**. Chức năng thêm khe mới trong tuần 1 dùng để tạo đối tượng, không được ghi nhận thay cho use case tiếp hàng.

## UC05 Tra cứu và thống kê

Tác nhân: người vận hành/khách mua tùy truy vấn. Luồng dự kiến: lọc hàng còn/hết, sắp theo giá, thống kê doanh thu từ giao dịch hoàn tất; kết quả dùng STL, lambda/template. Trạng thái: **tuần 3**. Việc liệt kê map trong tuần 1 không được tính là đã hoàn thành toàn bộ use case này.

## UC06 Lưu và khôi phục

Tác nhân: người vận hành. Tiền điều kiện dự kiến: máy `Idle`. Ghi snapshot phiên bản hóa; đọc vào dữ liệu tạm, kiểm tra tất cả trường/mã trùng rồi mới thay dữ liệu hiện hành. Lỗi tệp không làm mất trạng thái đang có. Trạng thái: **tuần 3**.

## Ranh giới hoàn thành tuần 1

Mốc đầu đạt khi có thể build từ bản clone sạch, tạo sản phẩm/khe hợp lệ, từ chối dữ liệu sai, chọn hàng bằng xử lý đa hình, hủy mà không đổi kho và giải thích được ownership. Không yêu cầu máy đã bán hàng thực sự tại mốc này.
