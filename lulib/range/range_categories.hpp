#pragma once

namespace lulib { namespace range {

	// apply()を持ち、funcに直接valueを渡すようなrange
	struct post_range_tag {};

	// 通常のrange
	struct default_range_tag {};

}}// namespace lulib::range
