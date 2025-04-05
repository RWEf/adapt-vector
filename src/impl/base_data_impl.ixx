export module base_data_impl;

export template <typename DataType> struct base_data_impl {
	using data_t = DataType;

	data_t* data;
	size_t  size;

	constexpr data_t* begin() const noexcept { return data; }
	constexpr data_t* end() const noexcept { return data + size; }
};

export struct base_view_impl {
	size_t size;
};

export template <typename DataType> struct const_base_data_impl {
	using data_t = DataType;

	const data_t* data;
	size_t  size;

	constexpr data_t* begin() const noexcept { return data; }
	constexpr data_t* end() const noexcept { return data + size; }
};
