export module unordered_init_list;

import unorder_init_list_impl;

import <initializer_list>;

export namespace dacm {

	namespace fast {
		template <class _Elem>
		const _Elem& get_list_elem(const std::initializer_list<_Elem> list, size_t sub) {
			return *(list.begin() + sub);
		}
	}

	namespace intact {
		template <class _Elem>
		const _Elem& get_list_elem(const std::initializer_list<_Elem> list, size_t sub) {
			if (list.begin() == nullptr)
				return (_Elem&)(0);
			return *(list.begin() + sub);
		}
	}

	namespace safe {
		template <class _Elem>
		const _Elem& get_list_elem(const std::initializer_list<_Elem> list, size_t sub) {
			runtime_check(list.begin() == nullptr, "List is null");
			return *(list.begin() + sub);
		}
	}

}

export namespace dast {

	namespace compile {

		template <class Type>
		using uninit_list = uninit_list_impl<Type>;

	}
}