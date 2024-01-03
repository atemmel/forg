#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include "uniquePtr.hpp"
#include "array.hpp"
#include "hash.hpp"

template<typename T>
class HashTable
{
private:
	template<typename T>
	struct Node
	{
		Node(T data) :
			data(data) {}

		int data;
		cor::UniquePtr<Node> next = nullptr;
	};
	template<typename T>
	using hashArr = cor::Array<cor::UniquePtr<Node<T>>>;

	hashArr<T> hashTable;
	size_t ssize = 0;

	//load factor for separate chaining between 1 and 3;
	const float DEFAULT_LOAD_FACTOR = 2;

	template<typename T>
	size_t hashFunk(T key)
	{
		return cor::Hash<T>{}(key) % this->hashTable.size();
	}

public:

	HashTable() :hashTable(0) {}

	HashTable(size_t size) :hashTable(size) {}

	template<typename RandomIt>
	HashTable(RandomIt first, RandomIt last, size_t size)
	{
		auto newSize = std::floor(size / 0.5);
		this->hashTable.resize(int(newSize));
		for (auto it = first; it != last; it++)
		{
			this->insert(*it);
		}
	}

	HashTable(const HashTable& other) :hashTable(other.hashTable.size())
	{
		for (auto &e : other.hashTable)
		{
			if (e)
			{
				auto head = e.get();
				while (head) {
					this->insert(head->data);
					head = head->next.get();
				}
			}
		}
	}

	HashTable(HashTable&& other) {
		this->swap(other);
	}

	template <typename InputIt>
	HashTable(InputIt first, InputIt last) :HashTable(first, last, (last - first)) {}

	HashTable& operator=(const HashTable& other) {
		HashTable temp(other);
		this->swap(temp);
		return *this;
	}
	HashTable& operator=(HashTable&& other)
	{
		this->swap(other);
		return *this;
	}

	template<typename T>
	Node<T>* search(T key)
	{
		size_t index = this->hashFunk(key);

		auto node = this->hashTable.at(index).get();

		while (node != nullptr)
		{
			if (node->data == key)
			{
				return node;
			}
			node = node->next.get();
		}

		return nullptr;
	}

	template<typename T>
	void insert(T key)
	{
		auto newNode = cor::makeUnique<Node<T>>(key);

		size_t index = this->hashFunk(key);

		if (this->hashTable[index] == nullptr)
		{
			this->hashTable[index] = cor::isMovable(newNode);
			ssize++;
		}
		else
		{
			auto curr = this->hashTable[index].get();
			while (curr->next != nullptr)
			{
				curr = curr->next.get();
			}
			curr->next = cor::isMovable(newNode);
			ssize++;
		}

		if (loadFactor() > DEFAULT_LOAD_FACTOR)
		{
			this->resizeAndRehash(ssize * 2);
		}
	}

	template <typename T>
	void remove(T key) {
		if (search(key))
		{
			size_t index = this->hashFunk(key);

			if (this->hashTable.at(index).get()->data == key)
			{
				this->hashTable.at(index) = cor::isMovable(this->hashTable.at(index).get()->next);
			}
			else
			{
				auto head = this->hashTable.at(index).get();
				while (head->next && head->next->data != key)
				{
					head = head->next.get();
				}
				if (head->next) {
					head->next = cor::isMovable(head->next->next);
				}
			}
			this->ssize--;
		}
	}

	void resizeAndRehash(size_t newSize) {
		HashTable<T> newTable(newSize);

		for (auto &e : hashTable)
		{
			if (e)
			{
				auto head = e.get();
				while (head) {
					newTable.insert(head->data);
					head = head->next.get();
				}
			}

		}
		this->swap(newTable);
	}

	size_t size() const {
		return ssize;
	}

	hashArr<T>& data() {
		return hashTable;
	}

	float loadFactor() const { return (float)this->size() / this->hashTable.size(); }

	void swap(HashTable& other) {
		this->hashTable.swap(other.hashTable);
		cor::swap(this->ssize, other.ssize);
	}
	void print()
	{
		//for (auto &node : this->hashTable)
		//{
		//	if (node == nullptr)
		//	{
		//		std::cout << "null\n";
		//	}
		//	else if (node->next == nullptr)
		//	{
		//		std::cout << node->data << "-> null\n";
		//	}
		//	else {

		//		while (node->next != nullptr)
		//		{
		//			std::cout << node->data << " -> ";
		//			node = node->next;
		//		}
		//		std::cout << node->data << "-> null\n";
		//	}
		//}
	}
};

#endif // !HASH_TABLE_HPP
