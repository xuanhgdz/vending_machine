#include "vending/ConsoleUI.hpp"
#include "vending/DemoData.hpp"
#include <exception>
#include <iostream>

int main() {
    try {
        vending::VendingMachine machine{vending::makeDemoInventory()};
        vending::ConsoleUI ui{machine, std::cin, std::cout};
        return ui.run();
    } catch (const std::exception& error) {
        std::cerr << "Khong khoi tao duoc chuong trinh: " << error.what() << '\n';
        return 1;
    }
}
