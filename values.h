#include "object.h"
#include "val_types.h"

#ifndef VALUES_H
#define VALUES_H

struct Value {
    ValueType type;
    union {
        int intValue;
        bool boolValue;
        Obj* objPtr;
    };

    static Value fromInt(int v) {
        Value val;
        val.type = ValueType::INT;
        val.intValue = v;
        return val;
    }

    static Value fromBool(bool b) {
        Value val;
        val.type = ValueType::BOOL;
        val.boolValue = b;
        return val;
    }

    static Value fromObj(Obj* o) {
        Value val;
        val.type = ValueType::OBJ_PTR;
        val.objPtr = o;
        return val;
    }
};

#endif //VALUES_H