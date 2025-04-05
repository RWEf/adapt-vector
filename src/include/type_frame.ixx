export module type_frame;

export import <type_traits>;

_EXPORT_STD _STD_BEGIN

_EXPORT_STD template <typename Type>
constexpr bool check_pars() { return true; };

_EXPORT_STD template <typename _Ty1, typename _Ty2, typename... _Pars>
constexpr bool check_pars() {
    return is_same_v<_Ty1, _Ty2> && check_pars<_Ty2, _Pars...>();
}

_EXPORT_STD template <class Type>
constexpr bool is_character_v = _Is_any_of_v<remove_cv_t<Type>, char8_t, char16_t, char32_t, char, wchar_t>;

_EXPORT_STD template <class Type>
struct is_character : bool_constant<is_character_v<Type>> {};

_EXPORT_STD template <class Type>
constexpr bool is_base_type_v = _Is_any_of_v<
    remove_cv_t<Type>,
    char, signed char, unsigned char,
    char8_t, char16_t, char32_t, wchar_t,
    short, signed short, unsigned short,
    int, signed int, unsigned, int,
    long, unsigned long,
    long long, unsigned long long,
    float, double, long double,
    bool
>;

template <class ValType>
struct add_const {
    const ValType& v;
};

template <class ValType>
struct add_volatile {
    volatile ValType& v;
};

template <class ValType>
struct add_static {
    static ValType& v;
    add_static(ValType& r) : v(r) {};
};

template <class T>
add_const(T) -> add_const<T>;

template <class T>
add_volatile(T) -> add_volatile<T>;

_STD_END