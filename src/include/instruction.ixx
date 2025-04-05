export module instruction;

export import type_frame;

export template <class Type>
concept char_t = std::is_character_v<Type> && !std::is_const_v<Type> && !std::is_pointer_v<Type> && !std::is_reference_v<Type>;

export template <class Type>
concept can_sub = requires(Type val) { val -= 1; };

export template <class Type>
concept can_compare = requires(Type val) { val == -1; };

export template <class Type>
concept restrictive_ref = std::is_reference_v<Type>;

export template <class Type>
concept restrictive_ptr = std::is_pointer_v<Type>;

export template <class Type>
concept restrictive_class = !std::is_class_v<Type>;

export template <typename First, typename Second, class... Pars_Ty>
concept restrictive_same = std::check_pars<First, Second, Pars_Ty...>();

export template <typename Type>
using remove_cvrp_t = std::remove_pointer_t<std::remove_reference_t<std::remove_cv_t<Type>>>;

export template <bool result>
constexpr bool assert_result() {
	return !(result);
}