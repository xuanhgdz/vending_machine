# Thiết kế lớp và UML phiên bản 1

## Danh từ động từ và trách nhiệm

| Khái niệm | Lớp | Thuộc tính chính | Thao tác chính |
|---|---|---|---|
| Tiền | Money | dong_ | cộng/trừ/so sánh/in tiền |
| Sản phẩm | Product | id_, name_, price_ | đọc thông tin đã kiểm tra |
| Khe hàng | Slot | code_, product_, quantity_, capacity_ | refill, takeOne, inStock |
| Kho khe | Inventory | slots_ | addSlot, find, slots |
| Trạng thái | MachineState | không có dữ liệu sở hữu | id, name, handle thuần ảo |
| Máy | VendingMachine | inventory_, states_, state_, selectedSlot_ | dispatch, addSlot, describeStates |
| Giao diện console | ConsoleUI | machine_&, input_&, output_& | run, menu, hiển thị, nhập dữ liệu |

Các lớp con `IdleState`, `SelectingState`, `PayingState`, `DispensingState` là những trạng thái chuyên biệt của `MachineState`. Mỗi lớp override cả `id`, `name`, `handle`. Cây gồm base và 4 lớp con, không tính các lớp dữ liệu để làm đủ số lượng kế thừa.

## Ownership và vòng đời

- `VendingMachine` sở hữu `Inventory` theo giá trị, sở hữu 4 state qua `vector<unique_ptr<MachineState>>`.
- Các state được tạo một lần bằng `make_unique`; không thay/xóa state bên trong `handle`.
- `state_` chỉ lưu enum, không giữ con trỏ treo. `currentState()` tìm state rồi trả `const MachineState&`.
- `Inventory` sở hữu `map<string, Slot>`; mỗi `Slot` chứa một bản giá trị `Product`; `Product` chứa `Money`. Chưa có catalog sản phẩm dùng chung.
- `Inventory::find()` trả `const Slot*` quan sát, không chuyển quyền sở hữu; `nullptr` biểu thị không tìm thấy. Không lưu con trỏ này qua vòng đời Inventory. Bản hiện tại không xóa khe.
- `ConsoleUI` mượn máy và hai stream; trong `main`, máy được tạo trước UI và bị hủy sau UI.
- `StateContext` mượn tham chiếu const chỉ trong một lần gọi đồng bộ. State không giữ context lại trong thuộc tính.
- Stream và container tự giải phóng theo RAII. Không có `new/delete` thủ công. Destructor của base là `virtual ~MachineState() = default`.

## Luồng dispatch

`ConsoleUI` tạo `MachineEvent` → `VendingMachine::dispatch` lấy `const MachineState&` hiện hành → gọi virtual `handle(context, event)` → lớp con trả `StateDecision` → máy chỉ áp dụng quyết định được chấp nhận.

Handler chỉ nhìn dữ liệu qua tham chiếu const. Chuỗi và optional được chuẩn bị trong quyết định trước khi máy thay dữ liệu; cập nhật lựa chọn bằng `optional<string>::swap`, rồi gán enum trạng thái. Sự kiện bị từ chối không làm đổi lựa chọn/trạng thái. Đây là bảo vệ của luồng chọn hàng, chưa phải commit giao dịch bán hàng tuần 2.

`DispatchResult` mang accepted/from/to/message để console in kết quả mà không cần biết loại lớp con. Hàm `stateName` có switch chỉ để chuyển enum thành nhãn; nghiệp vụ không dispatch bằng switch theo trạng thái.

## Bảng chuyển trạng thái đã cài

| Trạng thái | Sự kiện | Kết quả |
|---|---|---|
| Idle | StartSelection | Selecting, chưa chọn khe |
| Idle | ChooseSlot hoặc Cancel | Từ chối; vẫn Idle |
| Selecting | ChooseSlot hợp lệ và còn hàng | Paying; nhớ mã khe; không giảm kho |
| Selecting | ChooseSlot sai mã/hết hàng | Từ chối; vẫn Selecting |
| Selecting | Cancel | Idle; xóa lựa chọn |
| Selecting | StartSelection | Từ chối; vẫn Selecting |
| Paying | Cancel | Idle; xóa lựa chọn; giữ kho |
| Paying | ChooseSlot hoặc StartSelection | Từ chối; giữ khe đã chọn |
| Bốn state | Describe | Mô tả riêng cho từng state; không sửa dữ liệu |
| Dispensing | Các lệnh chọn/hủy | Từ chối; mới là khung, chưa có đường vào từ phiên mua |

`describeStates()` duyệt đủ bốn đối tượng đa hình và gửi Describe, nhưng không kích hoạt giao dịch hay thay trạng thái thực tế. Sơ đồ trạng thái hiện tại không vẽ chuyển Paying → Dispensing vì bước đó chưa có trong code.

## Invariant

- Tiền không âm, phép cộng kiểm tra tràn trước khi cộng, phép trừ kiểm tra không âm trước khi trừ.
- Product không có tên/mã rỗng; giá dương là bội số 1.000.
- Slot luôn thỏa 0 <= quantity <= capacity; refill kiểm tra bằng `amount > capacity - quantity` để tránh tràn khi cộng.
- Mã khe không trùng. `try_emplace` không ghi đè khe cũ khi trùng mã.
- Idle/Selecting chưa giữ khe đã chọn; Paying có một khe hợp lệ. Chỉ VendingMachine được thay state_/selectedSlot_. Inventory bên ngoài chỉ có giao diện const.

## SOLID tại mốc đầu

SRP: lớp dữ liệu không đọc menu; ConsoleUI không sửa trực tiếp tồn kho. OCP/DIP: máy gọi qua MachineState, không phụ thuộc nhánh xử lý của từng state trong dispatch. LSP: mọi state dùng cùng hợp đồng nhận sự kiện, trả quyết định và giữ invariant; từ chối sự kiện sai trạng thái là hành vi hợp lệ của hợp đồng. ISP: MachineState chỉ công bố id/name/handle.

Phần SOLID/Strategy của đồ án cuối sẽ được bổ sung ở tuần 2 và rà soát tuần 4. Không tuyên bố mọi yêu cầu cuối kỳ đã hoàn tất tại mốc này.

## Sơ đồ

Mở `uml_week1.svg` để xem; chỉnh nguồn `uml_week1.puml` khi đổi code. Sơ đồ thể hiện các lớp chính và quan hệ sở hữu. Các DTO `MachineEvent`, `StateContext`, `StateDecision`, `DispatchResult`, `StateInfo` được định nghĩa đầy đủ trong `MachineTypes.hpp` và giải thích trong tài liệu này; chúng không nằm trong cây kế thừa.
