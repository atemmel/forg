#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <initializer_list>

#include "memory.hpp"

namespace cor {

typedef unsigned long long size_ty;

template <typename T>
class Buffer {
public:
    // constructors
    constexpr Buffer() : end_ptr(nullptr), begin_ptr(nullptr) {
    }

    constexpr Buffer(size_ty size)
        : begin_ptr(mem::template allocateArr<T>(size)),
          end_ptr(begin_ptr + size) {
    }

    constexpr Buffer(const T* source, size_ty size) : Buffer(size) {
        for (size_ty i = 0; i < size; i++) {
            this->begin_ptr[i] = cor::isMovable(source[i]);
        }
    }
    constexpr Buffer(const Buffer& other)  // copy constructor
        : Buffer(other.begin(), other.size()) {
    }

    constexpr Buffer(Buffer&& rhs) {  // move constructor
        this->swap(rhs);
    }

    constexpr Buffer(std::initializer_list<T> init)
        : Buffer(init.begin(), init.size()) {
    }

    // Assign
    constexpr Buffer& operator=(const Buffer& rhs) {  // copy assign
        auto temp(rhs);
        this->swap(temp);
        return *this;
    }

    constexpr Buffer& operator=(Buffer&& rhs) {  // move assign
        this->swap(rhs);
        return *this;
    }

    // Access
    constexpr T& operator[](size_ty index) {
        return this->begin_ptr[index];
    }
    constexpr const T& operator[](size_ty index) const {
        return this->begin_ptr[index];
    }

    constexpr size_ty size() const {
        return this->end_ptr - this->begin_ptr;
    }
    constexpr bool empty() {
        return this->end_ptr == this->begin_ptr;
    }

    constexpr T* begin() {
        return this->begin_ptr;
    }
    constexpr T* end() {
        return this->end_ptr;
    }

    constexpr const T* begin() const {
        return this->begin_ptr;
    }
    constexpr const T* end() const {
        return this->end_ptr;
    }

    constexpr T* data() {
        return this->begin_ptr;
    }
    constexpr const T* data() const {
        return this->begin_ptr;
    }

    constexpr void swap(Buffer& other) {
        cor::swap(this->begin_ptr, other.begin_ptr);
        cor::swap(this->end_ptr, other.end_ptr);
    }

    ~Buffer() {
        this->freemem();
    }

private:
    constexpr void freemem() {
        mem::deallocateArr(begin_ptr);
        begin_ptr = nullptr;
        end_ptr   = nullptr;
    }

    T* begin_ptr = nullptr;
    T* end_ptr   = nullptr;
};

}  // namespace cor

#endif  // !BUFFER_HPP
