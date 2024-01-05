#ifndef RANGE_HPP
#define RANGE_HPP

namespace cor {

template <class T>
class Slice {
public:
    Slice(T* first = nullptr, T* last = nullptr)
        : first(first), ssize((last - first)) {
    }

    size_t size() const {
        return ssize;
    }
    bool empty() const {
        return first == nullptr;
    }

    T& operator[](size_t index) const {
        return first[index];
    }
    T& at(size_t index) const {
        return first[index];
    }

    const T& front() const {
        return *first;
    }
    const T& back() const {
        return *(first + ssize - 1);
    }

    T& front() {
        return *first;
    }
    T& back() {
        return *(first + ssize - 1);
    }

    T* data() {
        return first;
    }

    const T* begin() const {
        return first;
    }
    const T* end() const {
        return first + ssize;
    }

    T* begin() {
        return first;
    }
    T* end() {
        return first + ssize;
    }

private:
    T* first = nullptr;
    size_t ssize;
};

template <class T>
class View {
public:
    constexpr View(const T* s, size_t count) : first(s), last(s + count) {
    }

    constexpr View(const T* s) : first(s), last(s + cor::strlen(s)) {
    }

    View(T* first = nullptr, T* last = nullptr) : first(first), last(last) {
    }

    size_t size() const {
        return last - first;
    }
    bool empty() const {
        return first == nullptr;
    }

    const T& operator[](size_t index) const {
        return first[index];
    }
    const T& at(size_t index) const {
        return first[index];
    }

    const T& front() const {
        return *first;
    }
    const T& back() const {
        return *last;
    }

    const T* data() const {
        return first;
    }

    const T* begin() const {
        return first;
    }
    const T* end() const {
        return last;
    }

private:
    const T* first = nullptr;
    const T* last  = nullptr;
};

}  // namespace cor
#endif  // !RANGE_HPP
