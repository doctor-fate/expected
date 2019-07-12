#pragma once

namespace stdx
{
	struct unexpect_t
	{
		constexpr explicit unexpect_t() = default;
	};

	constexpr unexpect_t unexpect;
}