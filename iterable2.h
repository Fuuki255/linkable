#ifndef _ITERABLE_C_H_
#define _ITERABLE_C_H_

#define ITERABLE2_VERSION {2, 0, 0}



#define IterableObject(OBJECT, PARENT) \
    OBJECT* prev##OBJECT, next##OBJECT;\
    ARRAY* parent##PARENT;

#define IterableObjectMethods(OBJECT) \




#define IterableArray(OBJECT, add) \
    OBJECT* first##OBJECT##add;\ 
    OBJECT* last##OBJECT##add;

#define IterableArrayMethods(ARRAY, OBJECT) \
    






#endif /* _ITERABLE_C_H_ */