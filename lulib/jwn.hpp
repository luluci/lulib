#pragma once

#include <string>
#include <list>

#include <boost/array.hpp>
#include <boost/optional.hpp>

#include <lulib/sqlite3.hpp>

namespace lulib {

	class jwn {
		typedef lulib::sqlite3::database db_type;     // データベース
		typedef lulib::sqlite3::statement stmt_type;  // ステートメント
		typedef lulib::sqlite3::row row_type;         // 行
		db_type db_;

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
				also = 0,  // See also
				ants,      // Antonyms
				attr,      // Attributes
				caus,      // Causes
				dmnc,      // Domain --- Category
				dmnr,      // Domain --- Region
				dmnu,      // Domain --- Usage
				dmtc,      // In Domain --- Category
				dmtr,      // In Domain --- Region
				dmtu,      // In Domain --- Usage
				enta,      // Entails
				hasi,      // Has Instance
				hmem,      // Holonyms --- Member
				holo,      // Holonyms
				hprt,      // Holonyms --- Part
				hsub,      // Holonyms --- Substance
				hype,      // Hypernym
				hypo,      // Hyponym
				inst,      // Instances
				mero,      // Meronyms
				mmem,      // Meronyms --- Member
				mprt,      // Meronyms --- Part
				msub,      // Meronyms --- Substance
				sim,       // Similar to
				syns,      // Synonyms

				// 独自拡張link
				coor,      // coordinate
				size,
			};
		};

	private:
		typedef boost::array<char const*, lang::size> lang_array;
		lang_array lang_;
		typedef boost::array<char const*, pos::size> pos_array;
		pos_array pos_;
		typedef boost::array<char const*, link::size> link_array;
		link_array link_;

	public:
		jwn() : db_() {
			init();
		}
		jwn(std::string const& file) : db_(file) {
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
			link_[link::also] = "also";
			link_[link::ants] = "ants";
			link_[link::attr] = "attr";
			link_[link::caus] = "caus";
			link_[link::dmnc] = "dmnc";
			link_[link::dmnr] = "dmnr";
			link_[link::dmnu] = "dmnu";
			link_[link::dmtc] = "dmtc";
			link_[link::dmtr] = "dmtr";
			link_[link::dmtu] = "dmtu";
			link_[link::enta] = "enta";
			link_[link::hasi] = "hasi";
			link_[link::hmem] = "hmem";
			link_[link::holo] = "holo";
			link_[link::hprt] = "hprt";
			link_[link::hsub] = "hsub";
			link_[link::inst] = "inst";
			link_[link::mero] = "mero";
			link_[link::mmem] = "mmem";
			link_[link::mprt] = "mprt";
			link_[link::msub] = "msub";
			link_[link::sim] = "sim";
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
		typedef std::function<bool(std::string const&)> callback_type;

		// 個別指定によるアクセス
		bool operator()(
			std::string const& word,
			link::enum_t lnk,
			pos::enum_t pos,
			lang::enum_t lng,
			callback_type const& func
		) {
			// linkで分岐
			// synsとcoorは別個のクエリ
			switch (lnk) {
				case link::syns: {
					return synonym(word, pos, lng, func);
				}
				case link::coor: {
					return coordinate(word, pos, lng, func);
				}
				default: {
					return get_word_by_link(word, lnk, pos, lng, func);
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
		bool synonym(std::string const& word, pos::enum_t pos, lang::enum_t lng, callback_type const& func) {
			char const* query = "SELECT word.lemma FROM word, sense WHERE sense.synset IN ( SELECT sense.synset FROM word, sense WHERE word.lemma=? AND word.pos=? AND word.lang=? AND sense.wordid = word.wordid ) AND sense.wordid = word.wordid AND word.pos=? AND word.lang=?;";
			return run_query_type_1(query, word, pos, lng, func);
		}

/*
SELECT word.lemma
FROM synlink, word, sense
WHERE
    synlink.link = 'hypo'
    AND synlink.synset1 IN (
        SELECT synlink.synset2
        FROM word, sense, synlink
        WHERE word.lemma = ?
            AND word.pos = ?
            AND word.lang = ?
            AND sense.wordid = word.wordid
            AND synlink.synset1 = sense.synset
            AND synlink.link = 'hype'
    )
    AND sense.synset = synlink.synset2
    AND word.wordid = sense.wordid
    AND word.pos = ?
    AND word.lang = ?;
*/
		// 1. wordからwordidを取得
		// 2. wordidが属するsynsetを取得
		// 3. synsetの上位語synsetを取得
		// 4. 上位語synsetの下位語synset(== 兄弟語synset)を取得
		// 5. 兄弟語synsetに属する単語を取得
		bool coordinate(std::string const& word, pos::enum_t pos, lang::enum_t lng, callback_type const& func) {
			char const* query = "SELECT word.lemma FROM synlink, word, sense WHERE synlink.link = 'hypo' AND synlink.synset1 IN ( SELECT synlink.synset2 FROM word, sense, synlink WHERE word.lemma=? AND word.pos=? AND word.lang=? AND sense.wordid = word.wordid AND synlink.synset1 = sense.synset AND synlink.link = 'hype' ) AND sense.synset = synlink.synset2 AND word.wordid = sense.wordid AND word.pos=? AND word.lang=?;";
			return run_query_type_1(query, word, pos, lng, func);
		}

		bool hypernym(std::string const& word, pos::enum_t pos, lang::enum_t lng, callback_type const& func) {
			return get_word_by_link(word, link::hype, pos, lng, func);
		}
		bool hyponym(std::string const& word, pos::enum_t pos, lang::enum_t lng, callback_type const& func) {
			return get_word_by_link(word, link::hypo, pos, lng, func);
		}

	private:

/*
SELECT word.lemma
FROM word, sense
WHERE
    sense.synset IN (
        SELECT synlink.synset2
        FROM word, sense, synlink
        WHERE word.lemma = ?
            AND word.pos = ?
            AND word.lang = ?
            AND sense.wordid = word.wordid
            AND synlink.synset1 = sense.synset
            AND synlink.link = ?
    )
    AND word.wordid = sense.wordid
    AND word.pos = ?
    AND word.lang = ?;
*/
		// wordと、linkで関係する語をコールバック
		bool get_word_by_link(
			std::string const& word,
			link::enum_t lnk,
			pos::enum_t pos,
			lang::enum_t lng,
			callback_type const& func
		) {
			char const* query = "SELECT word.lemma FROM word, sense WHERE sense.synset IN ( SELECT synlink.synset2 FROM word, sense, synlink WHERE word.lemma=? AND word.pos=? AND word.lang=? AND sense.wordid = word.wordid AND synlink.synset1 = sense.synset AND synlink.link=? ) AND word.wordid = sense.wordid AND word.pos=? AND word.lang=?;";
			// SQLクエリを実行
			stmt_type stmt = db_.prepare(query);
			if (!stmt) {
				return false;
			}
			// データをbind
			bool result = stmt.bind(word, pos_[pos], lang_[lng], link_[lnk], pos_[pos], lang_[lng]);
			if (!result) {
				return false;
			}

			// 返されるデータを参照
			stmt.each( [&func](row_type const& row) -> bool {
				// funcの返り値をそのまま返す
				if (auto p = row.as_string(0)) return func( *p );
				return false;
			});

			// 最後までループしてたらtrue
			return stmt.is_done();
		}

		// "word, pos_[pos], lang_[lng], pos_[pos], lang_[lng]"をbindするタイプのクエリを実行
		bool run_query_type_1(
			char const* query,
			std::string const& word,
			pos::enum_t pos,
			lang::enum_t lng,
			callback_type const& func
		) {
			// SQLクエリを実行
			stmt_type stmt = db_.prepare(query);
			if (!stmt) {
				return false;
			}
			// データをbind
			bool result = stmt.bind(word, pos_[pos], lang_[lng], pos_[pos], lang_[lng]);
			if (!result) {
				return false;
			}

			// 返されるデータを参照
			stmt.each( [&func](row_type const& row) -> bool {
				// funcの返り値をそのまま返す
				if (auto p = row.as_string(0)) return func( *p );
				return false;
			});

			// 最後までループしてたらtrue
			return stmt.is_done();
		}

	};

}
