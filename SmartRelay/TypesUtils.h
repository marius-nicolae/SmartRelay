#pragma once
namespace NsTypesUtils {

//remove the reference of a type reference
template<typename T>
struct RemoveReference {
    using Type = T;
};

template<typename T>
struct RemoveReference<T&> {
    using Type = T;
};

using uint = unsigned int;
using ulong = unsigned long;
}
