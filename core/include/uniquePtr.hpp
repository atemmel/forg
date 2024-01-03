#ifndef UNIQUEPTR_HPP
#define UNIQUEPTR_HPP

#include <type_traits>
#include "utility.hpp"
#include "memory.hpp"
#include "type_traits.hpp"
#include "types.hpp"

namespace cor {

	//non array types
	template<class T, class Deleter = mem::Deleter<T>>
	class UniquePtr
	{
	public:
		using deleterNoRef = typename cor::RemoveReference_T<Deleter>;
		using Pointer = typename cor::addPointer_T<T, deleterNoRef>;
		using Elem_t = T;

		//Constructors
		UniquePtr() :pointer(nullptr) {}
		UniquePtr(Pointer p) :pointer(p) {}

		UniquePtr(UniquePtr&& other) {
			this->reset(other.release());
			deleter = cor::forward<Deleter>(other.getDeleter());
		}

		//assigns
		UniquePtr& operator =(UniquePtr&& other) noexcept {
			{
				this->reset(other.release());
				deleter = cor::forward<Deleter>(other.getDeleter());
				return *this;
			}
		}
		UniquePtr& operator=(nullptr_t) noexcept {
			this->reset();
			return *this;
		}

		//deleted functionality
		UniquePtr(const UniquePtr& other) = delete;
		UniquePtr& operator =(const UniquePtr& other) = delete;

		//Modifiers
		Pointer release() noexcept {
			auto oldPtr = this->pointer;
			this->pointer = nullptr;

			return oldPtr;
		}
		void reset(Pointer ptr = Pointer()) noexcept {
			auto oldPtr = this->pointer;
			this->pointer = ptr;

			if (oldPtr)
			{
				this->getDeleter()(oldPtr);
			}
		}
		void swap(UniquePtr& other) noexcept {
			cor::swap(this->pointer, other.pointer);
			cor::swap(this->deleter, other.deleter);
		}

		//Observers
		Pointer get() const noexcept {
			return pointer;
		}
		Deleter& getDeleter() {
			return deleter;
		}
		explicit operator bool() const noexcept {
			return this->get() != nullptr;
		}

		//dereferences pointer to the managed object
		Pointer operator->() const noexcept {
			return pointer;
		}
		Elem_t operator*() const noexcept {
			return *get();
		}

		//Destructor
		~UniquePtr() {
			if (get() != nullptr)
			{
				this->getDeleter()(get());
			}
		}


	private:
		Pointer pointer = nullptr;
		Deleter deleter;
	};

	//array types
	template<class T, class Deleter>
	class UniquePtr<T[], Deleter>
	{
	public:
		using deleterNoRef = typename cor::RemoveReference_T<Deleter>;
		using Pointer = typename cor::addPointer_T<T, deleterNoRef>;
		using Elem_t = T;

		//Constructors
		UniquePtr() :pointer(nullptr) {}

		template< class U >
		explicit UniquePtr(U p) noexcept :pointer(p) {}

		UniquePtr(UniquePtr&& other) {
			this->reset(other.release());
			deleter = cor::forward<Deleter>(other.getDeleter());
		}

		//assigns
		UniquePtr& operator =(UniquePtr&& other) noexcept {
			{
				this->reset(other.release());
				deleter = cor::forward<Deleter>(other.getDeleter());
				return *this;
			}
		}
		UniquePtr& operator=(nullptr_t) noexcept {
			this->reset();
			return *this;
		}

		//deleted functionality
		UniquePtr(const UniquePtr& other) = delete;
		UniquePtr& operator =(const UniquePtr& other) = delete;

		//Modifiers
		Pointer release() noexcept {
			auto oldPtr = this->pointer;
			this->pointer = nullptr;

			return oldPtr;
		}
		template<class U>
		void reset(U ptr) noexcept {
			auto oldPtr = this->pointer;
			this->pointer = ptr;

			if (oldPtr)
			{
				this->getDeleter()(oldPtr);
			}
		}
		void reset(std::nullptr_t = nullptr) noexcept {
			reset(Pointer());
		}
		void swap(UniquePtr& other) noexcept {
			cor::swap(this->pointer, other.pointer);
			cor::swap(this->deleter, other.deleter);
		}

		//Observers
		Pointer get() const noexcept {
			return pointer;
		}
		Deleter& getDeleter() {
			return deleter;
		}
		explicit operator bool() const noexcept {
			return this->get() != nullptr;
		}

		//dereferences pointer to the managed object
		Pointer operator->() const noexcept {
			return pointer;
		}
		Elem_t operator*() const noexcept {
			return *get();
		}

		Elem_t& operator[](cor::size_t i) const {
			return this->get()[i];
		}

		//Destructor
		~UniquePtr() {
			if (get() != nullptr)
			{
				this->getDeleter()(get());
			}
		}

	private:
		Pointer pointer;
		Deleter deleter;
	};

	//non array type
	template<class T, class ...Args, cor::EnableIf_T<!cor::IsArray_T<T>, bool> = true >
	constexpr UniquePtr<T> makeUnique(Args && ...args)
	{
		return UniquePtr<T>(mem::allocate<T>(args...));
	}

	//array type
	template< class T, cor::EnableIf_T<cor::IsArray_T<T> && std::extent_v<T> == 0, bool> = true >
	constexpr UniquePtr<T> makeUnique(cor::size_t size) {
		return UniquePtr<T>(mem::allocate_r_extent<T>(size));
	}

	template<class T, class D>
	void swap(UniquePtr<T, D>& lhs, UniquePtr<T, D>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	template< class T, class D >
	bool operator==(const UniquePtr<T, D>& ptr, nullptr_t) noexcept {
		return !ptr;
	}

	template< class T, class D >
	bool operator!=(const UniquePtr<T, D>& ptr, nullptr_t) noexcept {
		return (bool)ptr;
	}

} // !namespace cor

#endif // !UNIQUEPTR_HPP
