export module vector;

import vector_impl;

import instruction;
import <initializer_list>;

export namespace dast {
	namespace rt {
		template <class Type>
		struct fast_vector : fast_vector_impl<Type, 2.0f, std::is_base_type_v<Type>> {
			using vector_t = fast_vector_impl<Type, 2.0f, std::is_base_type_v<Type>>;
			template <class... ArgsType>
			constexpr fast_vector(ArgsType&&... args) : vector_t(args...) {};
			explicit constexpr fast_vector() noexcept {};
			constexpr fast_vector(const std::initializer_list<Type>& list) : vector_t(list) {};
		};

		template <class Type>
		struct buffer_fast_vector : buffer_fast_vector_impl<Type, 11, 2.0f, std::is_base_type_v<Type>> {
			using vector_t = buffer_fast_vector_impl<Type, 11, 2.0f, std::is_base_type_v<Type>>;
			template <class... ArgsType>
			constexpr buffer_fast_vector(ArgsType&&... args) : vector_t(args...) {};
			explicit constexpr buffer_fast_vector() noexcept {};
			constexpr buffer_fast_vector(const std::initializer_list<Type>& list) : vector_t(list) {};
		};

		template <class Type, bool IsBaseType = true>
		struct vector : base_vector_impl<Type, 2.0f> {
			using vector_t = base_vector_impl<Type, 2.0f>;
			template <class... ArgsType>
			constexpr vector(ArgsType&&... args) : vector_t(args...) {};
			explicit constexpr vector() noexcept {};
			constexpr vector(const std::initializer_list<Type>& list) : vector_t(list) {};
		};

		template <class Type>
		struct vector<Type, false> : fast_vector<Type> {
			using vector_t = fast_vector<Type>;
			template <class... ArgsType>
			constexpr vector(ArgsType&&... args) : vector_t(args...) {};
			explicit constexpr vector() noexcept {};
			constexpr vector(const std::initializer_list<Type>& list) : vector_t(list) {};
		};

		template <class _First, class... _Second>
		buffer_fast_vector(_First, _Second...) -> buffer_fast_vector<
			std::enable_if_t<std::disjunction_v<std::is_same<_First, _Second>...>, _First>
		>;

		template <class _First, class... _Second>
		fast_vector(_First, _Second...) -> fast_vector<
			std::enable_if_t<std::disjunction_v<std::is_same<_First, _Second>...>, _First>
		>;

		template <class _First, class... _Second>
		vector(_First, _Second...) -> vector<
			std::enable_if_t<std::disjunction_v<std::is_same<_First, _Second>...>, _First>
		>;
	};
}