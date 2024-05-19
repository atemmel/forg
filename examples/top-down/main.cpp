#include "my_state.hpp"
#include <forg/core.hpp>

using namespace forg;

auto main() -> int {
  println("Starting...");
  return state::run<MyState>("My game");
}
