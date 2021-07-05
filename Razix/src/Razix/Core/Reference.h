#pragma once

#include "Razix/Core/Core.h"

#include <memory>

namespace Razix
{
	// TODO: Create custom smart pointer types using custom allocators

	template <typename T>
	using Scope = std::unique_ptr <T>;

	template <typename T>
	using Ref = std::shared_ptr<T>;
}