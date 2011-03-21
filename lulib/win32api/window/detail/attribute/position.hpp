#pragma once
#pragma warning(disable : 4819)

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	class position {

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
		position(int x, int y, bool repaint = true)
			: x_(x), y_(y), w_(-1), h_(-1), repaint_(repaint), mode_(modes::subst)
		{}
		position(int x, int y, int w, int h, bool repaint = true)
			: x_(x), y_(y), w_(w), h_(h), repaint_(repaint), mode_(modes::subst)
		{}
		position(RECT const& rc, bool repaint = true)
			: x_(rc.left), y_(rc.top), w_(rc.right - rc.left), h_(rc.bottom - rc.top)
			, repaint_(repaint), mode_(modes::subst)
		{}

		inline int x() { return x_; }
		inline int y() { return y_; }
		inline int w() { return w_; }
		inline int h() { return h_; }
		inline bool repaint() { return repaint_; }
		inline modes::enum_t mode() { return mode_; }

		friend position&& operator+(position &&attr);
		friend position&& operator-(position &&attr);

	private:
		// x座標, y座標, width, height
		// -1を指定した変数は無視する
		int x_, y_, w_, h_;
		// 更新時に再描画するか否か
		bool repaint_;

		modes::enum_t mode_;
	};

	// 加算型
	position&& operator+(position &&attr) {
		attr.mode_ = position::modes::add;
		return std::move(attr);
	}
	// 減算型
	position&& operator-(position &&attr) {
		attr.mode_ = position::modes::sub;
		return std::move(attr);
	}

}}}}// namespace lulib::win32api::window_detail::attribute
