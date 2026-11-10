"""Render the verified week-1 UML snapshot to SVG using only the standard library.

The editable semantic diagram is docs/uml_week1.puml. Update both after design changes.
"""
from html import escape
from pathlib import Path

out = Path(__file__).resolve().parents[1] / 'docs' / 'uml_week1.svg'
parts = ['''<svg xmlns="http://www.w3.org/2000/svg" width="1660" height="1330" viewBox="0 0 1660 1330">
<title>UML các lớp chính của Máy bán hàng tự động tuần 1</title>
<desc>VendingMachine sở hữu Inventory và bốn MachineState qua unique_ptr. IdleState, SelectingState, PayingState và DispensingState kế thừa MachineState. Inventory chứa Slot, Slot chứa Product và Product chứa Money. ConsoleUI mượn tham chiếu tới máy.</desc>
<defs>
 <marker id="arrow" markerWidth="12" markerHeight="12" refX="10" refY="6" orient="auto"><path d="M1,1 L10,6 L1,11" fill="none" stroke="#26364A" stroke-width="1.4"/></marker>
 <marker id="inherit" markerWidth="15" markerHeight="15" refX="13" refY="7.5" orient="auto"><path d="M1,1 L13,7.5 L1,14 Z" fill="white" stroke="#26364A" stroke-width="1.2"/></marker>
 <marker id="diamond" markerWidth="16" markerHeight="12" refX="1" refY="6" orient="auto"><path d="M1,6 L7,1 L13,6 L7,11 Z" fill="#26364A"/></marker>
</defs>
<rect width="1660" height="1330" fill="white"/>
<g font-family="DejaVu Sans, sans-serif" fill="#172635">
<text x="50" y="47" font-size="27" font-weight="bold">Máy bán hàng tự động — Sơ đồ lớp tuần 1</text>
<text x="50" y="76" font-size="16">Các lớp chính khớp mã C++20 • Dữ liệu đóng gói • State và container đa hình</text>
''']


def line(path, start=None, end=None):
    a = f' marker-start="url(#{start})"' if start else ''
    b = f' marker-end="url(#{end})"' if end else ''
    parts.append(f'<path d="{path}" stroke="#26364A" stroke-width="2" fill="none"{a}{b}/>')


def text(x, y, label, size=15, bold=False):
    parts.append(f'<text x="{x}" y="{y}" font-size="{size}"' +
                 (' font-weight="bold"' if bold else '') + f'>{escape(label)}</text>')


def box(x, y, w, title, fields, methods, abstract=False):
    head = 44 if not abstract else 61
    attrs = 22 * len(fields) + (14 if fields else 0)
    methods_h = 23 * len(methods) + 19
    height = head + attrs + methods_h
    parts.append(f'<rect x="{x}" y="{y}" width="{w}" height="{height}" fill="white" stroke="#26364A" stroke-width="1.6"/>')
    parts.append(f'<rect x="{x+1}" y="{y+1}" width="{w-2}" height="{head-1}" fill="#EAF0F7"/>')
    if abstract:
        text(x + 15, y + 21, '«abstract»', 14)
        text(x + 15, y + 46, title, 22, True)
    else:
        text(x + 15, y + 29, title, 21, True)
    line(f'M{x},{y+head} H{x+w}')
    for i, label in enumerate(fields): text(x + 13, y + head + 23 + i*22, label, 15)
    if fields: line(f'M{x},{y+head+attrs} H{x+w}')
    for i, label in enumerate(methods): text(x+13, y+head+attrs+25+i*23, label, 14)
    return height


# Relationships are drawn behind boxes; filled diamond is at the owner end.
line('M400,230 H500', end='arrow'); text(409,215,'mượn &',13)
line('M1160,220 H1240', start='diamond'); text(1184,201,'1',15)
line('M1425,336 V445', start='diamond'); text(1440,416,'0..*',15)
line('M1425,702 V760', start='diamond'); text(1440,734,'1',15)
line('M1425,972 V1035', start='diamond'); text(1440,1018,'1',15)
line('M690,357 V510', start='diamond'); text(710,451,'4 state được sở hữu',16)
line('M175,900 V820 H1030 V900')
line('M460,900 V820'); line('M745,900 V820')
line('M690,820 V682', end='inherit'); text(711,789,'IS-A',16)

box(50,140,350,'ConsoleUI',[
    '- machine_: VendingMachine&', '- input_: istream&', '- output_: ostream&'],['+ run(): int'])
box(500,100,660,'VendingMachine',[
    '- inventory_: Inventory', '- states_: vector<unique_ptr<MachineState>>',
    '- state_: StateId', '- selectedSlot_: optional<string>'],[
    '+ dispatch(event): DispatchResult', '+ describeStates(): vector<StateInfo>',
    '+ addSlot(slot): void', '+ inventory(): const Inventory&'])
box(1240,145,370,'Inventory',['- slots_: map<string, Slot>'],[
    '+ addSlot(slot): void', '+ find(code): const Slot*',
    '+ slots(): const map<string, Slot>&', '+ size(): size_t'])
box(1240,445,370,'Slot',[
    '- code_: string', '- product_: Product', '- quantity_: int', '- capacity_: int'],[
    '+ inStock(): bool', '+ product(): const Product&', '+ refill(amount): void', '+ takeOne(): void'])
box(1240,760,370,'Product',['- id_: string', '- name_: string', '- price_: Money'],[
    '+ id(): const string&', '+ name(): const string&', '+ price(): Money'])
box(1240,1035,370,'Money',['- dong_: int64_t'],[
    '+ dong(): int64_t', '+ operator+ / operator-', '+ operator== / operator<', '+ operator<< (friend)'])
box(420,510,540,'MachineState',[],[
    '+ virtual ~MachineState() = default', '+ id(): StateId = 0',
    '+ name(): string_view = 0', '+ handle(context, event): StateDecision = 0'],abstract=True)
for x, name in [(45,'IdleState'),(330,'SelectingState'),(615,'PayingState'),(900,'DispensingState')]:
    box(x,900,260,name,[],['+ id() override', '+ name() override', '+ handle(...) override'])

text(50,1120,'Quan hệ sở hữu',19,True)
text(50,1150,'Hình thoi đen: composition. Tam giác rỗng: kế thừa. Mũi tên: tham chiếu không sở hữu.',16)
text(50,1180,'Mỗi Slot chứa 1 Product; mỗi Product chứa 1 Money theo giá trị.',16)
text(50,1218,'Phạm vi mốc đầu',19,True)
text(50,1248,'DispensingState mới có khung handle. Luồng mua chưa chuyển vào Dispensing và chưa nhận tiền.',16)
text(50,1278,'DTO sự kiện/quyết định và hợp đồng tham chiếu: xem MachineTypes.hpp và docs/02_design.md.',16)
parts.append('</g></svg>')
out.write_text('\n'.join(parts), encoding='utf-8')
print(out)
