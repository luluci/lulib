#pragma once
#pragma warning(disable : 4819)

#include <cstdint>

namespace lulib { namespace unicode_detail { namespace traits {

	// traitsが返すcode-point情報型
	struct codepoint {
		std::uint32_t value;
		std::size_t bit_size;
		std::size_t byte_size;

		codepoint() : value(0), bit_size(0), byte_size(0) {}
		codepoint(std::uint32_t cp, std::size_t bit, std::size_t byte)
		: value(cp), bit_size(bit), byte_size(byte) {}
	};

}}}// namespace lulib::unicode_detail::traits
