#pragma once
#pragma warning(disable : 4819)

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	class size {

	public:
		// 演算モード
		struct modes {
			enum enum_t {
				subst,  // 代入
				add,    // 加算
				sub,    // 減算
			};
		};

	public:
		size(int w, int h, bool repaint = true)
			: w_(w), h_(h), repaint_(repaint), mode_(modes::subst)
		{}
		size(RECT const& rc, bool repaint = true)
			: w_(rc.right - rc.left), h_(rc.bottom - rc.top), repaint_(repaint), mode_(modes::subst)
		{}

		inline int w() { return w_; }
		inline int h() { return h_; }
		inline bool repaint() { return repaint_; }
		inline modes::enum_t mode() { return mode_; }

		friend size&& operator+(size &&attr);
		friend size&& operator-(size &&attr);

	private:
		// width, height
		// -1を指定した変数は無視する
		int w_, h_;
		// 更新時に再描画するか否か
		bool repaint_;

		modes::enum_t mode_;
	};

	// 加算型
	size&& operator+(size &&attr) {
		attr.mode_ = size::modes::add;
		return std::move(attr);
	}
	// 減算型
	size&& operator-(size &&attr) {
		attr.mode_ = size::modes::sub;
		return std::move(attr);
	}

}}}}// namespace lulib::win32api::window_detail::attribute
