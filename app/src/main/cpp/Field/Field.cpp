#include "Field.hpp"
#include "../Il2CppMethods/Il2CppMethods.hpp"

template <typename TYPE>
TYPE Field<TYPE>::Get() {
    TYPE ret{};
    if (isStatic) il2cppMethods.il2cpp_field_static_get_value(fieldData, (void*)&ret);
    else il2cppMethods.il2cpp_field_get_value(instance, fieldData, (void*)ret);
    return ret;
}

template <typename TYPE>
TYPE Field<TYPE>::Set(TYPE data) {
    if (isStatic) {
        il2cppMethods.il2cpp_field_static_set_value(fieldData, &data);
    } else {
        if (std::is_pointer_v<TYPE>) {
            il2cppMethods.il2cpp_field_set_value(instance, fieldData, (void*)&data);
        } else {
            il2cppMethods.il2cpp_field_set_value(instance, fieldData, &data);
        }
    }
    return data;
}