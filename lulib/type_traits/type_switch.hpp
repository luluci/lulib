#pragma once
#pragma warning(disable : 4819)

#include <type_traits>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/remove.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/unpack_args.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/placeholders.hpp>

#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/selection/max.hpp>

// enable_conditionが受け取るconditionの上限数
#ifndef LULIB_TYPE_SWITCH_MAX_CONDITION_SIZE
#	define LULIB_TYPE_SWITCH_MAX_CONDITION_SIZE 10
#endif
// conditionに受け取るテンプレート引数の上限数
#ifndef LULIB_TYPE_SWITCH_MAX_PARAM_SIZE
#	define LULIB_TYPE_SWITCH_MAX_PARAM_SIZE 10
#endif
// override the maximum supported arity of metafunctions/classes
#ifndef BOOST_MPL_LIMIT_METAFUNCTION_ARITY
#	define BOOST_MPL_LIMIT_METAFUNCTION_ARITY BOOST_PP_MAX( \
		LULIB_TYPE_SWITCH_MAX_CONDITION_SIZE, \
		LULIB_TYPE_SWITCH_MAX_PARAM_SIZE \
	)
#endif

// template parameters
//   typename T0 = mpl::void_, typename T1 = mpl::void_, ... , typename T10 = mpl::void_
#define LULIB_TYPE_SWITCH_PARAMS \
	BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(LULIB_TYPE_SWITCH_MAX_PARAM_SIZE, typename T, mpl::na)
// template arguments
//   T0, T1, ... , T10
#define LULIB_TYPE_SWITCH_ARGS \
	BOOST_PP_ENUM_PARAMS(LULIB_TYPE_SWITCH_MAX_PARAM_SIZE, T)

namespace lulib {
	namespace type_switch_detail {
		namespace mpl = boost::mpl;

		// sequenceからsizeを取得
		template<typename Seq>
		struct size : public mpl::size<Seq>::type {};

		// テンプレート引数からmpl::naを削除した上でsequence(mpl::vector)を作成する
		template<LULIB_TYPE_SWITCH_PARAMS>
		struct make_sequence : public mpl::remove< mpl::vector<LULIB_TYPE_SWITCH_ARGS>, mpl::na> {};
		// Lambda式にsequenceをapplyする
		template<typename Cond, typename ParamSeq>
		struct apply_seq : public mpl::apply< mpl::unpack_args<Cond>, ParamSeq >::type {};

		// make_seq policy
		struct get_cond {
			template<typename Case>
			struct get {
				typedef typename Case::condition type;
			};
		};
		struct get_then {
			template<typename Case>
			struct get {
				typedef typename Case::then_type type;
			};
		};
		// impl
		template<typename Policy, typename CondSeq, typename Seq, typename Iter>
		struct make_elem_seq_impl {
			typedef typename make_elem_seq_impl<
				Policy,
				typename mpl::push_back<
					CondSeq,
					typename Policy::template get< typename mpl::deref<Iter>::type >::type
				>::type,
				Seq, typename mpl::next<Iter>::type
			>::type type;
		};
		template<typename Policy, typename CondSeq, typename Seq>
		struct make_elem_seq_impl<Policy, CondSeq, Seq, typename mpl::end<Seq>::type> {
			typedef CondSeq type;
		};
		// type_case<> から Lambda を取り出してシーケンスを作成
		template<typename Policy, typename Seq>
		struct make_elem_seq : make_elem_seq_impl<
			Policy, typename mpl::vector<>::type, Seq, typename mpl::begin<Seq>::type
		> {};

		// 
		template<typename Seq, typename ParamSeq>
		struct match_pos {
			typedef typename size<Seq>::value_type size_type;
			static const size_type value = mpl::find_if<
				Seq, apply_seq<mpl::_1, ParamSeq>
			>::type::pos::value;
		};

		template<typename CondSeq, typename ThenSeq, typename ParamSeq>
		struct case_impl : mpl::at< ThenSeq, match_pos<CondSeq,ParamSeq> > {};


		template <typename Seq>
		class type_switch_impl {
			typedef typename size<Seq>::value_type size_type;
			static const size_type size = size<Seq>::value;

			// type_case::condition の sequence
			typedef typename make_elem_seq<get_cond, Seq>::type cond_seq;
			// type_case::then の sequence
			typedef typename make_elem_seq<get_then, Seq>::type then_seq;
			// matchしなかったとき用にvoidをセット
			typedef typename mpl::push_back<then_seq,void>::type then_seq_fix;

		public:
			template<LULIB_TYPE_SWITCH_PARAMS>
			struct case_ : case_impl<
				cond_seq, then_seq_fix, typename make_sequence< LULIB_TYPE_SWITCH_ARGS >::type
			> {};
		};

	}// namespace lulib::type_switch_detail

	// using placeholders
	using namespace boost::mpl::placeholders;

	template<
		BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
			LULIB_TYPE_SWITCH_MAX_CONDITION_SIZE, typename T, boost::mpl::na
		)
	> struct type_switch : public type_switch_detail::type_switch_impl<
		typename type_switch_detail::make_sequence<
			BOOST_PP_ENUM_PARAMS(LULIB_TYPE_SWITCH_MAX_CONDITION_SIZE, T)
		>::type
	> {
		typedef type_switch type;
	};

	// type_case_c
	// false
	template<bool, typename Then>
	struct type_case_c {
		typedef Then then_type;
	};
	// true
	template<typename Then>
	struct type_case_c<true,Then> {
		typedef Then then_type;
		typedef Then type;
	};
	// type_case
	template<typename Cond, typename Then>
	struct type_case : type_case_c<Cond::value,Then> {
		typedef Cond condition;
	};

}// namespace lulib

#undef MAKE_LULIB_TYPE_SWITCH
#undef LULIB_TYPE_SWITCH_PARAMS
#undef LULIB_TYPE_SWITCH_ARGS
