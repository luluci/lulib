
#include <iostream>

#include <boost/filesystem/operations.hpp>

#include <lulib/jwn.hpp>

int main(int argc, char **argv) {
	if (argc < 6) {
		std::cerr << "Usage: " << argv[0] << " <dbfile> <word> <link> <pos> <lang>" << std::endl;
		return 1;
	}

	// sample code
	typedef lulib::jwn jwn;

	std::string jwn_path = argv[1];

	// ファイル存在チェック
	{
		namespace fs = boost::filesystem;
		fs::path p(jwn_path);
		if (!fs::exists(p)) {
			std::cerr << "jwn file(" << jwn_path << ") not exists." << std::endl;
			return 1;
		}
	}

	jwn wn(jwn_path);  // 日本語WordNet dbファイルまでのパス
	if (!wn.is_open()) {
		std::cerr << "cannot open sqlite3 db file" << std::endl;
		return 1;
	}

	std::string word = argv[2];

	jwn::link::enum_t link = jwn::link::syns;
	std::string link_s = argv[3];
	if      (link_s == "syns") link = jwn::link::syns;
	else if (link_s == "coor") link = jwn::link::coor;
	else if (link_s == "hype") link = jwn::link::hype;
	else if (link_s == "hypo") link = jwn::link::hypo;

	jwn::pos::enum_t pos = jwn::pos::noun;
	std::string pos_s = argv[4];
	if      (pos_s == "noun") pos = jwn::pos::noun;
	else if (pos_s == "verb") pos = jwn::pos::verb;
	else if (pos_s == "adj")  pos = jwn::pos::adj;
	else if (pos_s == "adv")  pos = jwn::pos::adv;

	jwn::lang::enum_t lang = jwn::lang::jpn;
	std::string lang_s = argv[5];
	if      (lang_s == "jpn") lang = jwn::lang::jpn;
	else if (lang_s == "eng") lang = jwn::lang::eng;

	std::cout << "word:" << word << ", link:" << link_s << ", pos:" << pos_s << ", lang:" << lang_s << std::endl;
	bool ret = wn(word, link, pos, lang,
		[](std::string const& w) -> bool {
			std::cout << w << std::endl;
			return true;  // trueを返す間ループを続ける。falseを返すとその時点で返ってくる
		}
	);
}
