#include "linkable.hpp"



int main(int argc, char** argv) {
    TLinkableList<StringTLinkable> mylist = {"hello world", "minecraft", "talesrunner"};

    
    for (StringTLinkable* value : mylist) {
        printf("number: %s\n", (*value + "!").c_str());
    }

    return 0;
}