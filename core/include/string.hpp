#ifndef STRING_HPP
#define STRING_HPP

#include <initializer_list>

#include "algorithms.hpp"
#include "memory.hpp"
#include "range.hpp"
#include "utility.hpp"

namespace cor {

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

    // ITER
    pointer begin() noexcept;
    const_pointer begin() const noexcept;
    pointer end() noexcept;
    const_pointer end() const noexcept;

    // CAP
    constexpr usize size() const noexcept;
    constexpr bool empty() const noexcept {
        return size() == 0;
    }

    // MODIFIERS
    constexpr void swap(String& other) noexcept;

    ~String();

private:
    mem::Allocator<char> allocator;
    usize ssize = 0;
    pointer ptr = nullptr;
};

bool operator==(const cor::String& lhs, const cor::String& rhs) noexcept;
bool operator!=(const cor::String& lhs, const cor::String& rhs) noexcept;
bool operator==(const cor::String& lhs, const char* rhs) noexcept;
bool operator!=(const cor::String& lhs, const char* rhs) noexcept;
}  // namespace cor

#endif  // !STRING_HPP
