#pragma once
#pragma warning(disable : 4819)

#include <type_traits>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/count_if.hpp>
#include <boost/mpl/remove.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/unpack_args.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/bool.hpp>

#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/selection/max.hpp>

// enable_conditionが受け取るconditionの上限数
#ifndef LULIB_ENABLE_CONDITION_MAX_CONDITION_SIZE
#	define LULIB_ENABLE_CONDITION_MAX_CONDITION_SIZE 10
#endif
// conditionに受け取るテンプレート引数の上限数
#ifndef LULIB_ENABLE_CONDITION_MAX_PARAM_SIZE
#	define LULIB_ENABLE_CONDITION_MAX_PARAM_SIZE 10
#endif
// override the maximum supported arity of metafunctions/classes
#ifndef BOOST_MPL_LIMIT_METAFUNCTION_ARITY
#	define BOOST_MPL_LIMIT_METAFUNCTION_ARITY BOOST_PP_MAX( \
		LULIB_ENABLE_CONDITION_MAX_CONDITION_SIZE, \
		LULIB_ENABLE_CONDITION_MAX_PARAM_SIZE \
	)
#endif

// template parameters
//   typename T0 = mpl::void_, typename T1 = mpl::void_, ... , typename T10 = mpl::void_
#define LULIB_ENABLE_CONDITION_PARAMS \
	BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(LULIB_ENABLE_CONDITION_MAX_PARAM_SIZE, typename T, mpl::na)
// template arguments
//   T0, T1, ... , T10
#define LULIB_ENABLE_CONDITION_ARGS \
	BOOST_PP_ENUM_PARAMS(LULIB_ENABLE_CONDITION_MAX_PARAM_SIZE, T)

namespace lulib {
	namespace enable_condition_detail {
		namespace mpl = boost::mpl;

		template<LULIB_ENABLE_CONDITION_PARAMS>
		struct make_sequence : public mpl::remove< mpl::vector<LULIB_ENABLE_CONDITION_ARGS>, mpl::na> {};

		template <typename Seq>
		class enable_condition_impl {
		public:
			// conditions sequence
			typedef Seq seq;
			// sequence size
			typedef typename mpl::size<seq>::type seq_size;
			typedef decltype(seq_size::value) size_type;
			// sequence size constant
			static const size_type seq_size_c = seq_size::value;

			template<typename Cond, typename ParamSeq>
			struct apply_seq : public mpl::apply< mpl::unpack_args<Cond>, ParamSeq >::type {};

			// condition sequenceのPos番目のconditionにParamSeqを適用した結果を返す
			template<size_type Pos, typename ParamSeq>
			struct match : public apply_seq< typename mpl::at_c<seq, Pos>::type, ParamSeq > {};
			// condition sequenceの1~Pos-1番目のconditionにマッチせず、
			// Pos番目のconditionにマッチした場合にtrue
			template<size_type Pos, typename ParamSeq>
			struct match_never_at : public mpl::bool_<
				mpl::find_if< seq, apply_seq<mpl::_1, ParamSeq> >::type::pos::value == Pos
			> {};
			// condition sequenceに対してParamSeqをapplyしたときに
			// trueとなるconditionの数をカウントする
			template<typename ParamSeq>
			struct match_count : public mpl::count_if< seq, apply_seq<mpl::_1, ParamSeq> >::type {};

			template<size_type Pos, typename ParamSeq>
			struct if_impl : public std::enable_if< match<Pos,ParamSeq>::value > {};
			template<size_type Pos, typename ParamSeq>
			struct else_if_impl : public std::enable_if< match_never_at<Pos,ParamSeq>::value > {};
			template<typename ParamSeq>
			struct else_impl : public std::enable_if< match_never_at<seq_size_c,ParamSeq>::value > {};
			template<size_type Pos, typename ParamSeq>
			struct case_impl : public if_impl<Pos,ParamSeq> {
				static_assert(
					match_count<ParamSeq>::value < 2,
					"detect conflicting condition"
				);
			};

		public:
			// if: Pos番目のconditionに対してTをapplyした結果がtrue
			//         のとき有効
			//     (その他のconditionとは独立している)
			//     (Pos番目のconditionに対してenable_ifと同等の動作をする)
			template<size_type Pos, LULIB_ENABLE_CONDITION_PARAMS>
			struct if_ : public if_impl<
				Pos, typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};
			// else if: 0~Pos-1番目までのconditionにTをapplyした結果がfalse
			//          かつ Pos番目のconditionに対してTをapplyした結果がtrue
			//              のとき有効
			template<size_type Pos, LULIB_ENABLE_CONDITION_PARAMS>
			struct else_if_ : public else_if_impl<
				Pos, typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};
			// else: 各condition sequenceのconditionにTをapplyした結果がすべてfalse
			//         のとき有効
			template<LULIB_ENABLE_CONDITION_PARAMS>
			struct else_ : public else_impl<
				typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};
			// case: Pos番目のconditionに対してTをapplyした結果がtrue
			//           のとき有効
			//       ただし、condition sequenceに対してTをapplyしたときに
			//       2つ以上のconditionがtrueとなったらstatic_assertを
			template<size_type Pos, LULIB_ENABLE_CONDITION_PARAMS>
			struct case_ : public case_impl<
				Pos, typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};
			// default_: condition sequenceのすべてのマッチしない
			//           else_と同等
			template<LULIB_ENABLE_CONDITION_PARAMS>
			struct default_ : public else_impl<
				typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};

		};
	}// namespace lulib::enable_condition_detail

	// using placeholders
	using namespace boost::mpl::placeholders;

	template <BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(LULIB_ENABLE_CONDITION_MAX_CONDITION_SIZE, typename T, boost::mpl::na)>
	struct enable_condition : public enable_condition_detail::enable_condition_impl<
		typename enable_condition_detail::make_sequence<
			BOOST_PP_ENUM_PARAMS(LULIB_ENABLE_CONDITION_MAX_CONDITION_SIZE, T)
		>::type
	> {
		typedef enable_condition type;
	};

}// namespace lulib

#undef LULIB_ENABLE_CONDITION_PARAMS
#undef LULIB_ENABLE_CONDITION_ARGS
