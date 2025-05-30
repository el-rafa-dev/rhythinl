#include <string>
#ifndef OBJECT_H
#define OBJECT_H

enum class ObjType {
    STRING
};

struct Obj {
    ObjType type;
    Obj* next; 
};

struct ObjString : Obj {
    std::string value;

    ObjString(const std::string& str) : value(str) {
        type = ObjType::STRING;
        next = nullptr;
    }
};

#endif //OBJECT_H