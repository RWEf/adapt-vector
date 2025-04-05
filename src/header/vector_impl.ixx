export module vector_impl;

import instruction;
import type_frame;

import <initializer_list>;

import unordered_init_list;

import alloc_manager;
import base_data_type;

export template <
	class VecType,
	float ExpansionFactor,
	class AllocManagerType = base_alloc_manager_impl<VecType>
>
class base_vector_impl : private dast::rt::base_view_t {

private:

	using vec_type = VecType;
	using alloc_manager_t = AllocManagerType;
	using base_t = dast::rt::base_view_t;

private:

	static_assert(
		assert_result<std::is_function_v<vec_type>>(),
		"Can't allocate function type"
		);

	static_assert(
		assert_result<std::is_reference_v<vec_type>>(),
		"Can't allocate reference type"
		);

private:

	alloc_manager_t manager;

public:

	constexpr base_vector_impl() : base_t(0) {
		manager.allocate(1);
	}

	constexpr base_vector_impl(const vec_type& val) : base_t(0) {
		manager.allocate(1);
		manager.address[0] = val;
		this->size = 1;
	}

	constexpr base_vector_impl(size_t size) : base_t(0) {
		manager.allocate(size);
	}

	constexpr base_vector_impl(const base_vector_impl& entry) {
		if (entry.manager.address == nullptr)
			return;

		manager.allocate(entry.base_t::size);
		for (size_t i = 0; i < entry.base_t::size; i++) {
			manager.address[i] = entry.manager.address[i];
		}

		this->size = entry.base_t::size;
	}

	constexpr base_vector_impl(base_vector_impl&& entry) {
		entry.manager.move(manager);
		this->size = entry.base_t::size;
	}

	constexpr base_vector_impl(const dast::compile::uninit_list<vec_type>& pars) {
		this->size = pars.size();
		manager.allocate(this->size * ExpansionFactor);
		for (size_t i = 0; i < this->size; i++) {
			manager.address[i] = pars[i];
		}
	}

	template <size_t len>
	constexpr base_vector_impl(vec_type(&entry)[len]) {
		this->size = len;
		manager.allocate(len * ExpansionFactor);
		for (size_t i = 0; i < this->size; i++) {
			manager.address[i] = entry[i];
		}
	};

	constexpr void resize(size_t length) {
		if (length < this->size) {
			this->size = length;
			return;
		}

		if (manager.address == nullptr) {
			manager.allocate(length);
			this->size = 0;
			return;
		}

		manager.reallocate(length);
	}

public:

	template <size_t len>
	constexpr void multiple_push_back(vec_type(&vals)[len]) {
		for (size_t i = 0; i < len; i++) {
			push_forward(vals[i]);
		}
	}

	constexpr void multiple_push_forward(const dast::compile::uninit_list<vec_type>& list) {
		size_t size = list.size();
		for (int i = size - 1; i != -1; i--) {
			push_forward(list[i]);
		}
	}

public:

	constexpr void push_forward(vec_type element) {
		if (this->size >= manager.length) {
			this->resize(this->size * ExpansionFactor);
		}
		for (size_t i = this->size + 1; i > 0; i--) {
			manager.address[i] = manager.address[i - 1];
		}
		manager.address[0] = element;
		this->size += 1;
	}

	constexpr void push_back(const vec_type& element) {
		if (this->size >= manager.length) {
			this->resize(this->size * ExpansionFactor);
		}
		manager.address[this->size] = element;
		this->size += 1;
	}

	constexpr void push_back(vec_type&& element) {
		if (this->size >= manager.length) {
			this->resize(this->size * ExpansionFactor);
		}
		manager.address[this->size] = std::forward<vec_type&&>(element);
		this->size += 1;
	}

	constexpr void pop_front() noexcept {
		if (this->size == 0)
			return;
		for (size_t i = 0; i < this->size - 1; i++) {
			manager.address[i] = manager.address[i + 1];
		}
		this->size -= 1;
	}

	constexpr void pop_back() noexcept {
		if (this->size == 0)
			return;
		this->size -= 1;
	}

public:

	constexpr vec_type& at(size_t sub) const {
		if (sub > this->size)
			throw "Pos error";
		if (manager.address == nullptr)
			throw "Data is nullptr";
		return manager.address[sub];
	}

	constexpr bool is_empty() const noexcept {
		return !this->size;
	}

public:

	constexpr void reset() const { delete[] manager.address; manager.address = nullptr; this->size = 0; manager.length = 0; }

	constexpr size_t length() const noexcept { return this->size; }

	constexpr size_t space_len() const noexcept { return manager.length; }

	constexpr vec_type* data() const noexcept { return manager.address; }

	constexpr vec_type* begin() const noexcept { return manager.address; }

	constexpr vec_type* end() const noexcept { return manager.address + this->size; }

public:

	constexpr vec_type& operator[](size_t sub) const { return manager.address[sub]; }

	constexpr vec_type& operator>>(vec_type val) {
		if (manager.address == nullptr)
			throw "Data is nullptr!";
		push_forward(val);
		return manager.address[0];
	}

	constexpr vec_type& operator<<(vec_type val) {
		if (manager.address == nullptr) throw "Data is nullptr!";
		push_back(val);
		return manager.address[this->size - 1];
	}

public:

	constexpr void operator=(std::nullptr_t) { manager.free_allocate(); }

	constexpr base_vector_impl& operator=(base_vector_impl&& entry) noexcept {
		if (entry.manager.address == nullptr)
			throw "Data is nullptr!";

		manager.move(std::move(entry.manager));
		return *this;
	}

	constexpr base_vector_impl& operator=(const base_vector_impl& entry) {
		if (entry.manager.address == nullptr)
			throw "Data is nullptr!";

		manager.allocate(entry.manager.length);
		for (size_t i = 0; i < entry.base_t::size; i++) {
			manager.address[i] = entry.manager.address[i];
		}
		this->size = entry.base_t::size;
		return *this;
	}

public:

	constexpr base_vector_impl& operator-=(int number) noexcept { for (size_t i = 0; i < this->size; i++) { manager.address[i] = manager.address[i] - number; }; return *this; }

	constexpr base_vector_impl& operator+=(int number) noexcept { for (size_t i = 0; i < this->size; i++) { manager.address[i] = manager.address[i] + number; }; return *this; }

	constexpr base_vector_impl& operator*=(int number) noexcept { for (size_t i = 0; i < this->size; i++) { manager.address[i] = manager.address[i] * number; }; return *this; }

	constexpr base_vector_impl& operator/=(int number) noexcept { for (size_t i = 0; i < this->size; i++) { manager.address[i] = manager.address[i] / number; }; return *this; }
};

export template <class AllocTy, float ExpansionFactor, bool IsBaseType>
class fast_vector_impl : deta_manager<AllocTy, IsBaseType> {

private:

	static_assert(
		assert_result<std::is_function_v<AllocTy>>(),
		"Can't allocate function type"
		);

	static_assert(
		assert_result<std::is_reference_v<AllocTy>>(),
		"Can't allocate reference type"
		);

private:

	using value_t = AllocTy;
	using reference_t = value_t&;
	using pointer_t = value_t*;

	using manager_t = deta_manager<AllocTy, true>;

private:

	manager_t manager;

public:

	explicit constexpr fast_vector_impl() {
		manager.allocate(1);
	};

	constexpr fast_vector_impl(size_t size) {
		manager.allocate(size);
	}

	constexpr fast_vector_impl(const value_t& value) {
		manager.allocate(1);
		manager.construct_next(std::forward<const value_t&&>(value));
	}

	template <size_t size>
	constexpr fast_vector_impl(value_t(&array)[size]) {
		manager.allocate(size * ExpansionFactor);
		for (size_t i = 0; i < size; i++) {
			manager.construct_next(std::forward<const value_t&&>(array[i]));
		}
		return;
	}

	constexpr fast_vector_impl(const dast::compile::uninit_list<value_t>& list) {
		size_t list_size = list.size();
		manager.allocate(list_size * ExpansionFactor);
		for (size_t i = 0; i < list_size; i++) {
			manager.construct_next(std::forward<const value_t&&>(list[i]));
		}
		return;
	}

	constexpr void push_back(const value_t&& value) {
		if (manager.length() + 1 > manager.max_length())
			manager.resize(manager.max_length() * ExpansionFactor);
		manager.construct_next(std::forward<const value_t&&>(value));
	}

	constexpr void pop_back() noexcept {
		manager.remove_end();
	}

	constexpr size_t length() const noexcept { return manager.length(); }

	constexpr const pointer_t begin() const noexcept {
		return &manager;
	}

	constexpr const pointer_t end() const noexcept {
		return &manager + manager.length();
	}

	constexpr fast_vector_impl& operator=(fast_vector_impl&& other) {
		other.manager.move(manager);
		return *this;
	}
};

export template <class AllocTy, size_t BufferSize, float ExpansionFactor, bool IsBaseType>
class buffer_fast_vector_impl {};

export template <class AllocTy, size_t BufferSize, float ExpansionFactor>
class buffer_fast_vector_impl<AllocTy, BufferSize, ExpansionFactor, true> : deta_manager<AllocTy, true> {

private:

	static_assert(
		assert_result<std::is_function_v<AllocTy>>(),
		"Can't allocate function type"
		);

	static_assert(
		assert_result<std::is_reference_v<AllocTy>>(),
		"Can't allocate reference type"
		);

	static_assert(
		assert_result<(BufferSize == 0 || BufferSize < 0)>(),
		"Can't allocate reference type"
		);

private:

	using value_t = AllocTy;
	using reference_t = value_t&;
	using pointer_t = value_t*;

	using manager_t = deta_manager<AllocTy, true>;

private:

	struct {
		value_t data[BufferSize]; size_t size = 0;
		constexpr static size_t max_size = BufferSize - 1;
		constexpr const pointer_t converted() const noexcept {
			return (pointer_t)data;
		}
	} buffer;

	manager_t manager;

public:

	explicit constexpr buffer_fast_vector_impl() noexcept {};

	constexpr buffer_fast_vector_impl(size_t size) {
		if (size > buffer.max_size)
			manager.allocate(size);
	}

	constexpr buffer_fast_vector_impl(const value_t& value) {
		buffer.data[0] = std::forward<const value_t&&>(value);
		buffer.size = 1;
	}

	template <size_t size>
	constexpr buffer_fast_vector_impl(value_t(&array)[size]) {
		if (size > buffer.max_size) {
			manager.allocate(size * ExpansionFactor);
			for (size_t i = 0; i < size; i++) {
				manager.construct_next(std::forward<value_t&&>(array[i]));
			}
			return;
		}
		for (size_t i = 0; i < size; i++) {
			buffer.data[i] = std::forward<value_t&&>(array[i]);
		}
		buffer.size = size;
	}

	constexpr buffer_fast_vector_impl(const dast::compile::uninit_list<value_t>& list) {
		size_t list_size = list.size();
		if (list_size > buffer.max_size) {
			manager.allocate(list_size * ExpansionFactor);
			for (size_t i = 0; i < list_size; i++) {
				manager.construct_next(std::forward<const value_t&&>(list[i]));
			}
			return;
		}
		for (size_t i = 0; i < list_size; i++) {
			buffer.data[i] = std::forward<const value_t&&>(list[i]);
		}
		buffer.size = list_size;
	}

	constexpr void push_back(const value_t& value) {
		if (manager.length()) {
			if (manager.length() + 1 > manager.max_length())
				manager.resize(manager.max_length() * ExpansionFactor);
			manager.construct_next(std::forward<const value_t&&>(value));
			return;
		}
		if (buffer.size + 1 > buffer.max_size) {
			manager.allocate(buffer.size * ExpansionFactor);
			for (size_t i = 0; i < buffer.size; i++) {
				manager.construct_next(std::forward<const value_t&&>(buffer.data[i]));
			}
			manager.construct_next(std::forward<const value_t&&>(value));
			return;
		}
		buffer.data[buffer.size] = std::forward<const value_t&&>(value);
		buffer.size += 1;
	}

	constexpr void pop_back() noexcept {
		manager.remove_end();
	}

	constexpr const pointer_t begin() const noexcept {
		if (manager.length())
			return &manager;
		else
			return buffer.converted();
	}

	constexpr const pointer_t end() const noexcept {
		if (manager.length())
			return &manager + manager.length();
		else
			return buffer.converted() + buffer.size;
	}
};

export template <class AllocTy, size_t BufferSize, float ExpansionFactor>
class buffer_fast_vector_impl<AllocTy, BufferSize, ExpansionFactor, false> : deta_manager<AllocTy, false> {

private:

	static_assert(
		assert_result<std::is_function_v<AllocTy>>(),
		"Can't allocate function type"
		);

	static_assert(
		assert_result<std::is_reference_v<AllocTy>>(),
		"Can't allocate reference type"
		);

	static_assert(
		assert_result<(BufferSize == 0 || BufferSize < 0)>(),
		"Can't allocate reference type"
		);

private:

	using value_t = AllocTy;
	using reference_t = value_t&;
	using pointer_t = value_t*;

	using manager_t = deta_manager<AllocTy, true>;
	constexpr static size_t type_size = sizeof(AllocTy);

private:

	struct {
		char data[BufferSize * type_size]; size_t size = 0;
		constexpr static size_t max_size = BufferSize - 1;
		template <class... ArgsType>
		constexpr void construct_next(const ArgsType&&... args) noexcept {
			void(
				new ((void*)((pointer_t)data + size))
				value_t(args...)
				);
			size += 1;
		}
		constexpr const pointer_t converted() const noexcept {
			return (pointer_t)data;
		}
	} buffer;

	manager_t manager;

public:

	explicit constexpr buffer_fast_vector_impl() noexcept {};

	constexpr buffer_fast_vector_impl(size_t size) {
		if (size > buffer.max_size)
			manager.allocate(size);
	}

	template <size_t size>
	constexpr buffer_fast_vector_impl(value_t(&array)[size]) {
		if (size > buffer.max_size) {
			manager.allocate(size * ExpansionFactor);
			for (size_t i = 0; i < size; i++) {
				manager.construct_next(std::forward<value_t&&>(array[i]));
			}
			return;
		}
		for (size_t i = 0; i < size; i++) {
			buffer.construct_next(std::forward<value_t&&>(array[i]));
		}
	}

	constexpr buffer_fast_vector_impl(const value_t& value) {
		buffer.construct_next(std::forward<const value_t&&>(value));
	}

	constexpr buffer_fast_vector_impl(const dast::compile::uninit_list<value_t>& list) {
		size_t list_size = list.size();
		if (list_size > buffer.max_size) {
			manager.allocate(list_size * ExpansionFactor);
			for (size_t i = 0; i < list_size; i++) {
				manager.construct_next(std::forward<const value_t&&>(list[i]));
			}
			return;
		}
		for (size_t i = 0; i < list_size; i++) {
			buffer.construct_next(std::forward<const value_t&&>(list[i]));
		}
	}

	constexpr void push_back(const value_t& value) {
		if (manager.length()) {
			if (manager.length() + 1 > manager.max_length())
				manager.resize(manager.max_length() * ExpansionFactor);
			manager.construct_next(std::forward<const value_t&&>(value));
			return;
		}
		if (buffer.size + 1 > buffer.max_size) {
			manager.allocate(buffer.size * ExpansionFactor);
			for (size_t i = 0; i < buffer.size; i++) {
				manager.construct_next(
					std::forward<const value_t&&>(
						*((pointer_t)buffer.data + i)
					)
				);
			}
			manager.construct_next(
				std::forward<const value_t&&>(value)
			);
			return;
		}
		buffer.construct_next(buffer.size, std::forward<const value_t&&>(value));
		buffer.size += 1;
	}

	constexpr void pop_back() noexcept {
		manager.remove_end();
	}

	constexpr const pointer_t begin() const noexcept {
		if (manager.length())
			return &manager;
		else
			return buffer.converted();
	}

	constexpr const pointer_t end() const noexcept {
		if (manager.length())
			return &manager + manager.length();
		else
			return buffer.converted() + buffer.size;
	}
};