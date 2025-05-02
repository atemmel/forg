#pragma once

#include <cstdio>
#include <initializer_list>

#include "algorithms.hpp"
#include "array.hpp"
#include "memory.hpp"
#include "slice.hpp"
#include "static_array.hpp"
#include "types.hpp"
#include "utility.hpp"
#include "view.hpp"

namespace forg {

class String {
public:
    using pointer         = char*;
    using const_pointer   = const char*;
    using reference       = char&;
    using const_reference = const char&;

    // CTOR
    String() = default;
    String(usize count, char chr);
    String(const_pointer s);
    String(const_pointer s, usize count);
    String(const String& other);
    String(String&& other) noexcept;
    String(std::initializer_list<char> ilist);
    String(Slice<char> slice);
    String(View<char> view);

    // ASSIGN
    String& operator=(const String& str);
    String& operator=(String&& str) noexcept;
    String& operator=(const_pointer s);
    String& operator=(std::initializer_list<char> ilist);

    // ELEM ACCESS
    reference operator[](usize index);
    const_reference operator[](usize index) const;
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;
    pointer data() noexcept;
    const_pointer data() const noexcept;
    Slice<char> slice(usize first, usize last);
    Slice<char> slice();
    View<char> view(usize first, usize last) const;
    View<char> view() const;

    // ITER
    pointer begin() noexcept;
    const_pointer begin() const noexcept;
    pointer end() noexcept;
    const_pointer end() const noexcept;

    // CAP
    constexpr usize size() const noexcept;
    constexpr bool empty() const noexcept;

    // MODIFIERS
    constexpr void swap(String& other) noexcept;

    ~String();

private:
    template <typename Param, typename... Params>
    static auto fmt(Array<char>& bytes, const Param& param,
                    Params... params) -> String {
        bytes.append(representation(param));
        return fmt(bytes, forward<Params>(params)...);
    }

    template <typename Param>
    static auto fmt(Array<char>& bytes, const Param& param) -> String {
        bytes.append(representation(param));
        return String(bytes.slice());
    }

public:
    template <typename Param, typename... Params>
    static auto fmt(const Param& param, Params... params) -> String {
        Array<char> bytes;
        return fmt(bytes, param, forward<Params>(params)...);
    }

    static auto representation(int value) -> View<char> {
        static thread_local StaticArray<char, 12> buffer;
        auto n = snprintf(buffer.data(), buffer.size(), "%d", value);
        return buffer.view(0, n);
    }

    static auto representation(float value) -> View<char> {
        static thread_local StaticArray<char, 12> buffer;
        auto n = snprintf(buffer.data(), buffer.size(), "%f", value);
        return buffer.view(0, n);
    }

    static auto representation(const char* value) -> const View<char> {
        usize n = strlen(value);
        return View<char>(value, n);
    }

    static auto representation(const String& value) -> const View<char> {
        return value.view();
    }

    static auto representation(bool value) -> const View<char> {
        return value ? View("true", 4) : View("false", 5);
    }

private:
    mem::Allocator<char> allocator;
    usize ssize = 0;
    pointer ptr = nullptr;
};

auto fprintType(File* file, const String& str) -> void;

bool operator==(const forg::String& lhs, const forg::String& rhs) noexcept;
bool operator!=(const forg::String& lhs, const forg::String& rhs) noexcept;
bool operator==(const forg::String& lhs, const char* rhs) noexcept;
bool operator!=(const forg::String& lhs, const char* rhs) noexcept;
}  // namespace forg
