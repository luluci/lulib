#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <string>
#include <memory>

#include <lulib/win32api/font/detail/basic_font_op.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

#include <boost/optional.hpp>

namespace lulib { namespace win32api {

	namespace font_detail {
		struct HFONT_deleter {
			typedef HFONT pointer;
			inline void operator()( HFONT hFont ) const {
				if (hFont) ::DeleteObject( hFont );
			}
		};

		//typedef select_object<HFONT> select_font;

		template<typename Char = TCHAR>
		class basic_font {
			typedef basic_font<Char> self_type;
			typedef ::lulib::win32api::font_detail::policy<Char> policy;  // font policy
			typedef typename policy::log_font log_font;

#ifdef _MSC_VER
			typedef std::unique_ptr<HFONT, HFONT_deleter> font_type;
#else
			typedef std::unique_ptr< std::remove_pointer<HFONT>::type, HFONT_deleter> font_type;
#endif


			// Char型特性
			typedef lulib::char_traits<Char>          char_traits;
			typedef typename char_traits::char_type   char_type;
			typedef typename char_traits::string_type string_type;

		public:
			basic_font(void) : font_(), log_font_(), log_pixels_y_(), pt_(), face_() {
				init_log_font();
			}
			~basic_font(void) {
			}
		private:
			basic_font(self_type const&);
			self_type const& operator=(self_type const&);

		private:
			void init_log_font() {
				// フォント情報構造体を０で初期化
				ZeroMemory( &log_font_, sizeof(log_font_) );

				// フォントの幅、高さ
				log_font_.lfWeight = 0;
				log_font_.lfHeight = -12;  // 絶対値が大きいほどフォントサイズも大きく
				log_font_.lfCharSet = DEFAULT_CHARSET;  // システムのデフォルト文字セットを使う
				log_font_.lfOutPrecision = OUT_DEFAULT_PRECIS;  // 物理フォントを検索するための方法
				log_font_.lfClipPrecision = CLIP_DEFAULT_PRECIS;  // はみ出した文字をクリップする方法
				log_font_.lfQuality = DEFAULT_QUALITY;  // アンチエイリアス
				log_font_.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; //VARIABLE_PITCH | FF_ROMAN;
				//Wwsprintf(log_font_.lfFaceName, policy::format, font_name_.c_str());  // フォント情報構造体の設定
			}

		public:
			inline operator HFONT() {
				return font_.get();
			}
			inline operator bool() {
				return (font_.get() != NULL);
			}

			// 論理フォントを作成する
			void create(void) {
				// 論理フォントを作成する
				font_.reset( ::CreateFontIndirect( &log_font_ ) );
				// フォントハンドルの確保に失敗したら
				if (font_.get() == NULL) throw ra_error("failed to acquire font resrc.");
			}

			// init
			typedef attribute::init init;
			template<typename T>
			friend basic_font<T>& attribute::operator<<(basic_font<T>&, attribute::init&&);
			// PtSize
			typedef attribute::point pt;
			typedef attribute::point point;
			template<typename T>
			friend basic_font<T>& attribute::operator<<(basic_font<T>&, attribute::point&&);
			// Height
			typedef attribute::height size;
			typedef attribute::height height;
			template<typename T>
			friend basic_font<T>& attribute::operator<<(basic_font<T>&, attribute::height&&);
			// Regular
			static const attribute::regular_t regular;
			template<typename T>
			friend basic_font<T>& attribute::operator<<(basic_font<T>&, attribute::regular_t const&);
			// Bold
			static const attribute::bold_t bold;
			template<typename T>
			friend basic_font<T>& attribute::operator<<(basic_font<T>&, attribute::bold_t const&);
			// UnderLine
			static const attribute::underline_t underline;
			template<typename T>
			friend basic_font<T>& attribute::operator<<(basic_font<T>&, attribute::underline_t const&);
			// FontFace
			typedef attribute::basic_face<Char> face;
			typedef face facename;
			typedef face name;
			template<typename T>
			friend basic_font<T>& attribute::operator<<(basic_font<T>&, attribute::basic_face<T>&&);

		private:
			font_type font_;     // FontHandler
			log_font log_font_;  // フォント情報の構造体
			boost::optional<int> log_pixels_y_;  // logPixelsY
			int pt_;             // PointSize
			string_type face_;   // face name
		};
		template<typename T> const attribute::regular_t   basic_font<T>::regular   = {};
		template<typename T> const attribute::bold_t      basic_font<T>::bold      = {};
		template<typename T> const attribute::underline_t basic_font<T>::underline = {};

	}// namespace lulib::win32api::font_detail

	using font_detail::basic_font;

} }// namespace lulib::win32api
