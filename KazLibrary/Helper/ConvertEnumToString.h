#include <iostream>
#include <string>
#include <utility>
#include <array>

//https://kenkyu-note.hatenablog.com/entry/2020/07/06/195821
template <size_t N>
struct StaticString
{
    StaticString(const char *c)
        : StaticString(c, std::make_index_sequence<N>())
    {}

    std::string str() const { return std::string(mStr); }

private:
    template <size_t ...Indices>
    StaticString(const char *c, std::index_sequence<Indices...>)
        : mStr{ c[Indices]..., '\0' }
    {}

    char mStr[N + 1];
};

constexpr size_t FindChar(const char *str, char f)
{
    size_t res = 0;
    while (*str != f && *str != '\0') ++res, ++str;
    return res;
}

template <class M, M m>
constexpr auto EnumName()
{
#ifdef _MSC_VER
    constexpr char name[] = { __FUNCSIG__ };
    constexpr size_t s = FindChar(name, ',') + 1;
    constexpr size_t e = FindChar(name, '>');
#else
    constexpr const char *name_ = __PRETTY_FUNCTION__;
    constexpr const char *name = name_ + FindChar(name_, '=') + 1;
    constexpr size_t s = FindChar(name, '=') + 2;
    constexpr size_t e = FindChar(name, ']');
#endif
    constexpr size_t N = e - s;
    return StaticString<N>(name + s);
}
template <class Enum, class Indices>
struct EnumNames;
template <class Enum, std::underlying_type_t<Enum> ...Indices>
struct EnumNames<Enum, std::integer_sequence<std::underlying_type_t<Enum>, Indices...>>
{
    static const std::array<std::string, sizeof...(Indices)> msNames;
};
template <class Enum, std::underlying_type_t<Enum> ...Indices>
const std::array<std::string, sizeof...(Indices)>
EnumNames<Enum, std::integer_sequence<std::underlying_type_t<Enum>, Indices...>>::msNames
= { EnumName<Enum, (Enum)(Indices)>().str()... };

template <class Enum, std::underlying_type_t<Enum> Max = 128>
std::string GetEnumName(Enum e)
{
    using UType = std::underlying_type_t<Enum>;
    static const auto &names = EnumNames<Enum, std::make_integer_sequence<UType, Max>>::msNames;
    return names[(UType)e];
}
