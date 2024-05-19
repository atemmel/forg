#include <forg/core.hpp>

#include "my_state.hpp"

using namespace forg;

auto main() -> int {
    println("Starting...");
    return state::run<MyState>("My game");
}
