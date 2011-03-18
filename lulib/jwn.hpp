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
			link::enum_t link,
			pos::enum_t pos,
			lang::enum_t lng,
			callback_type const& func
		) {
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
            AND synlink.link = 'hype'
    )
    AND word.wordid = sense.wordid
    AND word.pos = ?
    AND word.lang = ?;
*/
		bool hypernym(std::string const& word, pos::enum_t pos, lang::enum_t lng, callback_type const& func) {
			char const* query = "SELECT word.lemma FROM word, sense WHERE sense.synset IN ( SELECT synlink.synset2 FROM word, sense, synlink WHERE word.lemma=? AND word.pos=? AND word.lang=? AND sense.wordid = word.wordid AND synlink.synset1 = sense.synset AND synlink.link = 'hype' ) AND word.wordid = sense.wordid AND word.pos=? AND word.lang=?;";
			return run_query_type_1(query, word, pos, lng, func);
		}

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
            AND synlink.link = 'hypo'
    )
    AND word.wordid = sense.wordid
    AND word.pos = ?
    AND word.lang = ?;
*/
		bool hyponym(std::string const& word, pos::enum_t pos, lang::enum_t lng, callback_type const& func) {
			char const* query = "SELECT word.lemma FROM word, sense WHERE sense.synset IN ( SELECT synlink.synset2 FROM word, sense, synlink WHERE word.lemma=? AND word.pos=? AND word.lang=? AND sense.wordid = word.wordid AND synlink.synset1 = sense.synset AND synlink.link = 'hypo' ) AND word.wordid = sense.wordid AND word.pos=? AND word.lang=?;";
			return run_query_type_1(query, word, pos, lng, func);
		}

	private:
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
