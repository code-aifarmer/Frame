#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_
#pragma once
#include<exception>
#include<type_traits>

template<typename T>
class TypeComparator {
public :
	bool(*equalP)(T, T) = this->equals<std::is_arithmetic<T>::value || std::is_pointer<T>::value>;

	template<bool B>
	bool equals(T t1, T t2) {
		return false;
	}
	template<>
	bool equals<true>(T t1, T t2) {
		return t1 == t2;
	}
};

#ifndef NULL
#define NULL 0
#endif //! NULL
#if _HAS_EXCEPTIONS
/*
 * the super class of all Exceptions in LinkedList operation.
 * inherited from std::exception
 */
class ListException : std::exception {
public:
	ListException() : std::exception() {}
	ListException(char const* const _Message) : std::exception(_Message) {}
	ListException(char const* const _Message, int flag) : std::exception(_Message, flag) {}
};
/**
 * Subscript out of bounds exception,
 * when the incoming subscript exceeds the effective length of the linked list is thrown
 */
class IndexOutOfBoundException : public ListException {
public:
	IndexOutOfBoundException() : ListException() {}
	explicit IndexOutOfBoundException(char const* const _Message) : ListException(_Message) {}
	IndexOutOfBoundException(char const* const _Message, int flag) : ListException(_Message, flag) {}
};
#else // _HAS_EXCEPTIONS
/*
 * the super class of all Exceptions in LinkedList operation.
 * Not inherited from std::exception
 */
class ListException {
private:
	const char* _Ptr;
public:
	ListException() : _Ptr("An exception is thrown when operating LinkedList") {}
	ListException(char const* const _Message) : _Ptr(_Message) {}
	virtual const char* what() {
		return this->_Ptr;
	}
	virtual ~ListException() {}
};
/**
 * Subscript out of bounds exception,
 * when the incoming subscript exceeds the effective length of the linked list is thrown
 */
class IndexOutOfBoundException : public ListException {
public:
	IndexOutOfBoundException() : ListException() {}
	IndexOutOfBoundException(const char* _Message = "Linked list index out of bounds") : ListException(_Message) {}
};
#endif // !_HAS_EXCEPTIONS

/*
 * LinkedList class:
 * Store data in a chain.
 * if exceptions are thrown and you want to handle all kinds of exceptions take place at LinkedList,
 * it's better to receive with `LiseException`, it is the super class of all exceptions of List.
 */
template <typename T>
class LinkedList {
protected :
	static TypeComparator<T> compHelper;
private :
	template<typename U>
	struct Node {
		U* t;
		Node* next;
		//Record construction type. If it is 0, it means that pointer type is not used for construction.
		//When type is 0, the data space in the node will be released while releasing the node space, 
		//Otherwise, the data won't be released.
		int type;
		Node() {
			this->t = NULL;
			this->next = NULL;
			type = 0;
		}
		Node(const T t) {
			this->t = new T(t);
			this->next = NULL;
			type = 0;
		}
		Node(T* t) {
			this->t = t;
			this->next = NULL;
			type = 1;
		}
		~Node() {
			if (!type)
				delete this->t;
		}
	};
	Node<T>* head;
	Node<T>* rear;
	unsigned count;
	Node<T>* iterator;
	unsigned i_pos;

	/**
	 * reset the iterator pointer and i_pos of linkedList
	 */
	void reset() {
		this->iterator = this->head;
		this->i_pos = 0;
	}

public:
	LinkedList() {
		this->rear = this->head = NULL;
		this->count = 0;
		this->iterator = NULL;
		this->i_pos = -1;
	}

	/**
	 * Overload subscript operator.
	 * Get the element of the specified subscript
	 *
	 * @param `index`: the specified subscript
	 * @return reference to the data corresponding to the subscript position
	 */
	T& operator [] (unsigned int index) {
		return this->get(index);
	}

	/**
	 * Overload assignment operator
	 * each element of linked list `list` will be copied,
	 * so that they won't effect each other when operating.
	 */
	LinkedList<T>& operator = (LinkedList<T>& list) {
		if (&list != this) {
			this->clear();
			Node<T>* pnode = list.head;
			for (unsigned i = 0; i < list.count; i++) {
				this->add(*pnode->t);
				pnode = pnode->next;
			}
		}
		return *this;
	}

	/**
	 * Get the number of data in the linked list
	 * @return the number of Data
	 */
	unsigned length() {
		return this->count;
	}

	/**
	 * Determine whether the linked list is empty
	 *
	 * @return if empty, return `true`. Else return `false`
	 */
	bool empty() {
		return !this->count;
	}

	/**
	 * Add elements to the end of the linked list.
	 * This will add a copy into the linked list,
	 * There's no influnce when operate the data in the list and the source data.
	 *
	 * @param `t`: the element need to add
	 */
	void add(const T t) {
		Node<T>* pnode = new Node<T>(t);
		if (this->head == NULL) {
			this->rear = this->head = pnode;
			reset();
			this->count++;
			return;
		}
		this->rear->next = pnode;
		this->rear = pnode;
		this->count++;
	}
	/**
	 * Add elements to the end of the linked list.
	 * This will add the pointer of data into linked list,
	 * so when you operate or release the source data,
	 * the linked list will also be affected.
	 *
	 * @param `t`: the element need to add
	 */
	void add(T* t) {
		Node<T>* pnode = new Node<T>(t);
		if (this->head == NULL) {
			this->rear = this->head = pnode;
			reset();
			this->count++;
			return;
		}
		this->rear->next = pnode;
		this->rear = pnode;
		this->count++;
	}
	/**
	 * Insert data into the specified position of the linked list
	 * This will add a copy into the linked list,
	 * There's no influnce when operate the data in the list and the source data.
	 *
	 * @param `t`: the element need to add
	 * @param `index`: the subscript to insert.
	 *		The data will be added into the end if `index` is larger than the length of linked list
	 */
	void add(unsigned index, const T t) {
		if (index >= this->count) {
			this->add(t);
			return;
		}
		if (index == 0) {
			Node<T>* pnode = new Node<T>(t);
			this->count++;
			pnode->next = this->head;
			this->head = pnode;
			return;
		}
		unsigned i = 0;
		if (this->i_pos < index)
			i = i_pos;
		else
			reset();
		for (; i < index - 1; i++) {
			this->iterator = iterator->next;
			this->i_pos++;
		}
		Node<T>* pnode = new Node<T>(t);
		pnode->next = this->iterator->next;
		this->iterator->next = pnode;
		this->count++;
	}
	/**
	 * Add elements to the end of the linked list.
	 * This will add the pointer of data into linked list,
	 * so when you operate or release the source data,
	 * the linked list will also be affected.
	 *
	 * @param `t`: the element need to add
	 * @param `index`: the subscript to insert.
	 *		The data will be added into the end if `index` is larger than the length of linked list
	 */
	void add(unsigned index, T* t) {
		if (index >= this->count) {
			this->add(t);
			return;
		}
		if (index == 0) {
			Node<T>* pnode = new Node<T>(t);
			this->count++;
			pnode->next = this->head;
			this->head = pnode;
			return;
		}
		unsigned i = 0;
		if (this->i_pos < index)
			i = i_pos;
		else
			reset();
		for (; i < index - 1; i++) {
			this->iterator = iterator->next;
			this->i_pos++;
		}
		Node<T>* pnode = new Node<T>(t);
		pnode->next = this->iterator->next;
		this->iterator->next = pnode;
		this->count++;
	}

	/*
	 * get the element in a certain subscript
	 *
	 * @param `index`: the subscript of element to get
	 * @return Reference to the data corresponding to the subscript position
	 * @throws IndexOutOfBoundException the subscript out of range when get element of list
	 */
	T& get(unsigned index) {
		if (index >= this->count)
			throw* new IndexOutOfBoundException("the subscript out of range when get element of list");
		if (index == 0) {
			return *this->head->t;
		}
		unsigned i = 0;
		if (this->i_pos < index)
			i = i_pos;
		else
			reset();
		for (; i < index; i++) {
			this->iterator = iterator->next;
			this->i_pos++;
		}
		return *this->iterator->t;
	}

	/**
	 * Traverse the linked list and use the external interface `todo`
	 * to manipulate each data in the linked list
	 *
	 * @param `todo`: void(*todo)(T&): the operate of each element of linked list
	 */
	void travel(void(*todo)(T& t)) {
		Node<T>* pnode = this->head;
		for (unsigned i = 0; i < this->count; i++) {
			todo(*pnode->t);
			pnode = pnode->next;
		}
	}
	/**
	 * Traverse the linked list and use the external interface `todo`
	 * to manipulate each data in the linked list
	 *
	 * @param `todo`: bool(*todo)(T&): the operate of each element of linked list.
	 *			when the return of `todo` is `false`, The traversal operation will terminate
	 */
	bool travel(bool(*todo)(T& t)) {
		Node<T>* pnode = this->head;
		for (unsigned i = 0; i < this->count; i++) {
			if (!todo(*pnode->t)) return false;
			pnode = pnode->next;
		}
		return true;
	}

	/**
	 * Delete an element with specific subscripts in the linked list
	 *
	 * @param `index`: (default _ 0) the subscript of element to delete.
	 *			Look out that if the paramater is 0, please pass the paramater `0U` or null-param.
	 * @throws IndexOutOfBoundException The subscript exceeds the valid range when deleting the linked list data
	 */
	void drop(unsigned index = 0) {
		if (index >= this->count) {
			throw* new IndexOutOfBoundException("The subscript exceeds the valid range when deleting the linked list data");
		}
		if (index == 0) {
			Node<T>* pnode = this->head;
			this->head = this->head->next;
			this->count--;
			if (this->iterator == pnode)
				reset();
			pnode->~Node();
			return;
		}
		unsigned i = 0;
		if (index >= this->i_pos)
			i = i_pos;
		else
			reset();
		for (; i < index - 1; i++) {
			this->iterator = this->iterator->next;
			this->i_pos++;
		}
		Node<T>* ptemp = this->iterator->next;
		this->iterator->next = this->iterator->next ? this->iterator->next->next : NULL;
		delete ptemp;
		this->count--;
	}
	/**
	 * delete all of elements that can satisfy the `isDelete` function
	 *
	 * @param `t`: Reference element. Passed as the first parameter to the `isDelete` function.
	 * @param `isDelete`: bool(*isDelete)(T&, T&): to compare parameter `t` and each element of list.
	 *			the elements in the linked list will be deleted when the return of `isDelete` is `true`
	 * @return the number of elements deleted
	 */
	unsigned drop(T& t, bool(*isDelete)(T&, T&)) {
		if (this->count == 0) return 0;
		unsigned delNum = 0;
		Node<T>* pnode = this->head->next, * pfount = this->head;
		for (int i = 1; i < this->count; ) {
			if (isDelete(t, *pnode->t)) {
				Node<T>* ptemp = pnode;
				pfount->next = pnode->next;
				pnode = pnode->next;
				delete ptemp;
				this->count--;
				delNum++;
				continue;
			}
			pnode = pnode->next;
			pfount = pfount->next;
			i++;
		}
		if (isDelete(t, *this->head->t)) {
			this->drop();
			delNum++;
		}
		return delNum;
	}
	/**
	 * delete all of elements that can satisfy the `isDelete` function
	 *
	 * @param `isDelete`: bool(*isDelete)(T&): to determine whether the elements in the linked list
	 *			need to be deleted. the elements in the linked list will be deleted when the return
	 *			of `isDelete` is `true`
	 * @return the number of elements deleted
	 */
	unsigned drop(bool(*isDelete)(T& t)) {
		if (this->count == 0) return 0;
		unsigned delNum = 0;
		Node<T>* pnode = this->head->next, * pfount = this->head;
		for (int i = 1; i < this->count; ) {
			if (isDelete(*pnode->t)) {
				Node<T>* ptemp = pnode;
				pfount->next = pnode->next;
				pnode = pnode->next;
				delete ptemp;
				this->count--;
				delNum++;
				continue;
			}
			pnode = pnode->next;
			pfount = pfount->next;
			i++;
		}
		if (isDelete(*this->head->t)) {
			this->drop();
			delNum++;
		}
		return delNum;
	}

	/**
	 * Replace the specified subscript data with the specified data.
	 * The original data and the data in the linked list will not affect each other when operating.
	 *
	 * @param `t`: the data to replace
	 * @param `index`: the subscript of data to be replaced
	 * @return reference to the original data at the replacement location
	 * @throws IndexOutOfBoundException The subscript exceeds the valid range when replacing the linked list data
	 */
	T& replace(unsigned index, const T t) {
		if (index >= this->count)
			throw* new IndexOutOfBoundException("The subscript exceeds the valid range when replacing the linked list data");
		unsigned i = 0;
		if (this->i_pos < index)
			i = i_pos;
		else
			reset();
		for (; i < index; i++) {
			this->iterator = this->iterator->next;
			this->i_pos++;
		}
		T* pt = this->iterator->t;
		this->iterator->t = new T(t);
		return *pt;
	}
	/**
	 * Replace the specified subscript data with the specified data.
	 * The original data and the data in the linked list have the same
	 * address, and they will affect each other during operation.
	 *
	 * @param `t`: the data to replace
	 * @param `index`: the subscript of data to be replaced
	 * @return reference to the original data at the replacement location
	 * @throws IndexOutOfBoundException The subscript exceeds the valid range when replacing the linked list data
	 */
	T& replace(unsigned index, T* t) {
		if (index >= this->count)
			throw* new IndexOutOfBoundException("The subscript exceeds the valid range when replacing the linked list data");
		unsigned i = 0;
		if (this->i_pos < index)
			i = i_pos;
		else
			reset();
		for (; i < index; i++) {
			this->iterator = this->iterator->next;
			this->i_pos++;
		}
		T* pt = this->iterator->t;
		this->iterator->t = t;
		return *pt;
	}

	/*
	 * append LinkedList `list` to the end of this linked list.
	 * All the data will be copy so that there won't effect each other when operating data in two linked lists.
	 *
	 * @param `list`: the list to append
	 */
	void append(LinkedList<T>& list) {
		Node<T>* pnode = list.head;
		for (unsigned i = 0; i < list.count; i++) {
			this->add(*pnode->t);
			pnode = pnode->next;
		}
	}
	/*
	 * append LinkedList `list` to the end of this linked list.
	 *
	 * @param `list`: the list to append.
	 * @param `isCopy`: if `isCopy` is `true`, all the data will be copy so that there won't effect each other when
	 *			operating data in two linked lists. If `false`, the datas in two linked list will have the same address;
	 */
	void append(LinkedList<T>& list, bool isCopy) {
		if (isCopy) this->append(list);
		Node<T>* pnode = list.head;
		for (unsigned i = 0; i < list.count; i++) {
			this->add(pnode->t);
			pnode = pnode->next;
		}
	}

	/*
	 * clear all elements of linked list
	 * if the data is accepted with pointer, it won't be released
	 */
	void clear() {
		reset();
		for (unsigned i = 0; i < this->count; i++) {
			Node<T>* ptemp = this->iterator->next;
			delete this->iterator;
			this->iterator = ptemp;
		}
		this->count = 0;
		this->i_pos = -1;
		this->head = NULL;
		this->rear = NULL;
		this->iterator = NULL;
	}

	/*
	 * destructor. clear elements of linked list and release space.
	 * See `void template<class T>LinkedList::clear()` or detials
	 */
	virtual ~LinkedList() {
		this->clear();
	}

	/*
	 * Determine whether there is specific data in the linked list
	 *
	 * @param `t`: the determined data. it will be as the first parameter of `equals`
	 * @param `equals`: bool (*equals)(T&, T&): determine whether each element of list is equals to parameter `t`
	 *			{default:: if type `T` is an arithmetic type or pointer type, it will use the default operator`==`.
	 *			 Otherwise, `false` will be return no matter which two data are involved in the comparison.}
	 * @return `true` if contains. else `false` will be returned.
	 */
	bool contain(T t, bool(*equals)(T, T) = compHelper.equalP) {
		Node<T>* pnode = this->head;
		for (unsigned i = 0; i < this->count; i++) {
			if (equals(t, *pnode->t))
				return true;
			pnode = pnode->next;
		}
		return false;
	}

};
#endif // !_LINKED_LIST_H_
