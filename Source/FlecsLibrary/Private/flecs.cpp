
#include "flecs.h"

namespace flecs {
	namespace _ {

		FLECSLIBRARY_API robin_hood::unordered_map<std::string, type_impl_data> g_type_to_impl_data;

	} // namespace _
} // namespace flecs