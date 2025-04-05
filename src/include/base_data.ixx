export module base_data_type;

import base_data_impl;

export namespace dast {
	namespace rt {
		template <typename data_t>
		using base_data_t = base_data_impl<data_t>;

		using base_view_t = base_view_impl;

		template <typename data_t>
		using const_base_data_t = const_base_data_impl<data_t>;
	}
}