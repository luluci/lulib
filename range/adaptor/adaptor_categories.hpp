#pragma once

namespace lulib { namespace range {

	// apply()を持ち、funcに直接valueを渡すようなrange
	struct post_adaptor_tag {};

	// 通常のrange
	struct adaptor_tag : public post_adaptor_tag {};

}}// namespace lulib::range
