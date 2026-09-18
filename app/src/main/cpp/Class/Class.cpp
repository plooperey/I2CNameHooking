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
    Logger::LogDebug("Creating class with namespace '" + std::string(classNamespace) + "' and name '" + std::string(className) + '\'');
    auto domain = il2cppMethods.il2cpp_domain_get();
    Logger::LogDebug("Got domain.");
    size_t assembliesCount;
    auto assemblies = il2cppMethods.il2cpp_domain_get_assemblies(domain, &assembliesCount);
    Logger::LogDebug("Assemblies count: " + std::to_string(assembliesCount));
    for (size_t as = 0; as < assembliesCount; as++) {
        Il2CppImage* image = assemblies[as]->image;
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