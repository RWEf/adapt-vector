export module unorder_init_list_impl;

import instruction;

import <initializer_list>;

export template <class Type>
class uninit_list_impl : public std::initializer_list<Type> {

	using list_t = std::initializer_list<Type>;

	using value_t = Type;
	using pointer_t = Type*;
	using reference_t = Type&;

public:

	constexpr uninit_list_impl(const uninit_list_impl&) noexcept = default;
	constexpr uninit_list_impl(uninit_list_impl&&) noexcept = default;

	constexpr uninit_list_impl(const list_t& list) noexcept : list_t(std::move(list)) {}

	constexpr const pointer_t rbegin() const noexcept { return list_t::end(); }

	constexpr const pointer_t rend() const noexcept { return list_t::begin(); }

	constexpr const reference_t at(size_t sub) const noexcept {
		if (sub >= list_t::size())
			throw "The sub is out of range";
		return list_t::begin()[sub];
	}

	constexpr reference_t operator[](size_t sub) noexcept { return list_t::begin()[sub]; }

	constexpr const reference_t operator[](size_t sub) const noexcept { return (const reference_t)(list_t::begin()[sub]); }

	constexpr const pointer_t operator&() const noexcept { return list_t::begin(); }
};

template <class _First, class... _Second>
uninit_list_impl(_First, _Second...) -> uninit_list_impl<
	std::enable_if_t<std::disjunction_v<std::is_same<_First, _Second>...>, _First>
>;