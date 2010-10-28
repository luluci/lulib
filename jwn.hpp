#pragma once


/*
// sample code
typedef lulib::jwn jwn;

jwn wn("./wnjpn.db");  // 日本語WordNet dbファイルまでのパス
if (!wn.is_open()) {
	std::cerr << "cannot open sqlite3 db file" << std::endl;
	return 1;
}

std::string word = "test";
jwn::pos::enum_t pos   = jwn::pos::noun;
jwn::lang::enum_t lang = jwn::lang::jpn;

bool ret = wn.synonym(word, pos, lang,
	[](std::string const& w) -> bool {
		std::cout << w << std::endl;
		return true;  // trueを返す間ループを続ける。falseを返すとその時点で返ってくる
	}
);
*/


#include <string>
#include <list>

#include <boost/array.hpp>
#include <boost/optional.hpp>

#include <lulib/sqlite3.hpp>

namespace lulib {

	class jwn {
		typedef lulib::sqlite3 sqlite3;
		typedef sqlite3::row row_type;
		sqlite3 db_;

	public:
		// lang enum
		struct lang {
			enum enum_t {
				jpn = 0,
				eng = 1,
				size,
			};
		};
		// pos enum
		struct pos {
			enum enum_t {
				noun = 0,
				verb = 1,
				adj  = 2,
				adv  = 3,
				size,
			};
		};
		// link enum
		struct link {
			enum enum_t {
				syns = 0,  // synonyms
				coor,      // coordinate
				hype,      // hypernym
				hypo,      // hyponym
				size,
			};
		};

	private:
		typedef boost::array<std::string, lang::size> lang_array;
		lang_array lang_;
		typedef boost::array<std::string, pos::size> pos_array;
		pos_array pos_;
		typedef boost::array<std::string, link::size> link_array;
		link_array link_;

		typedef std::list<int> int_list;
		typedef std::list<std::string> str_list;
		typedef std::list<double> double_list;

	public:
		jwn() : db_() {
			init();
		}
		jwn(const std::string &file) : db_(file) {
			init();
		}
		void init() {
			// 言語タグ
			lang_[lang::jpn] = "jpn";
			lang_[lang::eng] = "eng";
			// POSタグ
			pos_[pos::noun] = "n";
			pos_[pos::verb] = "v";
			pos_[pos::adj]  = "a";
			pos_[pos::adv]  = "r";
			// link
			link_[link::syns] = "syns";
			link_[link::coor] = "coor";
			link_[link::hype] = "hype";
			link_[link::hypo] = "hypo";
		}

		inline bool open(const std::string &file) {
			return db_.open(file);
		}
		inline bool is_open() {
			return db_.is_open();
		}

		// コールバック用ファンクタタイプ
		// 引数：lemma_type: 現れた単語への参照
		// 返り値：bool: 探索終了フラグ(trueで終了, falseで継続)
		// ファンクタがfalseを返す限り処理を続ける
		// trueでbreak
		typedef std::function<bool(std::string&&)> callback_type;

		// 個別指定によるアクセス
		bool operator()(const std::string &word, link::enum_t link, pos::enum_t pos, lang::enum_t lng, const callback_type& func) {
			// linkで分岐
			switch (link) {
				case link::syns: {
					return synonym(word, pos, lng, func);
				}
				case link::coor: {
					return coordinate(word, pos, lng, func);
				}
				case link::hype: {
					return hypernym(word, pos, lng, func);
				}
				case link::hypo: {
					return hyponym(word, pos, lng, func);
				}
			}
			return false;
		}

/*
SELECT word.lemma
FROM word, sense
WHERE sense.synset IN (
    SELECT sense.synset 
    FROM word, sense 
    WHERE word.lemma=?
      AND word.pos=?
      AND word.lang=?
      AND sense.wordid = word.wordid
)
AND sense.wordid = word.wordid
AND word.pos=?
AND word.lang=?;
*/
		// 1. wordからwordidを取得
		// 2. wordidが属するsynsetを取得
		// 3. synsetに属する単語を取得
		bool synonym(const std::string &word, pos::enum_t pos, lang::enum_t lng, const callback_type& func) {
			char const* query = "SELECT word.lemma FROM word, sense WHERE sense.synset IN ( SELECT sense.synset FROM word, sense WHERE word.lemma=? AND word.pos=? AND word.lang=? AND sense.wordid = word.wordid ) AND sense.wordid = word.wordid AND word.pos=? AND word.lang=?;";
			// SQLクエリを実行
			db_.execute(query, word, pos_[pos], lang_[lng], pos_[pos], lang_[lng]).each_row(
				// 呼び出し元にsynsetをコールバック
				[&func](const row_type &row) -> bool {
					// funcの返り値をそのまま返す
					if (auto p = row.as_string(0)) return func( std::move(*p) );
					return false;
				}
			);

			// 最後までループしてたらtrue
			return db_.is_done();
		}

/*
SELECT word.lemma
FROM synlink, word, sense
WHERE
    synlink.link = 'hypo'
    AND synlink.synset1 IN (
        SELECT synlink.synset2
        FROM word, sense, synlink
        WHERE word.lemma = 'boat' 
            AND word.pos = 'n' 
            AND word.lang = 'eng' 
            AND sense.wordid = word.wordid
            AND synlink.synset1 = sense.synset
            AND synlink.link = 'hype'
    )
    AND sense.synset = synlink.synset2
    AND word.wordid = sense.wordid
    AND word.pos = 'n'
    AND word.lang = 'eng';
*/
		// 1. wordからwordidを取得
		// 2. wordidが属するsynsetを取得
		// 3. synsetの上位語synsetを取得
		// 4. 上位語synsetの下位語synset(== 兄弟語synset)を取得
		// 5. 兄弟語synsetに属する単語を取得
		bool coordinate(const std::string &word, pos::enum_t pos, lang::enum_t lng, const callback_type& func) {
			char const* query = "SELECT word.lemma FROM synlink, word, sense WHERE synlink.link = 'hypo' AND synlink.synset1 IN ( SELECT synlink.synset2 FROM word, sense, synlink WHERE word.lemma=? AND word.pos=? AND word.lang=? AND sense.wordid = word.wordid AND synlink.synset1 = sense.synset AND synlink.link = 'hype' ) AND sense.synset = synlink.synset2 AND word.wordid = sense.wordid AND word.pos=? AND word.lang=?;";
			// SQLクエリを実行
			db_.execute(query, word, pos_[pos], lang_[lng], pos_[pos], lang_[lng]).each_row(
				// 呼び出し元にsynsetをコールバック
				[&func](const row_type &row) -> bool {
					// funcの返り値をそのまま返す
					if (auto p = row.as_string(0)) return func( std::move(*p) );
					return false;
				}
			);

			// 最後までループしてたらtrue
			return db_.is_done();
		}

/*
SELECT word.lemma
FROM word, sense
WHERE
    sense.synset IN (
        SELECT synlink.synset2
        FROM word, sense, synlink
        WHERE word.lemma = 'boat' 
            AND word.pos = 'n' 
            AND word.lang = 'eng' 
            AND sense.wordid = word.wordid
            AND synlink.synset1 = sense.synset
            AND synlink.link = 'hype'
    )
    AND word.wordid = sense.wordid
    AND word.pos = 'n'
    AND word.lang = 'eng';
*/
		bool hypernym(const std::string &word, pos::enum_t pos, lang::enum_t lng, const callback_type& func) {
			char const* query = "SELECT word.lemma FROM word, sense WHERE sense.synset IN ( SELECT synlink.synset2 FROM word, sense, synlink WHERE word.lemma=? AND word.pos=? AND word.lang=? AND sense.wordid = word.wordid AND synlink.synset1 = sense.synset AND synlink.link = 'hype' ) AND word.wordid = sense.wordid AND word.pos=? AND word.lang=?;";
			// SQLクエリを実行
			db_.execute(query, word, pos_[pos], lang_[lng], pos_[pos], lang_[lng]).each_row(
				// 呼び出し元にsynsetをコールバック
				[&func](const row_type &row) -> bool {
					// funcの返り値をそのまま返す
					if (auto p = row.as_string(0)) return func( std::move(*p) );
					return false;
				}
			);

			// 最後までループしてたらtrue
			return db_.is_done();
		}

/*
SELECT word.lemma
FROM word, sense
WHERE
    sense.synset IN (
        SELECT synlink.synset2
        FROM word, sense, synlink
        WHERE word.lemma = 'boat' 
            AND word.pos = 'n' 
            AND word.lang = 'eng' 
            AND sense.wordid = word.wordid
            AND synlink.synset1 = sense.synset
            AND synlink.link = 'hypo'
    )
    AND word.wordid = sense.wordid
    AND word.pos = 'n'
    AND word.lang = 'eng';
*/
		bool hyponym(const std::string &word, pos::enum_t pos, lang::enum_t lng, const callback_type& func) {
			char const* query = "SELECT word.lemma FROM word, sense WHERE sense.synset IN ( SELECT synlink.synset2 FROM word, sense, synlink WHERE word.lemma=? AND word.pos=? AND word.lang=? AND sense.wordid = word.wordid AND synlink.synset1 = sense.synset AND synlink.link = 'hypo' ) AND word.wordid = sense.wordid AND word.pos=? AND word.lang=?;";
			// SQLクエリを実行
			db_.execute(query, word, pos_[pos], lang_[lng], pos_[pos], lang_[lng]).each_row(
				// 呼び出し元にsynsetをコールバック
				[&func](const row_type &row) -> bool {
					// funcの返り値をそのまま返す
					if (auto p = row.as_string(0)) return func( std::move(*p) );
					return false;
				}
			);

			// 最後までループしてたらtrue
			return db_.is_done();
		}

	private:
		bool each(const std::string &word, link::enum_t link, pos::enum_t pos, lang::enum_t lng, const callback_type& func) {
			// 対象となる単語のwordidを取得
			int wordid;
			if (optional_int p = get_wordid(word, pos_[pos], lang_[lng])) {
				wordid = p.get();
			}
			else {
				return false;
			}

			// wordidが属するsynsetをすべて取得
			bool ret;
			str_list list;
			ret = get_synset_by_wordid(wordid, list);
			// synsetが1つも取得できなかったら終了
			if (list.empty()) return false;

			// synsetとlinkで関係するsynset2を取得
			str_list links;
			{
				auto it = list.begin(), end = list.end();
				while (it != end) {
					ret = get_synset_by_link(*it, link_[link], links);
					it++;
				}
			}

			// 各synsetに属する単語をすべて抽出
			str_list::iterator it = links.begin(), end = links.end();
			while (it != end) {
				// synsetに属する単語をfuncにコールバックする
				// falseを返したら終了
				if ( !each_word_in_synset(*it, pos_[pos], lang_[lng], func) ) return false;
				it++;
			}

			return true;
		}

		typedef boost::optional<int> optional_int;
		typedef boost::optional<std::string> optional_str;

		// 指定した品詞==pos, 言語==lang, 単語==wordを持つwordidを返す
		optional_int get_wordid(const std::string &word, const std::string &pos, const std::string &lang) {
			// SQLクエリを実行
			db_.execute("SELECT wordid FROM word WHERE lemma=? and pos=? and lang=?", word, pos, lang);
			// wordidを取り出して、返す
			if (db_.has_row()) return optional_int( db_.get_row().as_int(0) );
			return optional_int();
		}
		// wordidと対応付けられているwordを取り出す
		optional_str get_lemma(int wordid, const std::string &pos, const std::string &lang) {
			//std::cout << "wordid:" << wordid << std::endl;
			// SQLクエリを実行
			db_.execute("SELECT lemma FROM word WHERE wordid=? and pos=? and lang=?", wordid, pos, lang);
			// wordidを取り出して、返す
			//std::cout << "lemma:" << db_.get_row().as_string(0) << std::endl;
			if (db_.has_row()) return db_.get_row().as_string(0);
			return optional_str();
		}
		// wordidが属するsynsetをコールバックする
		bool each_synset(int wordid, const callback_type& func) {
			// SQLクエリを実行
			db_.execute("SELECT synset FROM sense WHERE wordid=?", wordid).each_row(
				// 呼び出し元にsynsetをコールバック
				[&func](const row_type &row) -> bool {
					// funcの返り値をそのまま返す
					if (auto p = row.as_string(0)) return func( std::move(*p) );
					return false;
				}
			);

			// 最後までループしてたらtrue
			return db_.is_done();
		}
		// wordidが属するsynsetをlistに格納
		bool get_synset_by_wordid(int wordid, str_list &list) {
			// SQLクエリを実行
			db_.execute("SELECT synset FROM sense WHERE wordid=?", wordid).each_row(
				// 呼び出し元にsynsetをコールバック
				[&list](const row_type &row) -> bool {
					// synsetをlistに追加
					if (auto p = row.as_string(0)) list.push_back( std::move(*p) );
					return true;
				}
			);

			// 最後までループしてたらtrue
			return !list.empty();
		}
		// synsetとlinkで関係するsynset2をlistに追加する
		bool get_synset_by_link(const std::string &synset, const std::string &link, str_list &list) {
			// SQLクエリを実行
			db_.execute("SELECT synset2 FROM synlink WHERE synset1=? and link=?", synset, link).each_row(
				// 呼び出し元にsynsetをコールバック
				[&list](const row_type &row) -> bool {
					// synsetをlistに追加
					if (auto p = row.as_string(0)) list.push_back( std::move(*p) );
					return true;
				}
			);

			// 最後までループしてたらtrue
			return !list.empty();
		}
		// synsetに属するwordidをコールバック
		typedef std::function<bool(int)> callback_int_type;
		bool each_wordid_in_synset(const std::string &synset, const callback_int_type &func) {
			// SQLクエリを実行
			db_.execute("SELECT wordid FROM sense WHERE synset=?", synset).each_row(
				// 呼び出し元にsynsetをコールバック
				[&func](const row_type &row) -> bool {
					// funcの返り値をそのまま返す
					if (auto p = row.as_int(0)) return func( *p );
				}
			);

			// 最後までループしてたらtrue
			return db_.is_done();
		}
		// synsetに属するすべての単語をコールバックする
		bool each_word_in_synset(const std::string &synset, const std::string &pos, const std::string &lang, const callback_type& func) {
			bool ret;

			// synsetに属するwordidをすべて取得
			int_list list;
			ret = each_wordid_in_synset(
				synset,
				[&list](int wordid) -> bool {
					list.push_back(wordid);
					return true;
				}
			);
			// 1つもなかったらfalseを返す
			if (list.empty()) return false;

			auto it = list.begin(), end = list.end();
			while (it != end) {
				if (optional_str p = get_lemma(*it, pos, lang)) {
					// コールバック関数がfalseを返したら終了
					if ( !func( std::move(*p) ) ) return false;
				}
				it++;
			}

			return true;
		}
	};

}
