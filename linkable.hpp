#ifndef _LINKABLE_HPP_
#define _LINKABLE_HPP_

#define LINKABLE_VERSION {2,0,0}

#include <exception>



struct DeletingLinkableException : public std::exception {
  const char* what() override const {
    return "not allowed to delete LLinkable or RLinkable in object side";
  }
};



template<typename T> class TLinkableList;


// TwoLinkable, a full linkable system to controll objects
template<typename T>
class TLinkable {
  TLinkable<T>* tlLeft;
  TLinkable<T>* tlRight;
  TLinkableList<T>* tlParent;
};


// LeftLinkable
template<typename T>
class LLinkable {
  LLinkable<T>* llLeft;
};


// RightLinkable
template<typename T>
class RLinkable {
  RLinkable<T>* rlRight;
};






// TwoLinkableList
template<typename T>
class TLinkableList {
  TLinkable<T>
};


// LeftLinkableList
template<typename T>
class LLinkableList {
  
};


// RightLinkableList
template<typename T>
class RLinkableList {
  
};










#endif // _LINKABLE_HPP_
