#include "InnerField.hpp"

InnerField::InnerField(const FieldInfo* info) {
    fieldData = (FieldInfo*)info;
    isStatic = (info->type->attrs & 0x0010) == 0x0010;
}

InnerField::InnerField(const InnerField& oth) {
    fieldData = oth.fieldData;
    isStatic = oth.isStatic;
    instance = oth.instance;
}

template <typename TYPE>
InnerField::InnerField(const Field<TYPE> oth) {
    fieldData = oth.fieldData;
    isStatic = oth.isStatic;
    instance = oth.instance;
}