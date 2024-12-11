#include <catch2/catch_all.hpp>

#include "core.hpp"

struct DummyState : public forg::State {
    auto update() -> bool {
        return false;
    }
    auto draw() -> void {};
};

TEST_CASE("state test") {
    forg::state::run<DummyState>("Test window", 400, 400);
}
