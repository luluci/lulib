#pragma once
#pragma warning(disable : 4819)

#include <string>
#include <lulib/unicode/traits/codepoint.hpp>

namespace lulib { namespace unicode_detail { namespace traits {

	struct utf8 {
		typedef char        char_type;
		typedef std::string string_type;

		// �R�[�h�|�C���g���擾����
		// �C�e���[�^�͎��̕����̊J�n�ʒu�ɂ܂�forward�����
		// First byte �̉��
		//    : First byte �͐擪�� "10" �����Ȃ�
		template<typename Iterator>
		static codepoint get_code_point(Iterator &it, Iterator end) {
			codepoint result;
			if (it == end) return result;

			std::uint32_t cp, rest;

			// ���1bit��0�Ȃ�A1�o�C�g����
			if ((*it & 0x80) == 0x00) {
				cp = (*it & 0x7F);  // �L��bit(����7bit)���擾
				rest = 0;
				result.bit_size  = 7;
				result.byte_size = 1;
			}
			// ���3bit��110 (110X XXXX) �Ȃ�A2�o�C�g����
			else if ((*it & 0xE0) == 0xC0) {
				cp = (*it & 0x1F);  // �L��bit(����3bit)���擾
				rest = 1;
				result.bit_size  = 11;
				result.byte_size = 2;
			}
			// ���4bit��1110 (1110 XXXX) �Ȃ�A3�o�C�g����
			else if ((*it & 0xF0) == 0xE0) {
				cp = (*it & 0x0F);  // �L��bit(����3bit)���擾
				rest = 2;
				result.bit_size  = 16;
				result.byte_size = 3;
			}
			// ���5bit��11110 (1111 0XXX) �Ȃ�A4�o�C�g����
			else if ((*it & 0xF8) == 0xF0) {
				cp = (*it & 0x07);  // �L��bit(����3bit)���擾
				rest = 3;
				result.bit_size  = 21;
				result.byte_size = 4;
			}
			// �s���ȃV�[�P���X
			else {
				return result;
			}
			// ASCII�łȂ��L��bit��0: �璷�ȕ\���̂��߁A�s���ȃV�[�P���X
			if (result.bit_size != 7 && cp == 0) {
				return result;
			}

			// �c��byte-sequence����R�[�h�|�C���g�����߂�
			++it;
			result.value = get_code_point_impl(it, end, cp, rest);
			return result;
		}
		// continuation bytes �̉��
		//     : continuation byte �͕K�� "10" �Ŏn�܂�
		//   cp: �擪�o�C�g�̗L���f�[�^
		// rest: �c��o�C�g
		template<typename Iterator>
		static std::size_t get_code_point_impl(
			Iterator &it, Iterator end, std::uint32_t cp, std::uint32_t rest
		) {
			for (; rest > 0 && it != end && (*it & 0xC0) == 0x80; --rest) {
				cp = ((cp << 6) | (*it & 0x3F));  // ����byte�̉���6bit��A��
				++it;
			}
			// start byte ���������� continuation bytes ���Ȃ�
			if (rest > 0) return 0;
			return cp;
		}

		// code-point ����G���R�[�h�ɕK�v�� bit/byte����Ԃ�
		static codepoint get_size(std::uint32_t cp) {
			codepoint result;

			// 1byte: U+00 .. U+7F
			if (0x00000000 <= cp && cp <= 0x0000007F) {
				result.bit_size  = 7;
				result.byte_size = 1;
			}
			// 2byte: U+0080 .. U+ 07FF
			else if (0x00000080 <= cp && cp <= 0x000007FF) {
				result.bit_size  = 11;
				result.byte_size = 2;
			}
			// 3byte: U+0800 .. U+FFFF
			else if (0x00000800 <= cp && cp <= 0x0000FFFF) {
				result.bit_size  = 16;
				result.byte_size = 3;
			}
			// 4byte: U+010000 .. U+1FFFFF
			else if (0x00010000 <= cp && cp <= 0x001FFFFF) {
				result.bit_size  = 21;
				result.byte_size = 4;
			}
			/*
			// invalide sequence
			// 5byte: U+00200000 .. U+03FFFFFF
			else if (0x00200000 <= value_ && value_ <= 0x03FFFFFF) {
				result.bit_size  = 26;
				result.byte_size = 5;
			}
			// 6byte: U+04000000 .. U+7FFFFFFF
			else if (0x04000000 <= value_ && value_ <= 0x7FFFFFFF) {
				result.bit_size  = 31;
				result.byte_size = 6;
				return 6;
			}
			*/
			// error: ���̑�
			else {
				result.bit_size  = 0;
				result.byte_size = 0;
			}

			return result;
		}
		inline static std::size_t get_bit_size(std::uint32_t cp) {
			return get_size(cp).bit_size;
		}
		inline static std::size_t get_byte_size(std::uint32_t cp) {
			return get_size(cp).byte_size;
		}

		static string_type get_string(std::uint32_t value) {
			string_type str;
			std::uint32_t temp;
			codepoint cp = get_size(value);
			str.reserve(cp.byte_size);

			switch (cp.byte_size) {
				case 1: {
					// ��1byte
					str.push_back( static_cast<char_type>(value) );
					break;
				}
				case 2: {
					// 00000000 00000000 00000yyy yyxxxxxx
					// 00000000 00000000 00000111 11000000 => 0x000007C0
					// 00000000 00000000 00000000 00111111 => 0x0000003F
					// 110y yyyy
					// 10xx xxxx
					// ��2byte
					temp = value & 0x000007C0;
					temp >>= 6;
					temp |= 0xC0;
					str.push_back( static_cast<char_type>(temp) );
					// ��1byte
					temp = value & 0x0000003F;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					break;
				}
				case 3: {
					// 00000000 00000000 zzzzyyyy yyxxxxxx
					// 00000000 00000000 11110000 00000000 => 0x0000F000
					// 00000000 00000000 00001111 11000000 => 0x00000FC0
					// 00000000 00000000 00000000 00111111 => 0x0000003F
					// 1110 zzzz
					// 10yy yyyy
					// 10xx xxxx
					// ��3byte
					temp = value & 0x0000F000;
					temp >>= 12;
					temp |= 0xE0;
					str.push_back( static_cast<char_type>(temp) );
					// ��2byte
					temp = value & 0x00000FC0;
					temp >>= 6;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					// ��1byte
					temp = value & 0x0000003F;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					break;
				}
				case 4: {
					// 00000000 000aaazz zzzzyyyy yyxxxxxx
					// 00000000 00011100 00000000 00000000 => 0x001C0000
					// 00000000 00000011 11110000 00000000 => 0x0003F000
					// 00000000 00000000 00001111 11000000 => 0x00000FC0
					// 00000000 00000000 00000000 00111111 => 0x0000003F
					// 1111 0aaa
					// 10zz zzzz
					// 10yy yyyy
					// 10xx xxxx
					// ��4byte
					temp = value & 0x001C0000;
					temp >>= 18;
					temp |= 0xF0;
					str.push_back( static_cast<char_type>(temp) );
					// ��3byte
					temp = value & 0x0003F000;
					temp >>= 12;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					// ��2byte
					temp = value & 0x00000FC0;
					temp >>= 6;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					// ��1byte
					temp = value & 0x0000003F;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					break;
				}
			}

			return str;
		}
	};
}}}// namespace lulib::unicode_detail::traits
