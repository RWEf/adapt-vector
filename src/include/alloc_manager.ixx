export module alloc_manager;

import alloc_manager_impl;
import instruction;

export template <typename AllocType>
using base_alloc_manager = base_alloc_manager_impl<AllocType>;

export template <class AllocTy, bool IsBaseType = std::is_base_type_v<AllocTy>>
using deta_manager = detachable_manager_impl<AllocTy, std::is_base_type_v<AllocTy>>;