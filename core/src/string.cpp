#include "string.hpp"

#include "algorithms.hpp"
#include "print.hpp"

using namespace forg;

forg::String::String(usize count, char chr) : ssize(count) {
    this->ptr = allocator.createN(this->ssize + 1);
    usize i   = 0;
    for (; i < count; i++) {
        this->ptr[i] = chr;
    }
    this->ptr[i] = '\0';
}

forg::String::String(const_pointer s) : String(s, forg::strlen(s)) {
}

forg::String::String(const_pointer s, usize count) {
    this->ssize = count;
    this->ptr   = allocator.createN(this->ssize + 1);
    mem::copy(s, s + this->ssize, this->ptr);
}

forg::String::String(const String& other) : String(other.data(), other.size()) {
}

forg::String::String(String&& other) noexcept {
    this->swap(other);
}

forg::String::String(std::initializer_list<char> ilist)
    : String(ilist.begin(), ilist.size()) {
}

forg::String& forg::String::operator=(const String& str) {
    if (*this != str) {
        auto temp(str);
        this->swap(temp);
    }
    return *this;
}

forg::String& forg::String::operator=(String&& str) noexcept {
    if (*this != str) {
        this->swap(str);
    }
    return *this;
}

forg::String& forg::String::operator=(const_pointer s) {
    if (*this != s) {
        forg::String temp(s);
        this->swap(temp);
    }
    return *this;
}

forg::String& forg::String::operator=(std::initializer_list<char> ilist) {
    forg::String temp(ilist);
    this->swap(temp);
    return *this;
}

forg::String::reference forg::String::operator[](usize index) {
    return this->data()[index];
}

forg::String::const_reference forg::String::operator[](usize index) const {
    return this->data()[index];
}

forg::String::reference forg::String::front() {
    return this->data()[0];
}

forg::String::const_reference forg::String::front() const {
    return this->data()[0];
}

forg::String::reference forg::String::back() {
    return this->data()[this->size() - 1];
}

forg::String::const_reference forg::String::back() const {
    return this->data()[this->size() - 1];
}

forg::String::pointer forg::String::begin() noexcept {
    return this->data();
}

forg::String::const_pointer forg::String::begin() const noexcept {
    return this->data();
}

forg::String::pointer forg::String::end() noexcept {
    return this->data() + this->size();
}

forg::String::const_pointer forg::String::end() const noexcept {
    return this->data() + this->size();
}

constexpr usize forg::String::size() const noexcept {
    return this->ssize;
}

constexpr bool forg::String::empty() const noexcept {
    return size() == 0;
}

forg::String::const_pointer forg::String::data() const noexcept {
    return this->ptr;
}

forg::String::pointer forg::String::data() noexcept {
    return this->ptr;
}

forg::Slice<char> forg::String::slice(usize first, usize last) {
    return Slice<char>(this->begin() + first, this->begin() + last);
}

forg::Slice<char> forg::String::slice() {
    return Slice<char>(this->begin(), this->end());
}

constexpr void forg::String::swap(String& other) noexcept {
    forg::swap(this->ssize, other.ssize);
    forg::swap(this->ptr, other.ptr);
}

forg::String::~String() {
    allocator.deallocate(this->ptr);
}

auto forg::fprintType(forg::File* file, const String& str) -> void {
    fprintType(file, str.data());
}

bool forg::operator==(const forg::String& lhs,
                      const forg::String& rhs) noexcept {
    return forg::strcmp(lhs.data(), rhs.data()) == 0;
}

bool forg::operator!=(const forg::String& lhs,
                      const forg::String& rhs) noexcept {
    return forg::strcmp(lhs.data(), rhs.data()) != 0;
}

bool forg::operator==(const forg::String& lhs, const char* rhs) noexcept {
    return forg::strcmp(lhs.data(), rhs) == 0;
}

bool forg::operator!=(const forg::String& lhs, const char* rhs) noexcept {
    return forg::strcmp(lhs.data(), rhs) != 0;
}
