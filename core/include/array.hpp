#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "buffer.hpp"
#include "range.hpp"
namespace cor {

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
    constexpr Array(size_ty size) : buffer(size), currentSize(size) {
    }
    constexpr Array(const T* source, size_ty size)
        : buffer(source, size), currentSize(size) {
    }
    constexpr Array(size_t size, const T& val) : Array(size) {
        for (size_t i = 0; i < size; i++) {
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
    constexpr reference operator[](size_ty index) {
        return buffer[index];
    }
    constexpr const_reference operator[](size_ty index) const {
        return buffer[index];
    }

    constexpr reference at(size_ty index) {
        return buffer[index];
    }
    constexpr const_reference at(size_ty index) const {
        return buffer[index];
    }

    constexpr iterator data() {
        return buffer.data();
    }
    constexpr const_iterator data() const {
        return buffer.data();
    }

    // Capacity
    constexpr size_ty size() const {
        return currentSize;
    }
    constexpr bool empty() {
        return currentSize == 0 ? true : false;
    }
    constexpr size_ty capacity() {
        return buffer.size();
    }
    constexpr void reserve(size_ty newSize) {
        if (newSize > this->capacity()) {
            realoc(newSize);
        }
    }
    constexpr void shrinkToFit() {
        realoc(currentSize);
    }

    // Modifiers
    constexpr void clear() {
        for (auto& e : buffer) {
            e = T();
        }
        currentSize = 0;
    }
    constexpr void pushBack(const T& value) {
        if (this->size() == this->capacity()) {
            expand();
        }
        buffer[currentSize++] = value;
    }
    constexpr void pushBack(T&& value) {
        if (this->size() == this->capacity()) {
            expand();
        }
        buffer[currentSize++] = cor::isMovable(value);
    }
    constexpr void popBack() {
        --currentSize;
    }
    constexpr void resize(size_ty newSize) {
        Array newArr(newSize);

        auto count = newSize < this->size() ? newSize : this->size();

        for (size_t i = 0; i < count; i++) {
            newArr[i] = cor::isMovable(buffer[i]);
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
    constexpr Slice<T> slice(size_t first, size_t last) {
        return Slice<T>(this->begin() + first, this->begin() + last);
    }

    ~Array() {
        currentSize = 0;
    }

    constexpr void swap(Array& other) {
        this->buffer.swap(other.buffer);
        cor::swap(this->currentSize, other.currentSize);
    }

private:
    constexpr void expand() {
        size_ty newSize = static_cast<size_ty>(this->capacity() * 1.5);
        if (this->capacity() == 0) {
            newSize = 1;
        } else if (this->capacity() == 1) {
            newSize++;
        }

        realoc(newSize);
    }
    constexpr void realoc(size_ty newSize) {
        Buffer<T> newBuffer(newSize);
        for (size_t i = 0; i < newSize; i++) {
            newBuffer[i] = buffer[i];
        }
        buffer.swap(newBuffer);
    }

    Buffer<T> buffer;
    size_ty currentSize = 0;
};

template <class T>
void PrintArr(const Array<T>& arr) {
    for (auto& e : arr) {
        std::cout << e << ", ";
    }
    std::cout << "\n";
}

template <class T>
void PrintArr(const Array<Array<T>>& arr) {
    for (size_t row = 0; row < arr.size(); row++) {
        for (size_t col = 0; col < arr[row].size(); col++) {
            std::cout << arr[row][col] << ", ";
        }
        std::cout << "\n";
    }
}

}  // namespace cor

#endif  // !ARRAY_HPP
