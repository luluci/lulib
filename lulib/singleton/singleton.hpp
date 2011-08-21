#pragma once
#pragma warning(disable : 4819)

namespace lulib {

	// singleton
	template<typename T>
	class singleton : public T {
		typedef singleton<T> self_type;
		typedef T instance_type;

	private:
		singleton();
		~singleton();
		singleton(self_type const&);
		singleton(self_type &&);
		void operator=(self_type const&);
		void operator=(self_type &&);

	public:
		static T& instance() {
			static instance_type instance_;  // C++11からスレッドセーフ
			return instance_;
		}
	};

}// namespace lulib
