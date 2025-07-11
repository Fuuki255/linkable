#ifndef _LINKABLE_HPP_
#define _LINKABLE_HPP_

#define LINKABLE_VERSION {3,0,0}

#include <exception>
#include <iostream>



#ifndef _NO_TWOLINKABLE_

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

    TLinkable* tlFirst, *tlLast;
    Owner* tlOwner;

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



    TLinkableList() {
        tlFirst = tlLast = NULL;
        tlOwner = NULL;
    }

    TLinkableList(std::initializer_list<Object*> objects) {
        tlFirst = tlLast = NULL;
        tlOwner = NULL;
        
        for (Object* o : objects) {
            tlAdd(o);
        }
    }

    template<typename T>
    TLinkableList(std::initializer_list<T> values) {
        tlFirst = tlLast = NULL;
        tlOwner = NULL;
        
        for (const T& v : values) {
            tlAdd(new Object(v));
        }
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

    Object* tlGetEqual(Object* object) const {
        for (Object* check : *this) {
            if (*object == *check) {
                return check;
            }
        }
        return NULL;
    }

    template<typename T>
    Object* tlGetEqual(T value) const {
        for (Object* check : *this) {
            if (*check == value) {
                return check;
            }
        }
        return NULL;
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


#define TwoLinkable TLinkable
#define TwoLinkableList TLinkableList


#endif // _TWOLINKABLE_





#ifndef _NO_RIGHT_LINKABLE_

// Right-Linkable Queue
template<typename Object> class RLinkableQueue;


/* Right-Linkable、FIFO / FILO に特化するリンク構造

FIFO 1:
addLast();
object = getFirst();
deleteFirst();

FIFO 2:
addLast();
object = popFirst();
delete object;


FILO 1:
addFirst();
object = getFirst();
deleteFirst();

FILO 2:
addFirst();
object = popFirst();
delete object;

*/
class RLinkable {
    template<typename Object> friend class RLinkableQueue;

    RLinkable* rlRight = NULL;
    bool rlLocked = false;

public:
#ifndef NODEBUG
    virtual ~RLinkable() noexcept(false) {
        if (rlLocked) {
            throw std::runtime_error("couldn't delete RLinkable having parent!");
        }
    }
#else
    virtual ~RLinkable() {}
#endif
};



template<typename Object>
class RLinkableQueue {
public:
    friend class RLinkable;

    RLinkable* rlFirst = NULL, *rlLast = NULL;

public:
    class Iterator {
        friend class RLinkableQueue;

        Iterator& Next() {
            now = next;
            next = now ? now->rlRight : NULL;
            return *this;
        }

        Iterator(RLinkable* _now, RLinkable* _next) {
            now = _now;
            next = _next;
        }
    public:
        RLinkable* now, *next;

        Iterator(RLinkable* object) {
            now = object;
            next = now ? now->rlRight : NULL;
        }

        Iterator& operator++() {
            return Next();
        }

        Iterator& operator++(int) {
            return Next();
        }

        bool operator!=(Iterator& other) {
            return now != other.now;
        }

        Object* operator*() {
            return (Object*)now;
        }
    };

    Iterator begin() {
        return Iterator(rlFirst, rlFirst ? rlFirst->rlRight : NULL);
    }

    Iterator end() {
        return Iterator(NULL, NULL);
    }

    

    RLinkableQueue() {}

    RLinkableQueue(std::initializer_list<Object*> objects) {
        for (Object* o : objects) {
            rlAddLast(o);
        }
    }

    template<typename T>
    RLinkableQueue(std::initializer_list<T> values) {
        for (const T& v : values) {
            rlAddLast(new Object(v));
        }
    }

    ~RLinkableQueue() {
        for (Object* object : *this) {
            ((RLinkable*)object)->rlLocked = false;
            delete object;
        }
    }



    RLinkableQueue<Object>& rlClear() {
        for (Object* object : *this) {
            ((RLinkable*)object)->rlLocked = false;
            delete object;
        }
        rlFirst = rlLast = NULL;
        return *this;
    }

    Object* rlAddFirst(Object* object) {
        RLinkable* linkable = (RLinkable*)object;
        
        if (linkable->rlLocked) {
            throw std::runtime_error("couldn't add RLinkable having parent!");
        }
        linkable->rlLocked = true;

        if (rlFirst) {
            linkable->rlRight = rlFirst;
        }
        else {
            rlLast = linkable;
        }
        rlFirst = linkable;
        return object;
    }

    Object* rlAddLast(Object* object) {
        RLinkable* linkable = (RLinkable*)object;
        
        if (linkable->rlLocked) {
            throw std::runtime_error("couldn't add RLinkable having parent!");
        }
        linkable->rlLocked = true;

        if (rlLast) {
            rlLast->rlRight = linkable;
        }
        else {
            rlFirst = linkable;
        }
        rlLast = linkable;
        return object;
    }

    
    
    Object* rlGetFirst() const {
        return rlFirst;
    }

    Object* rlGetLast() const {
        return rlLast;
    }



    Object* rlPopFirst() {
        RLinkable* object = rlFirst;
        if (object) {
            rlFirst = object->rlRight;
            object->rlLocked = false;
        }
        return (Object*)object;
    }

    RLinkableQueue<Object>& rlDeleteFirst() {
        delete (Object*)rlPopFirst();
        return *this;
    }



    int rlLength() const {
        int total = 0;
        for (Iterator iter = begin(); *iter; iter++) {
            total++;
        }
        return total;
    }

    Object* rlGetEqual(Object* object) const {
        for (Object* check : *this) {
            if (*object == *check) {
                return check;
            }
        }
        return NULL;
    }

    template<typename T>
    Object* rlGetEqual(T* value) const {
        for (Object* check : *this) {
            if (*check == value) {
                return check;
            }
        }
        return NULL;
    }
};


#define RightLinkable RLinkable
#define RightLinkableQueue RLinkableQueue


#endif // _RIGHT_LINKABLE_








#if !defined(_NO_TWO_LINKABLE_) || !defined(_NO_TWO_LINKABLE_EXPAND_)

#include <string>


template<typename T>
class NumberTLinkable : public TLinkable {
public:
    T number;

    NumberTLinkable(T _number = 0) {
        number = _number;
    }

    operator T() const {
        return number;
    }
};

typedef NumberTLinkable<int> IntTLinkable;
typedef NumberTLinkable<long> LongTLinkable;
typedef NumberTLinkable<float> FloatTLinkable;
typedef NumberTLinkable<double> DoubleTLinkable;



class StringTLinkable : public TLinkable, public std::string {
public:
    StringTLinkable(std::string s) : std::string(s) {}
    StringTLinkable(const char* s) : std::string(s) {}
};


#endif //_TWO_LINKABLE_EXPAND_




#if !defined(_NO_RIGHT_LINKABLE_) || !defined(_NO_RIGHT_LINKABLE_EXPAND_)

template<typename T>
class NumberRLinkable : public RLinkable {
public:
    T number;

    NumberRLinkable(T _number = 0) {
        number = _number;
    }

    operator T() const {
        return number;
    }
};

typedef NumberRLinkable<int> IntRLinkable;
typedef NumberRLinkable<long> LongRLinkable;
typedef NumberRLinkable<float> FloatRLinkable;
typedef NumberRLinkable<double> DoubleRLinkable;



class StringRLinkable : public RLinkable, public std::string {
public:
    StringRLinkable(std::string s) : std::string(s) {}
    StringRLinkable(const char* s) : std::string(s) {}
};



#endif // _RIGHT_LINKABLE_EXPAND_


#endif // _LINKABLE_HPP_
