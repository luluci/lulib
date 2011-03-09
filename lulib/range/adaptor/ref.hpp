#pragma once

#include <boost/iterator.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_categories.hpp>

#include <boost/range/iterator_range.hpp>

#include <boost/multi_index_container.hpp>

#include <boost/optional.hpp>

#include <lulib/range/range_categories.hpp>

namespace lulib { namespace range { namespace adaptors {

	namespace detail {
		template<typename Iterator, typename Container>
		class ref_iterator;

		template<typename Iterator, typename Container>
		struct ref_iterator_base {
			typedef boost::iterator_adaptor<
				ref_iterator<Iterator,Container>,
				Iterator,
				boost::use_default,
				typename boost::mpl::if_<
					boost::is_convertible<
						typename boost::iterator_traversal<Iterator>::type,
						boost::random_access_traversal_tag
					>,
					boost::bidirectional_traversal_tag,
					boost::use_default
         			>::type
			> type;
		};

		template<typename Iterator, typename Container>
		class ref_iterator : public detail::ref_iterator_base<Iterator,Container>::type
		{
			typedef typename detail::ref_iterator_base<Iterator,Container>::type super_t;

			friend class boost::iterator_core_access;

		public:
			ref_iterator() {}
			ref_iterator(Iterator x, boost::optional<Container&> &ref)
				: super_t(x), ref_(ref)
			{
			}
			ref_iterator(Iterator x, boost::optional<Container&> const& ref)
				: super_t(x), ref_(ref)
			{
			}

			template<typename Function>
			void apply(Function const& func) {
				/*
				ref_.get().modify( this->base_reference(), [&](typename Container::value_type &value){
					func(value);
				});
				*/
				auto it = ref_.get().iterator_to( *this->base_reference() );
				ref_.get().modify( it, [&](typename Container::value_type &value){
					func(value);
				});
			}

		private:
			boost::optional<Container&> ref_;
		};

		// ref_range class
		template<typename Range, typename Container>
		class ref_range :
			public boost::iterator_range<
				ref_iterator< typename boost::range_iterator<Range>::type, Container >
			>
		{
			typedef boost::iterator_range<
				ref_iterator< typename boost::range_iterator<Range>::type, Container >
			> base;

			typedef boost::optional<Container&> ref_opt;

		public:
			typedef Container conatiner_type;
			typedef lulib::range::post_range_tag range_category;

			typedef ref_iterator< typename boost::range_iterator<Range>::type, Container > iterator;

			explicit ref_range (Range &r, ref_opt & ref)
				: base( iterator(boost::begin(r), ref), iterator(boost::end(r), ref) ), ref_(ref)
			{}
			explicit ref_range (Range &r, ref_opt const& ref)
				: base( iterator(boost::begin(r), ref), iterator(boost::end(r), ref) ), ref_(ref)
			{}

		private:
			ref_opt ref_;
		};

		// tag dispatch的な
		template<typename Container>
		struct ref_t {
			typedef Container container_type;
			boost::optional<Container&> ref;

			// ctor
			ref_t() {}
			ref_t(Container &c) : ref(c) {}

			// make copy
			template<typename T>
			ref_t<T> operator()(T &t) const {
				return ref_t<T>(t);
			}
		};

		// pipe的な
		// ADLで呼び出されるよん
		template<typename V, typename I, typename A, typename C>
		inline ref_range<
			boost::multi_index::multi_index_container<V,I,A>,
			boost::multi_index::multi_index_container<V,I,A>
		>
		operator|( boost::multi_index::multi_index_container<V,I,A> &mic, ref_t<C> ) {
			typedef boost::multi_index::multi_index_container<V,I,A> container;
			typedef ref_range< container, container > result_type;
			//std::cout << "    operator|(boost::multi_index::multi_index_container<V,I,A> &mic, ref_t const& ref)" << std::endl;
			return result_type(mic, boost::optional<container&>(mic));
		}
		// range
		template<typename Range, typename C>
		inline ref_range<Range,C>
		operator|(Range &rng, ref_t<C> ref) {
			//std::cout << "    operator|(Range &rng, ref_t const& ref)" << std::endl;
			return ref_range<Range,C>(rng, ref.ref);
		}
		// const
		template<typename Range, typename C>
		inline ref_range<Range const,C>
		operator|(Range const& rng, ref_t<C> ref) {
			//std::cout << "    operator|(Range const& rng, ref_t const& ref)" << std::endl;
			return ref_range<Range const,C>(rng, ref.ref);
		}
	}//namespace detail

	//using detail::ref_range;

	// refです
	namespace {
		const detail::ref_t<int> ref = detail::ref_t<int>();
	}

	/*
	// 手動range作成関数？
	template<typename Range>
	inline void//ref_range<Range>
	ref(Range &rng) {
		//return ref_range<Range>(rng);
	}
	// const
	template<typename Range>
	inline void//ref_range<Range const>
	ref(Range const& rng) {
		//return ref_range<Range const>(rng);
	}
	*/

}}}// namespace lulib::range::adaptors
