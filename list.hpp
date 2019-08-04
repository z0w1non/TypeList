#include <iostream>
#include <type_traits>

#define template_args(...) template<template<typename ...> typename list, __VA_ARGS__, typename ... types>

// declaration of constant types
template<typename type, type value>
using integral_constant = std::integral_constant<type, value>;
template<bool boolean> using bool_constant = std::bool_constant<boolean>;
using false_type = std::false_type;
using true_type = std::true_type;

#define declare_fundamental_consntant_type(fundamental_type, alias) \
    template<fundamental_type value> \
    using alias = integral_constant<fundamental_type, value>;

    declare_fundamental_consntant_type(bool,               bool_              )
    declare_fundamental_consntant_type(char,               char_              )
    declare_fundamental_consntant_type(signed char,        signed_char_       )
    declare_fundamental_consntant_type(unsigned char,      unsigned_char_     )
    declare_fundamental_consntant_type(short,              short_             )
    declare_fundamental_consntant_type(unsigned short,     unsigned_short_    )
    declare_fundamental_consntant_type(int,                int_               )
    declare_fundamental_consntant_type(unsigned int,       unsigned_int_      )
    declare_fundamental_consntant_type(long,               long_              )
    declare_fundamental_consntant_type(unsigned long,      unsigned_long_     )
    declare_fundamental_consntant_type(long long,          long_long_         )
    declare_fundamental_consntant_type(unsigned long long, unsigned_long_long_)
    declare_fundamental_consntant_type(std::size_t,        size_t_            )
    declare_fundamental_consntant_type(std::ptrdiff_t,     ptrdiff_t_         )
#undef declare_fundamental_consntant_type

// operator type implementation
#define declare_operator_type(alias, op) \
    template<typename a, typename b> \
    struct alias##_impl { \
        using type = integral_constant<typename a::value_type, a::value op b::value>; \
    }; \
    template<typename a, typename b> \
    using alias = typename alias##_impl<a, b>::type;

    declare_operator_type(plus,       +)
    declare_operator_type(minus,      -)
    declare_operator_type(multiplies, *)
    declare_operator_type(divides,    /)
    declare_operator_type(modulus,    %)
#undef declare_operator_type

// front implementation
template<typename list>
struct front_impl;
template<template<typename ...> typename list, typename front, typename ... types>
struct front_impl<list<front, types ...>> {
    using type = front;
};
template<typename list>
using front = typename front_impl<list>::type;

// push_front implementation
template<typename list, typename front>
struct push_front_impl;
template<template<typename ...> typename list, typename front, typename ... types>
struct push_front_impl<list<types ...>, front> {
    using type = list<front, types ...>;
};
template<typename list, typename front>
using push_front = typename push_front_impl<list, front>::type;

// pop_front implementation
template<typename list>
struct pop_front_impl;
template<template<typename ...> typename list, typename front, typename ... types>
struct pop_front_impl<list<front, types ...>> {
    using type = list<types ...>;
};
template<typename list>
using pop_front = typename pop_front_impl<list>::type;

// push_back implementation
template<typename list, typename back>
struct push_back_impl;
template<template<typename ...> typename list, typename back, typename ... types>
struct push_back_impl<list<types ...>, back> {
    using type = list<types ..., back>;
};
template<typename list, typename back>
using push_back = typename push_back_impl<list, back>::type;

// size implementation
template<typename list>
struct size_impl;
template<template<typename ...> typename list, typename ... types>
struct size_impl<list<types ...>> {
    using type = std::integral_constant<std::size_t, sizeof ... (types)>;

};
template<typename list>
using size = typename size_impl<list>::type;

// at implementation
template<typename list, std::size_t>
struct at_impl;
template<template<typename ...> typename list, typename front, typename ... types>
struct at_impl<list<front, types ...>, 0> {
    using type = front;
};
template<template<typename ...> typename list, std::size_t index, typename front, typename ... types>
struct at_impl<list<front, types ...>, index> {
    using type = typename at_impl<list<types ...>, index - 1>::type;
};
template<typename list, std::size_t index>
using at = typename at_impl<list, index>::type;

// back implementation
template<typename list>
struct back_impl;
template<template<typename ...> typename list, typename ... types>
struct back_impl<list<types ...>> {
    using type = typename at<list<types ...>, size<list<types ...>>::value - 1>;
};
template<typename list>
using back = typename back_impl<list>::type;

// pop_back implementation
template<typename list>
struct pop_back_impl;
template<typename list>
struct pop_back_impl_recurse;
template<template<typename ...> typename list, typename prev, typename back>
struct pop_back_impl_recurse<list<prev, back>> {
    using type = back;
};
template<template<typename ...> typename list, typename ... types>
struct pop_back_impl_recurse<list<types ...>> {
    using type = pop_back_impl_recurse<pop_front<list<types ...>>>;
};
template<template<typename ...> typename list, typename ... types>
struct pop_back_impl<list<types ...>> {
    using type = typename pop_back_impl_recurse<list<types ...>>::type;
};
template<typename list>
using pop_back = typename pop_back_impl<list>::type;

// map implementation
template<typename list, template<typename> typename func>
struct map_impl;
template<template<typename ...> typename list, template<typename> typename func, typename ... types>
struct map_impl<list<types ...>, func> {
    using type = list<typename func<types>::type ...>;
};
template<typename list, template<typename> typename func>
using map = typename map_impl<list, func>::type;

// size_of implementation
template<typename type>
struct size_of : public std::integral_constant<std::size_t, sizeof(type)> {};

// accumulate implementation
template<typename list, template<typename, typename> typename binop, typename accum, typename depth>
struct accumulate_impl_recurse {
    using type = typename accumulate_impl_recurse
    <
        pop_front<list>,
        binop,
        binop<accum, front<list>>,
        minus<depth, size_t_<1>>
    >::type;
};
template<typename list, template<typename, typename> typename binop, typename accum>
struct accumulate_impl_recurse<list, binop, accum, size_t_<0>> {
    using type = binop<accum, front<list>>;
};
template<typename list, typename init, template<typename, typename> typename binop>
struct accumulate_impl {
    using type = typename accumulate_impl_recurse<
        list,
        binop,
        init,
        minus<size<list>, size_t_<1>>
    >::type;
};
template<typename list, typename init, template<typename, typename> typename binop>
using accumulate = typename accumulate_impl<list, init, binop>::type;

template<typename T>
struct symbol {
    using type = T;
};

// if_ implementation
template<typename boolean, typename true_case, typename false_case>
struct if_impl {};
template<typename true_case, typename false_case>
struct if_impl<true_type, true_case, false_case> {
    using type = true_case;
};
template<typename true_case, typename false_case>
struct if_impl<false_type, true_case, false_case> {
    using type = false_case;
};
template<typename cond, typename true_case, typename false_case>
using if_ = typename if_impl<cond, true_case, false_case>::type;

//greater implementation
template<typename a, typename b, bool boolean = (a::value > b::value)>
struct greater_impl;
template<typename a, typename b>
struct greater_impl<a, b, true> : true_type {};
template<typename a, typename b>
struct greater_impl<a, b, false> : false_type {};
template<typename a, typename b>
using greater = typename greater_impl<a, b>::type;

// max implementation
template<typename a, typename b>
struct max_impl {
    using type = if_<greater<a, b>, a, b>;
};
template<typename a, typename b>
using max = typename max_impl<a, b>::type;

//list implementation
template<typename ... types>
struct list {
    using type = list<types ...>;

                                                   using size       = ::size      <type       >;
                                                   using pop_front  = ::pop_front <type       >;
                                                   using pop_back   = ::pop_back  <type       >;
                                                   using front      = ::front     <type       >;
                                                   using back       = ::back      <type       >;
    template<                   typename    back > using push_back  = ::push_back <type, back >;
    template<                   typename    front> using push_front = ::push_front<type, front>;
    template<                   std::size_t index> using at         = ::at        <type, index>;
    template<template<typename> typename    func > using map        = ::map       <type, func >;

    template<typename init, template<typename, typename> typename binop>
    using accumulate = ::accumulate<type, init, binop>;
};

#define declare_tag_type(type_name) struct type_name {};
    declare_tag_type(a)
    declare_tag_type(b)
    declare_tag_type(c)
#undef declare_tag_type

int main()
{
    static_assert(std::is_same_v<list<a>::push_back<b>::push_back<c>, list<a, b, c>>, "");
    static_assert(std::is_same_v<list<a, b, c>::at<0>, a>, "");
    static_assert(std::is_same_v<list<a, b, c>::at<1>, b>, "");
    static_assert(std::is_same_v<list<a, b, c>::at<2>, c>, "");
    static_assert(std::is_same_v<list<a, b, c>::front, a>, "");
    static_assert(std::is_same_v<list<a, b, c>::back, c>, "");
    static_assert(std::is_same_v<list<char, short, int>::map<size_of>::at<1>, size_t_<sizeof(short)>>, "");
    static_assert(std::is_same_v<if_<true_type, a, b>, a>, "");
    static_assert(std::is_same_v<max<int_<1>, int_<2>>, int_<2>>, "");
    static_assert(std::is_same_v<plus<int_<0xff00>, int_<0x00ff>>, int_<0xffff>>, "");
    static_assert(std::is_same_v<minus<int_<0x10000>, int_<0x1>>, int_<0xffff>>, "");
    static_assert(std::is_same_v<multiplies<int_<9>, int_<9>>, int_<81>>, "");
    static_assert(std::is_same_v<divides<int_<1000>, int_<10>>, int_<100>>, "");
    static_assert(std::is_same_v<list<char, short, int>::map<size_of>::accumulate<size_t_<0>, max>, size_t_<sizeof(int)>>, "");
}
