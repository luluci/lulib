
#include <iostream>

#include <lulib/jwn.hpp>

typedef lulib::jwn jwn;

jwn::pos::enum_t get_pos(std::string const& pos) {
	if      (pos == "noun") return jwn::pos::noun;
	else if (pos == "verb") return jwn::pos::verb;
	else if (pos == "adj")  return jwn::pos::adj;
	else if (pos == "adv")  return jwn::pos::adv;
	else return jwn::pos::null;
}

jwn::lang::enum_t get_lang(std::string const& lang) {
	if      (lang == "jpn") return jwn::lang::jpn;
	else if (lang == "eng") return jwn::lang::eng;
	else return jwn::lang::null;
}

int main(int argc, char **argv) {
	if (argc < 6) {
		std::cerr << "Usage: " << argv[0] << " <dbfile> <word> <link> <pos> <lang> [<trg-lang>]" << std::endl;
		return 1;
	}

	std::string jwn_path = argv[1];
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
	else if (link_s == "also") link = jwn::link::also;
	else if (link_s == "ants") link = jwn::link::ants;
	else if (link_s == "attr") link = jwn::link::attr;
	else if (link_s == "caus") link = jwn::link::caus;
	else if (link_s == "dmnc") link = jwn::link::dmnc;
	else if (link_s == "dmnr") link = jwn::link::dmnr;
	else if (link_s == "dmnu") link = jwn::link::dmnu;
	else if (link_s == "dmtc") link = jwn::link::dmtc;
	else if (link_s == "dmtr") link = jwn::link::dmtr;
	else if (link_s == "dmtu") link = jwn::link::dmtu;
	else if (link_s == "enta") link = jwn::link::enta;
	else if (link_s == "hasi") link = jwn::link::hasi;
	else if (link_s == "hmem") link = jwn::link::hmem;
	else if (link_s == "holo") link = jwn::link::holo;
	else if (link_s == "hprt") link = jwn::link::hprt;
	else if (link_s == "hsub") link = jwn::link::hsub;
	else if (link_s == "inst") link = jwn::link::inst;
	else if (link_s == "mero") link = jwn::link::mero;
	else if (link_s == "mmem") link = jwn::link::mmem;
	else if (link_s == "mprt") link = jwn::link::mprt;
	else if (link_s == "msub") link = jwn::link::msub;
	else if (link_s == "sim") link = jwn::link::sim;
	else if (link_s == "trans") link = jwn::link::trans;

	jwn::pos::enum_t pos = get_pos(argv[4]);
	if (pos == jwn::pos::null) {
		std::cerr << "invalid pos '" << argv[4] << "' input." << std::endl;
		return 1;
	}

	jwn::lang::enum_t lang = get_lang(argv[5]);
	if (lang == jwn::lang::null) {
		std::cerr << "invalid lang '" << argv[5] << "' input." << std::endl;
		return 1;
	}

	if (link != jwn::link::trans) {
		std::cout << "word:" << word << ", link:" << link_s << ", pos:" << argv[4] << ", lang:" << argv[5] << std::endl;
		bool ret = wn(word, link, pos, lang,
			[](std::string const& w) -> bool {
				std::cout << w << std::endl;
				return true;  // trueを返す間ループを続ける。falseを返すとその時点で返ってくる
			}
		);
	}
	// transの場合
	else {
		if (argc < 7) {
			std::cerr << "Usage: " << argv[0] << " <dbfile> <word> trans <pos> <src-lang> <trg-lang>" << std::endl;
			return 1;
		}
		jwn::lang::enum_t trg_lang = get_lang(argv[6]);
		if (trg_lang == jwn::lang::null) {
			std::cerr << "invalid lang '" << argv[6] << "' input." << std::endl;
			return 1;
		}

		std::cout << "word:" << word << ", link:" << link_s << ", pos:" << argv[4] << ", src-lang:" << argv[5] << ", trg-lang:" << argv[6] << std::endl;
		bool ret = wn.translation(word, pos, lang, trg_lang,
			[](std::string const& w) -> bool {
				std::cout << w << std::endl;
				return true;  // trueを返す間ループを続ける。falseを返すとその時点で返ってくる
			}
		);
	}
}
