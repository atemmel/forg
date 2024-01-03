#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "utility.hpp"
#include "type_traits.hpp"
#include "math.hpp"
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )


// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

constexpr auto DEFAULT_ALIGNMENT = 16ull;

namespace cor::mem {

	enum class alignVal_T : size_t {};

	template <typename T>
	T* allocateArr(size_t size) {
		return size > 0 ? DBG_NEW T[size]{} : DBG_NEW T[size];
	}

	template <typename T>
	void deallocateArr(T* ptr) {
		delete[] ptr;
	}

	template <typename T>
	std::remove_extent_t<T>* allocate_r_extent(size_t size) {
		typedef std::remove_extent_t<T> Elem;
		Elem* alocatedMem = DBG_NEW Elem[size]();
		return alocatedMem;
	}

	template <typename T>
	T* allocate() {
		return DBG_NEW T;
	}

	//template <typename T, typename U>
	//T* allocate(const U& value) {
	//	return new T(value);
	//}

	template <typename T, typename... U>
	T* allocate(U&&... value) {
		return DBG_NEW T(cor::forward<U>(value)...);
	}

	template <typename T>
	void deallocate(T* ptr) {
		delete ptr;
	}

	template <typename T, typename Place, typename... Args>
	T* allocatePlace(Place* place, Args&&... args) {
		return ::new (place) T(cor::forward<Args>(args)...);
	}

	template <typename T, typename Place>
	T* allocatePlaceDefault(Place* place) {
		return ::new (place) T();
	}

	template<size_t Align, EnableIf_T<(Align > DEFAULT_ALIGNMENT), int> = 0>
	void* allocateRaw(size_t count) {
		return operator new(count, std::align_val_t{ Align });
	}

	template<size_t Align, EnableIf_T<(Align <= DEFAULT_ALIGNMENT), int> = 0>
	void* allocateRaw(size_t count) {
		return operator new(count);
	}

	template<size_t Align, EnableIf_T<(Align > DEFAULT_ALIGNMENT), int> = 0>
	void deallocateRaw(void* ptr) {
		return operator delete(ptr, std::align_val_t{ Align });
	}

	template<size_t Align, EnableIf_T<(Align <= DEFAULT_ALIGNMENT), int> = 0>
	void deallocateRaw(void* ptr) {
		return operator delete(ptr);
	}

	template<class T>
	inline constexpr size_t align_of = max_of(alignof(T), static_cast<size_t>(DEFAULT_ALIGNMENT));

	template<class T>
	struct Allocator
	{
		using value_type = T;
		using pointer = T * ;
		using const_pointer = const T *;
		using reference = T & ;
		using const_reference = const T &;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;

		Allocator() = default;
		constexpr Allocator(const Allocator & other) noexcept = default;
		template<class Other>
		constexpr Allocator(const Allocator<Other> & other) noexcept {}

		[[nodiscard]] pointer allocate(size_type n) {
			return static_cast<pointer>(allocateRaw<align_of<value_type>>(n * sizeof(T)));
		}

		void deallocate(pointer p) {
			deallocateRaw<align_of<pointer>>(p);
		}

		void construct(pointer p, const_reference val) {
			allocatePlace<T>(p, val);
		}

		void constructN(pointer p, size_type n) {
			for (size_t i = 0; i < n; i++)
			{
				allocatePlaceDefault<T>(p + i);
			}
		}

		[[nodiscard]] pointer create(size_type n, const_reference val) {
			auto tmp_ptr = allocate(n);
			construct(tmp_ptr, val);
			return tmp_ptr;
		}

		[[nodiscard]] pointer createN(size_type n) {
			auto tmp_ptr = allocate(n);
			constructN(tmp_ptr, n);
			return tmp_ptr;
		}

		void destroy(pointer p) {
			p->~T();
		}

		~Allocator() = default;
	};

	template<class T>
	struct Deleter
	{
		constexpr Deleter() noexcept = default;

		constexpr void operator()(T* ptr) const {
			deallocate(ptr);
		}
	};

	template<class T>
	struct Deleter<T[]>
	{
		constexpr Deleter() noexcept = default;

		template< class U >
		constexpr void operator()(U* ptr) const {
			deallocateArr(ptr);
		}
	};

	template<typename InputIt, typename OutputIt>
	void memCopy(InputIt first, InputIt last, OutputIt d_first) {
		for (; first != last; first++, d_first++)
		{
			*d_first = *first;
		}
	}

	template<typename InContainer, typename OutContainer>
	void memCopy(InContainer& source, OutContainer& dest) {
		memCopy(source.begin(), source.end(), dest.begin());
	}

	template<typename InputIt, typename OutputIt>
	void memCopyBack(InputIt first, InputIt last, OutputIt d_last) {
		while (first != last)
		{
			*(--d_last) = *(--last);
		}
	}

	template<typename InContainer, typename OutContainer>
	void memCopyBack(InContainer& source, OutContainer& dest) {
		memCopyBack(source.begin(), source.end(), dest.begin());
	}

	template<typename InputIt, typename OutputIt>
	void memMove(InputIt first, InputIt last, OutputIt d_first) {
		if (d_first < first)
		{
			memCopy(first, last, d_first);
		}
		else if (d_first > first)
		{
			auto d_last = d_first + (last - first);
			memCopyBack(first, last, d_last);
		}
	}

	template<typename InContainer, typename OutContainer>
	void memMove(InContainer& source, OutContainer& dest) {
		memMove(source.begin(), source.end(), dest.begin());
	}

} // !namespace mem

#endif // !MEMORY_HPP
