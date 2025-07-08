#ifndef _ITERABLE_HPP_
#define _ITERABLE_HPP_

#define ITERABLE_VERSION {1, 1, 0}

#include <stdio.h>



template<typename T>
class IterableArray;

template<typename T>
class Iterable {
	friend class IterableArray<T>;

	IterableArray<T>* parent = NULL;
	Iterable<T>* prev = NULL;
	Iterable<T>* next = NULL;

	void ClearRelationship();

public:
	~Iterable() {
		ClearRelationship();
	}

//	T* operator T*() {
//		return (T*)this;
//	}

  void* GetOwner() {
    return parent->owner;
  }

	 T* Prev() {
		  return (T*)prev;
	 }

	 T* Next() {
		  return (T*)next;
	 }
};


template<typename T>
class IterableArray {
	friend class Iterable<T>;

	void* owner = NULL;
	Iterable<T>* first = NULL;
	Iterable<T>* last = NULL;

public:
	class Iterator {
		Iterable<T>* prev, *now, *next;

		Iterator& Prev() {
			next = now;
			now = prev;
			prev = prev ? prev->prev : NULL;
			return *this;
		}

		Iterator& Next() {
			prev = now;
			now = next;
			next = next ? next->next : NULL;
			return *this;
		}

	public:
		Iterator(Iterable<T>* _prev, Iterable<T>* _now, Iterable<T>* _next) {
			prev = _prev;
			now = _now;
			next = _next;
		}

		Iterator(Iterable<T>* _now) {
			now = _now;

			prev = now ? now->prev : NULL;
			next = now ? now->next : NULL;
		}

		bool operator!=(Iterator& ) {
			return now != NULL;
		}

		T* operator*() {
			return (T*)now;
		}


		inline Iterator& operator--() {
	    	return Prev();
		}

		inline Iterator& operator--(int) {
			return Prev();
		}

		inline Iterator& operator++() {
			return Next();
		}
		inline Iterator& operator++(int) {
			return Next();
		}
	};

	Iterator begin() {
		return Iterator(NULL, first, first ? first->next : NULL);
	}

	Iterator end() {
		return Iterator(last ? last->prev : NULL, last, NULL);
	}





	inline IterableArray() {
		owner = first = last = NULL;
	}

	~IterableArray() {
		Clear();
	}




	template<typename OwnerType>
	OwnerType* SetOwner(OwnerType* _owner) {
  if (owner == NULL) {
		  owner = (void*)_owner;
  }
  return owner;
	}

	void* GetOwner() {
		return owner;
	}


	T* InsertBefore(T* _after, T* _before) {
		Iterable<T>* after = (Iterable<T>*)_after;
		Iterable<T>* before = (Iterable<T>*)_before;

		// insert first when after == NULL
		if (after == NULL) {
			after = first;
		}

		// clear old relationship
		before->ClearRelationship();

		// inset part1
		before->parent = this;
		before->next = after;

		// after == first == NULL, meaning array is empty
		// init array
		if (after == NULL) {
			before->prev = NULL;
			first = before;
			last = before;
			return (T*)_before;
		}

		// insert part2
		before->prev = after->prev;

		if (after->prev) {
			after->prev->next = before;
		}
		else {	// if after->prev == NULL, meaning new insert object will be first
			first = before;
		}
		after->prev = before;
		return (T*)_before;
	}
	
	T* InsertBefore(int index, T* _before) {
		return InsertBefore((*this)[index], _before);
	}
	

	T* InsertAfter(T* _before, T* _after) {
		Iterable<T>* after = (Iterable<T>*)_after;
		Iterable<T>* before = (Iterable<T>*)_before;

		// insert last when before == NULL
		if (before == NULL) {
			before = last;
		}

		// clear old relationship
		after->ClearRelationship();

		// insert part1
		after->parent = this;
		after->prev = before;

		// before == last == NULL, meaning array is empty
		// init array
		if (before == NULL) {
			after->next = NULL;
			first = after;
			last = after;
			return (T*)after;
		}

		// insert  part2
		after->next = before->next;

		if (before->next) {
			before->next->prev = after;
		}
		else {	// if before->next == NULL, meaning new insert will be last
			last = after;
		}
		before->next = after;
		return (T*)after;
	}
	
	T* InsertAfter(int index, T* _after) {
		return InsertAfter((*this)[index], _after);
	}
	

	T* Add(T* _object) {
		Iterable<T>* object = (Iterable<T>*)_object;

		object->ClearRelationship();

		object->parent = this;
		object->prev = last;
		object->next = NULL;

		if (last) {
			last->next = object;
		}
		if (first == NULL) {
			first = object;
		}
		last = object;

		return (T*)object;
	}


	T* operator [](int index) {
		T* object;

		if (index < 0) {
			for (Iterator iter = end(); (object = *iter); iter--) {
				index++;
				if (index == 0) {
					return object;
				}
			}
			return NULL;
		}

		for (T* object : *this) {
			if (index == 0) {
				return object;
			}
			index--;
		}
		return NULL;
	}

	void DeleteIndex(int index) {
		delete (*this)[index];
	}

	void DeleteIndexes(int index, int count) {
		Iterator iter = Iterator((Iterable<T>*)(*this)[index]);

		while (count < 0) {
			delete *iter;
			iter--;
			count++;
		}
		while (count > 0) {
			delete *iter;
			iter++;
			count--;
		}
	}

	template<typename CheckType>
	bool IsContains(CheckType value) {
		for (T* object : *this) {
			if (object == value) {
				return true;
			}
		}
		return false;
	}




	int Length() {
		int total = 0;
		for (auto iter = begin(); *iter; iter++) {
			total++;
		}
		return total;
	}


	void Clear() {
		for (T* object : *this) {
			delete object;
		}
		first = NULL;
		last = NULL;
	}
};


template<typename T>
void Iterable<T>::ClearRelationship() {
	if (parent == NULL) {
		return;
	}

	if (prev) {
		prev->next = next;
	}
	else {
		parent->first = next;
	}

	if (next) {
		next->prev = prev;
	}
 else {
		parent->last = prev;
	}
}

#endif /* _ITERABLE_HPP_ */