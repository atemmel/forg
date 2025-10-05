#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "buffer.hpp"
#include "slice.hpp"

namespace forg {

template <typename T>
class Array {
public:
    using reference       = T&;
    using const_reference = const T&;
    using iterator        = T*;
    using const_iterator  = const T*;

    // constructors
    constexpr Array() : buffer(0) {
    }
    constexpr Array(usize size) : buffer(size), currentSize(size) {
    }
    constexpr Array(const T* source, usize size)
        : buffer(source, size), currentSize(size) {
    }
    constexpr Array(usize size, const T& val) : Array(size) {
        for (usize i = 0; i < size; i++) {
            this->buffer[i] = val;
        }
    }
    constexpr Array(const Array& rhs)  // copy constructor
        : Array(rhs.data(), rhs.size()) {
    }
    constexpr Array(Array&& rhs) {     // move constructor
        this->swap(rhs);
    }
    constexpr Array(std::initializer_list<T> init)
        : buffer(init), currentSize(init.size()) {
    }

    // Assign
    constexpr Array& operator=(const Array& rhs) {  // copy assign
        auto temp(rhs);
        this->swap(temp);
        return *this;
    }
    constexpr Array& operator=(Array&& rhs) {  // move assign
        this->swap(rhs);
        return *this;
    }

    // Access
    constexpr reference operator[](usize index) {
        return buffer[index];
    }
    constexpr const_reference operator[](usize index) const {
        return buffer[index];
    }

    constexpr reference at(usize index) {
        return buffer[index];
    }
    constexpr const_reference at(usize index) const {
        return buffer[index];
    }

    constexpr iterator data() {
        return buffer.data();
    }
    constexpr const_iterator data() const {
        return buffer.data();
    }

    // Capacity
    constexpr usize size() const {
        return currentSize;
    }
    constexpr bool empty() {
        return currentSize == 0;
    }
    constexpr usize capacity() {
        return buffer.size();
    }
    constexpr void reserve(usize newSize) {
        if (newSize > this->capacity()) {
            realloc(newSize);
        }
    }
    constexpr void shrinkToFit() {
        realloc(currentSize);
    }

    // Modifiers
    constexpr void clear() {
        for (auto& e : buffer) {
            e = T();
        }
        currentSize = 0;
    }

    constexpr void append(const T& value) {
        if (this->size() == this->capacity()) {
            expand();
        }
        buffer[currentSize++] = value;
    }

    constexpr void append(const Slice<T>& slice) {
        while (this->size() + slice.size() >= this->capacity()) {
            expand();
        }
        for (auto p : slice) {
            buffer[currentSize++] = p;
        }
    }

    constexpr void append(const View<T>& view) {
        while (this->size() + view.size() >= this->capacity()) {
            expand();
        }
        for (auto p : view) {
            buffer[currentSize++] = p;
        }
    }

    constexpr void append(T&& value) {
        if (this->size() == this->capacity()) {
            expand();
        }
        buffer[currentSize++] = forg::isMovable(value);
    }

    constexpr void popBack() {
        --currentSize;
    }

    constexpr void resize(usize newSize) {
        Array newArr(newSize);

        auto count = newSize < this->size() ? newSize : this->size();

        for (usize i = 0; i < count; i++) {
            newArr[i] = forg::isMovable(buffer[i]);
        }

        this->swap(newArr);
    }

    // Iterators
    constexpr const_iterator begin() const {
        return buffer.begin();
    }
    constexpr const_iterator end() const {
        return buffer.begin() + currentSize;
    }

    constexpr iterator begin() {
        return buffer.begin();
    }
    constexpr iterator end() {
        return buffer.begin() + currentSize;
    }

    constexpr Slice<T> slice() {
        return Slice<T>(this->begin(), this->end());
    }
    constexpr Slice<T> slice(usize first, usize last) {
        return Slice<T>(this->begin() + first, this->begin() + last);
    }

    ~Array() {
        currentSize = 0;
    }

    constexpr void swap(Array& other) {
        this->buffer.swap(other.buffer);
        forg::swap(this->currentSize, other.currentSize);
    }

private:
    constexpr void expand() {
        usize newSize = static_cast<usize>(this->capacity() * 2);
        if (this->capacity() == 0) {
            newSize = 1;
        } else if (this->capacity() == 1) {
            newSize++;
        }

        realloc(newSize);
    }
    constexpr void realloc(usize newSize) {
        Buffer<T> newBuffer(newSize);
        for (usize i = 0; i < newSize; i++) {
            newBuffer[i] = buffer[i];
        }
        buffer.swap(newBuffer);
    }

    Buffer<T> buffer;
    usize currentSize = 0;
};

}  // namespace forg

#endif  // !ARRAY_HPP
