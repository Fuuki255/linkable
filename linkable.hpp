#ifndef _LINKABLE_HPP_
#define _LINKABLE_HPP_

#define LINKABLE_VERSION {2,0,0}

#include <exception>
#include <iostream>






class TLinkable;
template<typename Object, typename Owner = void> class TLinkableList;


// TwoLinkable, a full linkable system to controll objects
class TLinkable {
private:
    template<typename Object, typename Owner> friend class TLinkableList;

    TLinkable* tlLeft = NULL;
    TLinkable* tlRight = NULL;
    TLinkableList<TLinkable>* tlParent = NULL;

    void tlUnlinkFast();

public:
    void* tlGetOwner() const;

    TLinkable* tlPrev() const {
        return tlLeft;
    }

    TLinkable* tlNext() const {
        return tlRight;
    }

    TLinkable* tlUnlink() {
        tlUnlinkFast();
        tlLeft = tlRight = NULL;
        tlParent = NULL;
        return this;
    }


    virtual ~TLinkable() {
        tlUnlinkFast();
    }
};




// TwoLinkableList
template<typename Object, typename Owner>
class TLinkableList {
private:
    friend class TLinkable;

    TLinkable* tlFirst = NULL, *tlLast = NULL;
    Owner* tlOwner = NULL;

public:
    class Iterator {
        Iterator& Prev() {
            next = now;
            now = prev;
            prev = prev ? prev->tlLeft : NULL;
            return *this;
        }

        Iterator& Next() {
            prev = now;
            now = next;
            next = next ? next->tlRight : NULL;
            return *this;
        }
    public:
        TLinkable* prev, *now, *next;

        Iterator(TLinkable* position) {
            now = position;
            prev = now ? now->tlLeft : NULL;
            next = now ? now->tlRight : NULL;
        }

        Iterator(TLinkable* _prev, TLinkable* _now, TLinkable* _next) {
            prev = _prev;
            now = _now;
            next = _next;
        }

        Object* operator*() {
            return (Object*)now;
        }

        bool operator!=(Iterator& other) {
            return now != other.now;
        }


        inline Iterator& operator--() {
            return Prev();
        }

        inline Iterator& operator++() {
            return Next();
        }

        inline Iterator& operator--(int) {
            return Prev();
        }

        inline Iterator& operator++(int) {
            return Next();
        }


        inline Iterator& operator-(int times) {
            if (times < 0) {
                while (times++) {
                    Next();
                }
                return *this;
            }

            times = times ? times : 1;
            while (times--) {
                Prev();
            }
            return *this;
        }

        inline Iterator& operator+(int times) {
            if (times < 0) {
                while (times++) {
                    Prev();
                }
                return *this;
            }

            times = times ? times : 1;
            while (times--) {
                Next();
            }
            return *this;
        }
    };

    Iterator begin() const {
        return Iterator(NULL, tlFirst, tlFirst ? tlFirst->tlRight : NULL);
    }

    Iterator end() const {
        return Iterator(tlLast, NULL, NULL);
    }


    virtual ~TLinkableList() {
        tlClear();
    }

    TLinkableList<Object, Owner>& tlClear() {
        for (Object* object : *this) {
            delete object;
        }
        tlFirst = tlLast = NULL;
        return *this;
    }


    TLinkableList<Object, Owner>& tlSetOwner(Owner* owner) {
        if (tlOwner == NULL) {
            tlOwner = owner;
        }
        return *this;
    }

    Owner* tlGetOwner() const {
        return tlOwner;
    }


    Object* operator[](int index) const {
        if (index < 0) {
            return *(end() + index);
        }

        return *(begin() + index);
    }

    Object* tlPop(int index) {
        TLinkable* tlinkable = (TLinkable*)(*this)[index];
        return (Object*)tlinkable->tlUnlink();
    }

    int tlLength() const {
        int total = 0;
        for (Iterator iter = begin(); *iter; iter++) {
            total++;
        }
        return total;
    }

    bool tlIsContains(Object* check) const {
        for (Object* object : *this) {
            if (*object == *check) {
                return true;
            }
        }
        return false;
    }

    template<typename T>
    bool tlIsContains(T value) const {
        for (Object* object : *this) {
            if (*object == value) {
                return true;
            }
        }
        return false;
    }



    Object* tlAdd(Object* object) {
        if (object == NULL) {
            throw std::runtime_error("Null pointer exception: object is NULL");
        }

        TLinkable* tlinkable = (TLinkable*)object;
        tlinkable->tlUnlinkFast();

        tlinkable->tlParent = (TLinkableList<TLinkable>*)this;
        tlinkable->tlLeft = tlLast;
        tlinkable->tlRight = NULL;

        if (tlLast) {
            tlLast->tlRight = tlinkable;
        }
        else {
            tlFirst = tlinkable;
        }
        tlLast = tlinkable;

        return object;
    }


    Object* tlInsertBefore(Object* position, Object* object) {
        if (object == NULL) {
            throw std::runtime_error("Null pointer exception: object is NULL");
        }

        TLinkable* pos = (TLinkable*)position;
        if (pos == NULL && pos->tlParent != (TLinkableList<TLinkable>*)this) {
            throw std::runtime_error("Insert exception: object of 'position' not the list object");
        }

        TLinkable* tlinkable = (TLinkable*)object;

        tlinkable->tlUnlinkFast();

        // insert part1
        tlinkable->tlParent = (TLinkableList<TLinkable>*)this;
        tlinkable->tlRight = pos;

        // position == first == NULL, meaning array is empty
		// init array
		if (position == NULL) {
			tlinkable->tlLeft = NULL;
			tlFirst = tlinkable;
			tlLast = tlinkable;
			return object;
		}

		// insert part2
		tlinkable->tlLeft = position->tlLeft;

		if (position->tlLeft) {
			position->tlLeft->tlRight = tlinkable;
		}
		else {	// if position->tlLeft == NULL, meaning new insert object will be first
			tlFirst = tlinkable;
		}
		position->tlLeft = tlinkable;
		return object;
    }

    Object* tlInsertAfter(Object* position, Object* object) {
        if (object == NULL) {
            throw std::runtime_error("Null pointer exception: object is NULL");
        }
        
        TLinkable* pos = (TLinkable*)position;
        if (pos == NULL && pos->tlParent != (TLinkableList<TLinkable>*)this) {
            throw std::runtime_error("Insert exception: object of 'position' not the list object");
        }

        TLinkable* tlinkable = (TLinkable*)object;

        tlinkable->tlUnlinkFast();

        // insert part1
        tlinkable->tlParent = (TLinkableList<TLinkable>*)this;
        tlinkable->tlLeft = pos;
        tlinkable->tlRight = pos->tlRight;

        // position == last == NULL, meaning array is empty
		// init array
		if (position== NULL) {
			tlinkable->tlRight = NULL;
			tlFirst = tlinkable;
			tlLast = tlinkable;
			return object;
		}

		// insert part2
		tlinkable->tlRight = position->tlRight;

		if (position->tlRight) {
			position->tlRight->tlLeft = tlinkable;
		}
		else {	// if position->tlRight == NULL, meaning new insert will be last
			tlLast = tlinkable;
		}
		position->tlRight = tlinkable;
		return object;
    }


    Object* tlInsertAfter(int position, Object* object) {
        return tlInsertAfter((*this)[position], object);
    }

    Object* tlInsertBefore(int position, Object* object) {
        return tlInsertBefore((*this)[position], object);
    }
};



void TLinkable::tlUnlinkFast() {
    if (tlParent == NULL) {
        return;
    }

    if (tlLeft) {
        tlLeft->tlRight = tlRight;
    }
    else {
        tlParent->tlFirst = tlRight;
    }

    if (tlRight) {
        tlRight->tlLeft = tlLeft;
    }
    else {
        tlParent->tlLast = tlLeft;
    }
}

void* TLinkable::tlGetOwner() const {
    return tlParent->tlOwner;
}










#endif // _LINKABLE_HPP_
