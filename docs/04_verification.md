# Kết quả xác minh tuần 1

Kiểm tra ngày 14/10/2026. Mã nguồn được clone sang thư mục mới, không mang theo build cache hoặc executable từ thư mục phát triển.

## Môi trường đã chạy

- Linux x86_64.
- g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0.
- cmake version 4.4.4.
- Python 3.12.14 cho kịch bản console.
- C++20; CMake bật -Wall -Wextra -Wpedantic -Wconversion -Wshadow.
- Commit mã nguồn được kiểm tra: `ae01d38eeb55956130dd6b2d3e5a526db55094aa`.

## Kết quả thực tế

| Kiểm tra | Kết quả | Bằng chứng |
|---|---|---|
| Configure và build từ clone sạch | Thành công; không có cảnh báo compiler trong log | evidence/01_configure.txt, 02_build.txt |
| Kiểm thử C++ | 18/18 nhóm đạt | evidence/04_cpp_tests.txt |
| Kịch bản console | 5/5 nhóm đạt, gồm EOF trong khi nhập | evidence/05_console_tests.txt |
| CTest | 2/2 test suite đạt | evidence/03_ctest.txt |
| Demo tạo/chọn/hủy/đa hình | Exit 0; có chuyển Idle → Selecting → Paying → Idle | evidence/06_demo.txt |
| Build trực tiếp bằng g++ theo README | Thành công; chạy chọn A1 và hủy được | evidence/07_direct_gcc_build.txt, 08_direct_gcc_smoke.txt |
| UML | Đã mở và kiểm tra bản SVG, đối chiếu cây lớp và ownership | uml_week1.svg, uml_week1.puml |

Sau commit mã trên chỉ bổ sung tài liệu xác minh và log; không sửa mã C++, CMake hoặc test. Gói phát hành chứa đầy đủ mã nguồn, sơ đồ, dữ liệu nhập và Git bundle.

## Phạm vi đã kiểm tra

Các test kiểm tra tiền âm/tràn, phép toán tiền, dữ liệu Product sai, tồn/sức chứa sai, refill lỗi không đổi kho, lấy sản phẩm cuối, mã trùng không ghi đè, trạng thái ban đầu, chọn sai/hết hàng, hủy, guard thêm khe và dispatch qua bốn lớp con. Console kiểm tra cả văn bản không phải số, số tràn, ký tự thừa và kết thúc đầu vào.

Không ghi nhận Windows/MSVC đã được kiểm thử; cần chạy lại trên máy nhóm. Chưa kiểm thử thanh toán, trả tiền thừa, xuất hàng và lưu/đọc nghiệp vụ vì các phần này chưa thuộc code tuần 1.
