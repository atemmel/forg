#include "string.hpp"

cor::String::String(size_t count, char chr) :ssize(count)
{
	this->ptr = allocator.createN(this->ssize + 1);
	size_t i = 0;
	for (; i < count; i++)
	{
		this->ptr[i] = chr;
	}
	this->ptr[i] = '\0';
}

cor::String::String(const_pointer s)
	:String(s, cor::strlen(s))
{}

cor::String::String(const_pointer s, size_t count)
{
	this->ssize = count;
	this->ptr = allocator.createN(this->ssize + 1);
	mem::memCopy(s, s + this->ssize, this->ptr);
}

cor::String::String(const String & other)
	:String(other.data(), other.size())
{}

cor::String::String(String && other) noexcept
{
	this->swap(other);
}

cor::String::String(std::initializer_list<char> ilist)
	:String(ilist.begin(), ilist.size())
{}

cor::String & cor::String::operator=(const String & str)
{
	if (*this != str)
	{
		auto temp(str);
		this->swap(temp);
	}
	return *this;
}

cor::String & cor::String::operator=(String && str) noexcept
{
	if (*this != str)
	{
		this->swap(str);
	}
	return *this;
}

cor::String & cor::String::operator=(const_pointer s)
{
	if (*this != s)
	{
		cor::String temp(s);
		this->swap(temp);
	}
	return *this;

}

cor::String & cor::String::operator=(std::initializer_list<char> ilist)
{
	cor::String temp(ilist);
	this->swap(temp);
	return *this;
}

cor::String::reference cor::String::operator[](size_t index)
{
	return this->data()[index];
}

cor::String::const_reference cor::String::operator[](size_t index) const
{
	return this->data()[index];
}

cor::String::reference cor::String::front()
{
	return this->data()[0];
}

cor::String::const_reference cor::String::front() const
{
	return this->data()[0];
}

cor::String::reference cor::String::back()
{
	return this->data()[this->size() - 1];
}

cor::String::const_reference cor::String::back() const
{
	return this->data()[this->size() - 1];
}

cor::String::pointer cor::String::begin() noexcept
{
	return this->data();
}

cor::String::const_pointer cor::String::begin() const noexcept
{
	return this->data();
}

cor::String::pointer cor::String::end() noexcept
{
	return this->data() + this->size();
}

cor::String::const_pointer cor::String::end() const noexcept
{
	return this->data() + this->size();
}

constexpr size_t cor::String::size() const noexcept
{
	return this->ssize;
}



cor::String::const_pointer cor::String::data() const noexcept
{
	return this->ptr;
}

cor::String::pointer cor::String::data() noexcept
{
	return this->ptr;
}

cor::Slice<char> cor::String::slice(size_t first, size_t last)
{
	return Slice<char>(this->begin() + first, this->begin() + last);
}

cor::Slice<char> cor::String::slice()
{
	return Slice<char>(this->begin(), this->end());
}

constexpr void cor::String::swap(String & other) noexcept
{
	cor::swap(this->ssize, other.ssize);
	cor::swap(this->ptr, other.ptr);
}

cor::String::~String()
{
	allocator.deallocate(this->ptr);
}

bool cor::operator==(const cor::String & lhs, const cor::String & rhs) noexcept
{
	return cor::strcmp(lhs.data(), rhs.data()) == 0;
}

bool cor::operator!=(const cor::String & lhs, const cor::String & rhs) noexcept
{
	return cor::strcmp(lhs.data(), rhs.data()) != 0;
}

bool cor::operator==(const cor::String & lhs, const char * rhs) noexcept
{
	return cor::strcmp(lhs.data(), rhs) == 0;
}

bool cor::operator!=(const cor::String & lhs, const char * rhs) noexcept
{
	return cor::strcmp(lhs.data(), rhs) != 0;
}
