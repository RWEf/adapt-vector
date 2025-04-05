export module alloc_manager_impl;

import instruction;

import <stdio.h>;

export template <typename AllocType>
class base_alloc_manager_impl {

private:

	using alloc_type = AllocType;

private:

	static_assert(
		assert_result<std::is_function_v<alloc_type>>(),
		"Can't allocate function type"
	);

	static_assert(
		assert_result<std::is_reference_v<alloc_type>>(),
		"Can't allocate reference type"
	);

public:

	alloc_type* address;

	size_t length;

public:

	constexpr base_alloc_manager_impl() noexcept 
		: address(nullptr), length(0) {}

	constexpr base_alloc_manager_impl(const base_alloc_manager_impl&)
		noexcept = delete;

	constexpr base_alloc_manager_impl(base_alloc_manager_impl&& entry) noexcept {
		address = entry.address; 
		length = entry.length; 
		entry.address = nullptr;
		entry.length = 0;
	};

	constexpr ~base_alloc_manager_impl() {
		free_allocate();
	}

	constexpr void allocate(size_t size) noexcept {
		if (size == 0 || size == -1)
			return;
		length = size;
		address = new alloc_type[size];
	}

	constexpr void allocate(size_t size, alloc_type init_value) noexcept {
		if (size == 0 || size == -1)
			return;
		length = size;
		address = new alloc_type[length] { init_value };
	}

	constexpr void reallocate(size_t size) noexcept {
		alloc_type* temp_ptr = address;
		size_t count = length > size ? size : length;
		allocate(size);
		for (size_t i = 0; i < count; i++) {
			address[i] = temp_ptr[i];
		}
		delete[] temp_ptr;
	}

	constexpr void move(base_alloc_manager_impl&& manager) noexcept {
		manager.address = address;
		manager.length = length;

		address = nullptr;
		length = 0;
	}

	constexpr void free_allocate() noexcept {
		delete[] address;
		address = nullptr;
		length = 0;
	}

	constexpr void free_allocate(alloc_type* entry) noexcept {
		delete[] entry;
		entry = nullptr;
	}

	constexpr alloc_type& operator[](size_t sub) const noexcept { return address[sub]; }

	constexpr alloc_type* begin() const noexcept { return address; }
	constexpr alloc_type* end() const noexcept { return address + length; }

	constexpr alloc_type& operator=(alloc_type&& entry) const noexcept {
		address = entry.address;
		length = entry.length;
		entry.address = nullptr;
		entry.length = 0;
		return *this;
	}
};

export template <class AllocTy, bool IsBaseType>
class detachable_manager_impl {};

export template <class AllocTy>
class detachable_manager_impl<AllocTy, true> {

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

private:

	pointer_t address;
	size_t size;
	size_t max_size;

	constexpr static size_t type_size = sizeof(value_t);
public:

	constexpr detachable_manager_impl() noexcept : address(nullptr), size(0), max_size(0) {}
	~detachable_manager_impl() { delete address; address = nullptr; }

	constexpr void allocate(size_t size) {
		max_size = size;
		address = (pointer_t)operator new(max_size * type_size);
	}

	constexpr void resize(size_t size) {
		if (this->size > size) {
			this->size = size;
			return;
		}
		reallocate(size);
	}

	constexpr void reallocate(size_t size) {
		pointer_t old_address = address;
		size_t count = this->size;
		allocate(size);
		for (size_t i = 0; i < count; i++) {
			address[i] = old_address[i];
		}
		delete old_address;
	}

	constexpr void construct_next(const value_t&& args) noexcept {
		if (size + 1 > max_size)
			return;
		address[size] = args;
		size += 1;
	}

	constexpr void move(detachable_manager_impl& other) {
		other.address = address;
		other.size = size;
		other.max_size = max_size;
		address = nullptr;
	}

	constexpr void remove_end() noexcept {
		if (!size)
			return;
		size -= 1;
	}

	constexpr const pointer_t begin() const noexcept {
		return address;
	}
	
	constexpr const pointer_t end() const noexcept {
		return address + size;
	}

	constexpr const size_t length() const noexcept {
		return size;
	}

	constexpr const size_t max_length() const noexcept {
		return max_size;
	}

	constexpr const pointer_t operator&() const noexcept {
		return address;
	}

	constexpr const pointer_t operator[](size_t space) const noexcept {
		if (space == -1 || space > size || space > max_size)
			return nullptr;
		return &address[space];
	}
};

export template <class AllocTy>
class detachable_manager_impl<AllocTy, false> {
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

private:

	pointer_t address;
	size_t size;
	size_t max_size;

	constexpr static size_t type_size = sizeof(value_t);
public:

	constexpr detachable_manager_impl() noexcept : address(nullptr), size(0), max_size(0) {}
	~detachable_manager_impl() {
		for (size_t i = 0; i < size; i++) {
			address[i].~value_t();
		}
		delete address;
		address = nullptr;
	}

	constexpr void allocate(size_t size) {
		max_size = size;
		address = (pointer_t)operator new(max_size * type_size);
	}

	constexpr void resize(size_t size) {
		if (this->size > size) {
			this->size = size;
			return;
		}
		reallocate(size);
	}

	constexpr void reallocate(size_t size) {
		pointer_t old_address = address;
		size_t count = this->size;
		allocate(size);
		for (size_t i = 0; i < count; i++) {
			void(new (&address[i]) value_t(old_address[i]));
		}
		delete old_address;
	}

	template <class... ArgsType>
	constexpr void construct_next(const ArgsType&&... args) noexcept {
		if (size + 1 > max_size)
			return;
		void (
			new (&address[size])
				value_t(args...)
			);
		size += 1;
	}

	constexpr void move(detachable_manager_impl& other) {
		other.address = address;
		other.size = size;
		other.max_size = max_size;
		address = nullptr;
	}

	constexpr void remove_end() noexcept {
		if (!size)
			return;
		address[size - 1].~value_t();
		size -= 1;
	}

	constexpr const pointer_t begin() const noexcept {
		return address;
	}

	constexpr const pointer_t end() const noexcept {
		return address + size;
	}

	constexpr const size_t length() const noexcept {
		return size;
	}

	constexpr const size_t max_length() const noexcept {
		return max_size;
	}

	constexpr const pointer_t operator&() const noexcept {
		return address;
	}

	constexpr const pointer_t operator[](size_t space) const noexcept {
		if (space == -1 || space > size || space > max_size)
			return nullptr;
		return &address[space];
	}
};