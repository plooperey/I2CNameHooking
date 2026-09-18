#include "Class.hpp"
#include "../Il2CppMethods/Il2CppMethods.hpp"

Class::Class(const Il2CppObject* object) {
    if (!object) {
        Logger::LogWarning("Creating class with null Il2CppObject!");
        return;
    }
    classData = object->klass;
}

Class::Class(const Il2CppClass* clazz) {
    if (!clazz) {
        Logger::LogWarning("Creating class with null Il2CppClass!");
        return;
    }
    classData = (Il2CppClass*)clazz;
}

Class::Class(const char* classNamespace, const char* className) {
    Logger::LogDebug("Find class with namespace '" + std::string(classNamespace) + "' and class name '" + std::string(className) + "'");
    std::vector<Il2CppAssembly*>* assemblies{};
    if (il2cppMethods.Assembly$$GetAllAssemblies) assemblies = il2cppMethods.Assembly$$GetAllAssemblies();
    else {
        Logger::LogError("GetAllAssemblies is null!");
        return;
    }
    if (assemblies->empty()) {
        Logger::LogError("Assemblies is null!");
        return;
    }
    for (auto assembly : *assemblies) {
        Il2CppImage* image = assembly->image;
        Logger::LogDebug(assembly->aname.name);
        Logger::LogDebug(image->name);
        size_t count = image->typeCount;
        for (size_t i = 0; i < count; ++i) {
            auto clazz = il2cppMethods.il2cpp_image_get_class(image, i);
            Logger::LogDebug("Class namespace '" + std::string(clazz->namespaze) + "' class name '" + std::string(clazz->name) + '\'');
            if (clazz->namespaze == classNamespace && clazz->name == className) classData = clazz;
        }
    }
}

void Class::TryInit() const {
    if (classData) il2cppMethods.Class$$Init(classData);
}

InnerMethod Class::GetMethod(const std::string& methodName) const {
    Class::TryInit();
    for (uint32_t i = 0; i < classData->method_count; i++) {
        const MethodInfo* method = classData->methods[i];
        if (method->name == methodName) return method;
    }
    return {};
}

InnerMethod Class::GetMethod(const std::string& methodName, int paramsCount) const {
    Class::TryInit();
    for (uint32_t i = 0; i < classData->method_count; i++) {
        auto method = classData->methods[i];
        if (method->name == methodName && method->parameters_count == paramsCount) return method;
    }
    return {};
}

bool Class::IsValid() const {
    Class::TryInit();
    return classData;
}