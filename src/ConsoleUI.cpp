#include "vending/ConsoleUI.hpp"
#include <charconv>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace vending {
namespace {

std::string_view trim(std::string_view text) {
    const auto begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string_view::npos) return {};
    const auto end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

template<typename Integer>
Integer parseInteger(const std::string& raw) {
    const auto text = trim(raw);
    if (text.empty()) throw std::invalid_argument("Can nhap mot so nguyen.");
    Integer value{};
    const auto parsed = std::from_chars(text.data(), text.data() + text.size(), value);
    if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size()) {
        throw std::invalid_argument("So nguyen khong hop le hoac vuot gioi han.");
    }
    return value;
}

bool readLine(std::istream& in, std::ostream& out, const char* prompt, std::string& value) {
    out << prompt;
    return static_cast<bool>(std::getline(in, value));
}

} // namespace

ConsoleUI::ConsoleUI(VendingMachine& machine, std::istream& input, std::ostream& output)
    : machine_{machine}, input_{input}, output_{output} {}

void ConsoleUI::printMenu() const {
    output_ << "\nTrang thai: " << stateName(machine_.state());
    if (machine_.selectedSlot()) output_ << " | Khe da chon: " << *machine_.selectedSlot();
    output_ << "\n1. Xem danh sach hang\n2. Bat dau chon hang\n3. Chon ma khe hang\n"
               "4. Huy lua chon\n5. Them khe hang (Idle)\n6. Xem cac trang thai va da hinh\n"
               "0. Thoat\nLua chon: ";
}

void ConsoleUI::showInventory() const {
    output_ << "\n" << std::left << std::setw(10) << "Khe" << std::setw(32) << "San pham"
            << std::right << std::setw(14) << "Gia (VND)" << std::setw(10) << "Ton"
            << std::setw(12) << "Suc chua" << '\n';
    for (const auto& [code, slot] : machine_.inventory().slots()) {
        output_ << std::left << std::setw(10) << code << std::setw(32) << slot.product().name()
                << std::right << std::setw(14) << slot.product().price().dong()
                << std::setw(10) << slot.quantity() << std::setw(12) << slot.capacity() << '\n';
    }
}

void ConsoleUI::showStates() const {
    output_ << "\nCung su kien Describe duoc gui qua MachineState cho 4 lop con:\n";
    for (const auto& info : machine_.describeStates()) {
        output_ << "- " << info.name << ": " << info.description << '\n';
    }
    output_ << "Xem thong tin khong lam doi trang thai hay ton kho.\n";
}

void ConsoleUI::showResult(const DispatchResult& result) const {
    output_ << (result.accepted ? "[OK] " : "[TU CHOI] ") << result.message
            << "\n" << stateName(result.from) << " -> " << stateName(result.to) << '\n';
}

bool ConsoleUI::addSlot() {
    if (machine_.state() != StateId::Idle) {
        output_ << "[TU CHOI] Chi them khe hang khi may o Idle.\n";
        return true;
    }
    std::string code, productId, name, price, quantity, capacity;
    if (!readLine(input_, output_, "Ma khe: ", code) ||
        !readLine(input_, output_, "Ma san pham: ", productId) ||
        !readLine(input_, output_, "Ten san pham: ", name) ||
        !readLine(input_, output_, "Gia (boi so 1000 VND): ", price) ||
        !readLine(input_, output_, "So luong ban dau: ", quantity) ||
        !readLine(input_, output_, "Suc chua: ", capacity)) return false;

    Product product{std::string{trim(productId)}, name, Money{parseInteger<std::int64_t>(price)}};
    machine_.addSlot(Slot{std::string{trim(code)}, std::move(product),
                          parseInteger<int>(quantity), parseInteger<int>(capacity)});
    output_ << "[OK] Da them khe hang.\n";
    return true;
}

int ConsoleUI::run() {
    output_ << "MAY BAN HANG TU DONG - MOC 1\n"
               "Ban nay cho phep tao va chon hang; chua nhan tien hoac xuat hang.\n";
    for (;;) {
        printMenu();
        std::string line;
        if (!std::getline(input_, line)) {
            output_ << "\nKet thuc dau vao. Tam biet.\n";
            return 0;
        }
        try {
            const auto choice = parseInteger<int>(line);
            switch (choice) {
            case 0:
                output_ << "Tam biet. Du lieu them trong phien nay chua duoc luu tep.\n";
                return 0;
            case 1: showInventory(); break;
            case 2: showResult(machine_.dispatch({EventType::StartSelection})); break;
            case 3: {
                std::string code;
                if (!readLine(input_, output_, "Nhap ma khe: ", code)) return 0;
                showResult(machine_.dispatch({EventType::ChooseSlot, std::string{trim(code)}}));
                break;
            }
            case 4: showResult(machine_.dispatch({EventType::Cancel})); break;
            case 5: if (!addSlot()) return 0; break;
            case 6: showStates(); break;
            default: output_ << "[LOI] Lua chon phai tu 0 den 6.\n"; break;
            }
        } catch (const std::exception& error) {
            output_ << "[LOI] " << error.what() << '\n';
        }
    }
}

} // namespace vending
