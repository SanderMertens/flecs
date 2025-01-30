
#include "flecs.h"

namespace flecs {
	namespace _ {

		robin_hood::unordered_map<std::string_view, type_impl_data> g_type_to_impl_data;

	} // namespace _
} // namespace flecs