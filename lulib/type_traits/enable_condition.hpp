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
#include <boost/mpl/not.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/bool.hpp>

#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/selection/max.hpp>

#include <boost/static_assert.hpp>

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

		// sequenceからsizeを取得
		template<typename Seq>
		struct size : public mpl::size<Seq>::type {};

		// Posが条件数以上の値になっていないか
		template<typename Seq, typename size<Seq>::value_type Pos>
		struct check_range {
			static_assert(
				Pos < size<Seq>::value,
				"detect out of range"
			);
		};
		// condition sequenceからPos番目のconditionを取り出す
		template<typename CondSeq, typename size<CondSeq>::value_type Pos>
		struct get_cond : mpl::at_c<CondSeq, Pos> {
			static_assert(
				Pos < size<CondSeq>::value,
				"detect out of range"
			);
		};

		// テンプレート引数からmpl::naを削除した上でsequence(mpl::vector)を作成する
		template<LULIB_ENABLE_CONDITION_PARAMS>
		struct make_sequence : public mpl::remove< mpl::vector<LULIB_ENABLE_CONDITION_ARGS>, mpl::na> {};
		// Lambda式にsequenceをapplyする
		template<typename Cond, typename ParamSeq>
		struct apply_seq : public mpl::apply< mpl::unpack_args<Cond>, ParamSeq >::type {};
		// Lambda式にsequenceをapplyし、その結果を反転する
		template<typename Cond, typename ParamSeq>
		struct apply_seq_inv : public mpl::not_< mpl::apply< mpl::unpack_args<Cond>, ParamSeq > >::type {};

		// CondSeqのPos番目のconditionにParamSeqを適用した結果を返す
		template<typename CondSeq, typename size<CondSeq>::value_type Pos, typename ParamSeq>
		struct match : public apply_seq< typename get_cond<CondSeq, Pos>::type, ParamSeq > {};
		// CondSeqの各conditionにParamSeqをapplyしたとき、
		// Pos番目のconditionで初めてtrueとなるかどうか
		template<typename CondSeq, typename size<CondSeq>::value_type Pos, typename ParamSeq>
		struct match_first : public mpl::bool_<
			mpl::find_if< CondSeq, apply_seq<mpl::_1, ParamSeq> >::type::pos::value == Pos
		> {};
		// CondSeqの各conditionにParamSeqをapplyしたとき、
		// Pos番目のconditionで初めてfalseとなるかどうか
		template<typename CondSeq, typename size<CondSeq>::value_type Pos, typename ParamSeq>
		struct not_match_first : public mpl::bool_<
			mpl::find_if< CondSeq, apply_seq_inv<mpl::_1, ParamSeq> >::type::pos::value == Pos
		> {};
		// condition sequenceに対してParamSeqをapplyしたときに
		// trueとなるconditionの数をカウントする
		template<typename CondSeq, typename ParamSeq>
		struct match_count : public mpl::count_if< CondSeq, apply_seq<mpl::_1, ParamSeq> >::type {};
		// condition sequenceに対してParamSeqをapplyしたときに
		// falseとなるconditionの数をカウントする
		template<typename CondSeq, typename ParamSeq>
		struct not_match_count : public mpl::count_if< CondSeq, apply_seq_inv<mpl::_1, ParamSeq> >::type {};

		// 重複する条件があるか
		template<typename Seq, typename ParamSeq>
		struct check_duplication {
			static_assert(
				2 > match_count<Seq,ParamSeq>::value,
				"detect conflicting condition"
			);
		};
		template<typename Seq, typename ParamSeq>
		struct check_duplication_inv {
			static_assert(
				2 > not_match_count<Seq,ParamSeq>::value,
				"detect conflicting condition"
			);
		};

		// enable policy
		struct enable_policy {
			// if impl
			template<typename Seq, typename size<Seq>::value_type Pos, typename ParamSeq>
			struct if_ : public std::enable_if< match<Seq,Pos,ParamSeq>::value > {};
			// else if impl
			template<typename Seq, typename size<Seq>::value_type Pos, typename ParamSeq>
			struct else_if_ : public std::enable_if< match_first<Seq,Pos,ParamSeq>::value > {};
			// else impl
			template<typename Seq, typename ParamSeq>
			struct else_ : public std::enable_if< match_first<Seq,size<Seq>::value,ParamSeq>::value > {};
			// case impl
			template<typename Seq, typename size<Seq>::value_type Pos, typename ParamSeq>
			struct case_ : public if_<Seq,Pos,ParamSeq>, check_duplication<Seq,ParamSeq> {};
		};
		// disable policy
		struct disable_policy {
			// if impl
			template<typename Seq, typename size<Seq>::value_type Pos, typename ParamSeq>
			struct if_ : public std::enable_if< !match<Seq,Pos,ParamSeq>::value > {};
			// else if impl
			template<typename Seq, typename size<Seq>::value_type Pos, typename ParamSeq>
			struct else_if_ : public std::enable_if< not_match_first<Seq,Pos,ParamSeq>::value > {};
			// else impl
			template<typename Seq, typename ParamSeq>
			struct else_ : public std::enable_if< not_match_first<Seq,size<Seq>::value,ParamSeq>::value > {};
			// case impl
			template<typename Seq, typename size<Seq>::value_type Pos, typename ParamSeq>
			struct case_ : public if_<Seq,Pos,ParamSeq>, check_duplication_inv<Seq,ParamSeq> {};
		};

		template <typename Seq, typename Policy = enable_policy>
		class enable_condition_impl {
			typedef typename size<Seq>::value_type size_type;
			static const size_type size = size<Seq>::value;

		public:
			// if: Pos番目のconditionに対してTをapplyした結果がtrue
			//         のとき有効
			//     (その他のconditionとは独立している)
			//     (Pos番目のconditionに対してenable_ifと同等の動作をする)
			template<size_type Pos, LULIB_ENABLE_CONDITION_PARAMS>
			struct if_ : public Policy::template if_<
				Seq, Pos, typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};
			// else if: 0~Pos-1番目までのconditionにTをapplyした結果がfalse
			//          かつ Pos番目のconditionに対してTをapplyした結果がtrue
			//              のとき有効
			template<size_type Pos, LULIB_ENABLE_CONDITION_PARAMS>
			struct else_if_ : public Policy::template else_if_<
				Seq, Pos, typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};
			// else: 各condition sequenceのconditionにTをapplyした結果がすべてfalse
			//         のとき有効
			template<LULIB_ENABLE_CONDITION_PARAMS>
			struct else_ : public Policy::template else_<
				Seq, typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};
			// case: Pos番目のconditionに対してTをapplyした結果がtrue
			//           のとき有効
			//       ただし、condition sequenceに対してTをapplyしたときに
			//       2つ以上のconditionがtrueとなったらstatic_assertを
			template<size_type Pos, LULIB_ENABLE_CONDITION_PARAMS>
			struct case_ : public Policy::template case_<
				Seq, Pos, typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};
			// default_: condition sequenceのすべてのマッチしない
			//           else_と同等
			template<LULIB_ENABLE_CONDITION_PARAMS>
			struct default_ : public Policy::template else_<
				Seq, typename make_sequence< LULIB_ENABLE_CONDITION_ARGS >::type
			> {};
		};

	}// namespace lulib::enable_condition_detail

	// using placeholders
	using namespace boost::mpl::placeholders;

	// make lulib::enable_condition
#	define MAKE_LULIB_ENABLE_CONDITION(name, policy)						\
		template<										\
			BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(						\
				LULIB_ENABLE_CONDITION_MAX_CONDITION_SIZE, typename T, boost::mpl::na	\
			)										\
		> struct name : public enable_condition_detail::enable_condition_impl<			\
			typename enable_condition_detail::make_sequence<				\
				BOOST_PP_ENUM_PARAMS(LULIB_ENABLE_CONDITION_MAX_CONDITION_SIZE, T)	\
			>::type,									\
			policy										\
		> {											\
			typedef name type;							\
		};											\

	// enable_condition
	MAKE_LULIB_ENABLE_CONDITION(enable_condition, enable_condition_detail::enable_policy);
	// disable_condition
	MAKE_LULIB_ENABLE_CONDITION(disable_condition, enable_condition_detail::disable_policy);

}// namespace lulib

#undef MAKE_LULIB_ENABLE_CONDITION
#undef LULIB_ENABLE_CONDITION_PARAMS
#undef LULIB_ENABLE_CONDITION_ARGS
