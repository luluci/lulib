#pragma once
#pragma warning(disable : 4819)

#include <typeinfo>

namespace lulib {

	class bad_lexical_cast : public std::bad_cast {

	public:
		bad_lexical_cast() : source_(), target_(&typeid(void)) {
		}
		template<typename Iterator>
		bad_lexical_cast(Iterator it, Iterator end, std::type_info const& ti)
		: source_(it, end), target_(&ti) {
		}
		virtual ~bad_lexical_cast() throw() {
		}

		std::string const& source() const {
			return source_;
		}
		std::type_info const& target_type() const {
			return *target_;
		}

		virtual char const* what() const throw(){
			return "bad lexical cast: source type value could not be interpreted as target";
		}

	private:
		std::string source_;
		std::type_info const* target_;
	};

}// namespace lulib

