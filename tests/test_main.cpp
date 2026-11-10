#include "vending/DemoData.hpp"
#include "vending/MachineState.hpp"
#include "vending/VendingMachine.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

using vending::EventType;
using vending::Inventory;
using vending::Money;
using vending::Product;
using vending::Slot;
using vending::StateId;
using vending::VendingMachine;

void check(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

template<typename Exception, typename Function>
void expectThrow(Function action) {
    bool caught = false;
    try { action(); }
    catch (const Exception&) { caught = true; }
    check(caught, "Expected exception was not thrown");
}

Product water() { return Product{"WATER", "Nuoc suoi", Money{10000}}; }
VendingMachine machine() { return VendingMachine{vending::makeDemoInventory()}; }

void chooseA1(VendingMachine& target) {
    check(target.dispatch({EventType::StartSelection}).accepted, "start failed");
    check(target.dispatch({EventType::ChooseSlot, "A1"}).accepted, "select A1 failed");
}

} // namespace

int main() {
    static_assert(std::has_virtual_destructor_v<vending::MachineState>);
    static_assert(!std::is_copy_constructible_v<std::unique_ptr<vending::MachineState>>);
    const std::vector<std::pair<std::string, std::function<void()>>> tests{
        {"money rejects negative values", [] {
            expectThrow<std::invalid_argument>([] { const Money bad{-1}; });
            check(Money{}.dong() == 0, "default must be zero");
        }},
        {"money arithmetic comparison and formatting", [] {
            check(Money{10000} + Money{5000} == Money{15000}, "sum incorrect");
            check(Money{20000} - Money{10000} == Money{10000}, "difference incorrect");
            check(Money{5000} < Money{10000}, "ordering incorrect");
            std::ostringstream out;
            out << Money{15000};
            check(out.str() == "15000 VND", "format incorrect");
        }},
        {"money overflow cannot change operands", [] {
            const Money max{std::numeric_limits<std::int64_t>::max()};
            expectThrow<std::overflow_error>([&] { const auto result = max + Money{1}; (void)result; });
            check(max.dong() == std::numeric_limits<std::int64_t>::max(), "operand changed");
        }},
        {"money subtraction cannot become negative", [] {
            expectThrow<std::invalid_argument>([] { const auto result = Money{1} - Money{2}; (void)result; });
        }},
        {"product validates identity name and price", [] {
            expectThrow<std::invalid_argument>([] { const Product p{"", "Water", Money{1000}}; });
            expectThrow<std::invalid_argument>([] { const Product p{"P", "   ", Money{1000}}; });
            expectThrow<std::invalid_argument>([] { const Product p{"P", "Water\n", Money{1000}}; });
            expectThrow<std::invalid_argument>([] { const Product p{"P", "Water", Money{0}}; });
            expectThrow<std::invalid_argument>([] { const Product p{"P", "Water", Money{1500}}; });
        }},
        {"slot enforces initial quantity and capacity", [] {
            expectThrow<std::invalid_argument>([] { const Slot s{"A1", water(), -1, 10}; });
            expectThrow<std::invalid_argument>([] { const Slot s{"A1", water(), 11, 10}; });
            expectThrow<std::invalid_argument>([] { const Slot s{"A1", water(), 0, 0}; });
            expectThrow<std::invalid_argument>([] { const Slot s{"A 1", water(), 0, 10}; });
        }},
        {"refill failures preserve stock including huge values", [] {
            Slot slot{"A1", water(), 2, 10};
            for (const int amount : {0, -1, 9, std::numeric_limits<int>::max()}) {
                expectThrow<std::invalid_argument>([&] { slot.refill(amount); });
                check(slot.quantity() == 2, "failed refill changed quantity");
            }
            slot.refill(8);
            check(slot.quantity() == 10, "exact capacity failed");
        }},
        {"taking final item never makes stock negative", [] {
            Slot slot{"A1", water(), 1, 10};
            slot.takeOne();
            check(!slot.inStock(), "last item not removed");
            expectThrow<std::logic_error>([&] { slot.takeOne(); });
            check(slot.quantity() == 0, "quantity became negative");
        }},
        {"inventory duplicate does not overwrite original", [] {
            Inventory inventory;
            inventory.addSlot(Slot{"A1", water(), 2, 10});
            expectThrow<std::invalid_argument>([&] { inventory.addSlot(Slot{"A1", water(), 9, 10}); });
            check(inventory.size() == 1 && inventory.find("A1")->quantity() == 2, "duplicate overwrote original");
            check(inventory.find("NO_SUCH_SLOT") == nullptr, "missing lookup must return nullptr");
        }},
        {"new machine is idle without selection", [] {
            const auto target = machine();
            check(target.state() == StateId::Idle && !target.selectedSlot(), "invalid initial state");
            check(target.inventory().size() == 3, "demo inventory incorrect");
        }},
        {"selection before starting is rejected", [] {
            auto target = machine();
            check(!target.dispatch({EventType::ChooseSlot, "A1"}).accepted, "idle accepted selection");
            check(target.state() == StateId::Idle && !target.selectedSlot(), "rejection mutated state");
        }},
        {"valid path selects without decrementing stock", [] {
            auto target = machine();
            chooseA1(target);
            check(target.state() == StateId::Paying && target.selectedSlot() == "A1", "wrong target state");
            check(target.inventory().find("A1")->quantity() == 2, "selection changed stock");
        }},
        {"missing and empty slots keep selecting state", [] {
            auto target = machine();
            (void)target.dispatch({EventType::StartSelection});
            for (const auto* code : {"MISSING", "A2"}) {
                check(!target.dispatch({EventType::ChooseSlot, code}).accepted, "bad slot accepted");
                check(target.state() == StateId::Selecting && !target.selectedSlot(), "bad choice mutated state");
            }
        }},
        {"cancel works in selecting and paying and is idempotent", [] {
            auto target = machine();
            (void)target.dispatch({EventType::StartSelection});
            check(target.dispatch({EventType::Cancel}).accepted, "cancel selecting failed");
            chooseA1(target);
            check(target.dispatch({EventType::Cancel}).accepted, "cancel paying failed");
            check(target.state() == StateId::Idle && !target.selectedSlot(), "cancel not reset");
            check(!target.dispatch({EventType::Cancel}).accepted, "second cancel should reject");
            check(target.inventory().find("A1")->quantity() == 2, "cancel changed stock");
        }},
        {"paying cannot switch product or restart selection", [] {
            auto target = machine();
            chooseA1(target);
            check(!target.dispatch({EventType::ChooseSlot, "B1"}).accepted, "paying switched item");
            check(!target.dispatch({EventType::StartSelection}).accepted, "paying restarted");
            check(target.selectedSlot() == "A1", "selected code changed");
        }},
        {"adding slots requires idle and valid data", [] {
            auto target = machine();
            chooseA1(target);
            expectThrow<std::logic_error>([&] { target.addSlot(Slot{"C1", water(), 1, 5}); });
            check(target.inventory().size() == 3, "failed add changed inventory");
            (void)target.dispatch({EventType::Cancel});
            target.addSlot(Slot{"C1", water(), 1, 5});
            check(target.inventory().size() == 4, "idle add failed");
        }},
        {"same event reaches four derived state implementations", [] {
            auto target = machine();
            chooseA1(target);
            const auto rows = target.describeStates();
            check(rows.size() == 4, "polymorphic container size incorrect");
            const std::array expected{StateId::Idle, StateId::Selecting, StateId::Paying, StateId::Dispensing};
            for (std::size_t i = 0; i < rows.size(); ++i) {
                check(rows[i].id == expected[i] && !rows[i].description.empty(), "missing state override");
                for (std::size_t j = i + 1; j < rows.size(); ++j)
                    check(rows[i].description != rows[j].description, "state descriptions must differ");
            }
            check(target.state() == StateId::Paying && target.selectedSlot() == "A1", "describe changed state");
        }},
        {"dispensing shell rejects unsafe commands through base", [] {
            const auto inventory = vending::makeDemoInventory();
            const std::optional<std::string> selection{"A1"};
            const vending::StateContext context{inventory, selection};
            const std::unique_ptr<vending::MachineState> state = std::make_unique<vending::DispensingState>();
            for (const auto event : {EventType::StartSelection, EventType::ChooseSlot, EventType::Cancel}) {
                const auto result = state->handle(context, {event, "B1"});
                check(!result.accepted && result.nextState == StateId::Dispensing, "dispensing accepted unsafe event");
                check(result.nextSelectedSlot == selection, "dispensing altered selection");
            }
        }}
    };

    std::size_t failures = 0;
    for (const auto& [name, test] : tests) {
        try {
            test();
            std::cout << "[PASS] " << name << '\n';
        } catch (const std::exception& error) {
            ++failures;
            std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
        }
    }
    std::cout << tests.size() - failures << '/' << tests.size() << " tests passed\n";
    return failures == 0 ? 0 : 1;
}
