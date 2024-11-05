/* >>)>>)>>) -- M o r b i u s  .  M o r s e  .  M a c h i n e -- (<<(<<(<<
 *
 * Copyright (c) RandomGuyWithoutY, 2024;
 * Copyright (c) Yuri, 2024
 *
 *
 * Compilation:
 *
 *   This program requires a compiler with support for the C11 standard in
 *   order to correctly handle the Unicode encoding of the German characters
 *   of the Morse alphabet defined in this implementation.
 *
 *
 * Review of an actual customer:
 *
 *   "It's Morsing time!"
 *         -- One of our numerous satisfied customers TM
 *
 * Review of an imaginary customer:
 *
 *   "This Morse translator is so amazing! I was finally able
 *    to communicate with the birds outside my window!"
 *                                     -- Pyome
 */

// >>)>>)>>) Headers (<<

	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <stdint.h>
	#include <uchar.h>
	#include <locale.h>


// >>)>>)>>) Macros (<<

	#define TRANS_PLAIN_2_MORSE  0
	#define TRANS_MORSE_2_PLAIN  1


// >>)>>)>>) Data Structures (<<

	typedef struct {
		uint8_t morse: 5;
		uint8_t len:   3;  // if this is 0, we interpret it as a word break
	} MorseSeq;

	// NOTE: we do not talk about MorseSeqL in this house...
	/* typedef struct {
		uint64_t morse_1: 6;
		uint64_t morse_2: 6;
		uint64_t morse_3: 6;
		uint64_t morse_4: 6;
		uint64_t morse_5: 6;
		uint64_t morse_6: 6;
		uint64_t morse_7: 6;
		uint64_t len_1: 3;
		uint64_t len_2: 3;
		uint64_t len_3: 3;
		uint64_t len_4: 3;
		uint64_t len_5: 3;
		uint64_t len_6: 3;
		uint64_t len_7: 3;
	} MorseSeqL; */


// >>)>>)>>) Conversion Tables (<<

	static const char16_t morse_alphabet[] = {
		u' ',
		u'E', u'T', u'I', u'N', u'A', u'M', u'S', u'D', u'R', u'G',
		u'U', u'K', u'W', u'O', u'H', u'B', u'L', u'Z', u'F', u'C',
		u'P', u'Ö', u'V', u'X', u'Ä', u'Q', u'Ü', u'Y', u'J', u'#',
	};


// >>)>>)>>) ASCII - Binary Representation Converters (<<

	size_t morse_symbol_count(const MorseSeq *morse, size_t morse_len)
	{
		size_t symbol_count = 0;

		for (size_t i = 0; i < morse_len; i++) {
			symbol_count += (
				// NOTE: if we encounter a word break (len = 0) or we have to insert
				//       a space to seperate to consecutive morse sequences (i != EOF
				//       && next_sequence != wordbreak), we add one more symbol to the
				//       counter to account for the implicit symbol
				!morse[i].len || (i != morse_len - 1 && morse[i + 1].len)
				? 1
				: 0
			) + morse[i].len;
		}

		return symbol_count;
	}

	// NOTE: returns <= morse_len on success, returns > morse_len on failure in which case the
	//       return value is equivalent to the minimum required buffer size to hold the entire
	//       sequence. err_msg must be checked for a non-NULL value to verify that the entire
	//       input was parsed and not only part of it.
	size_t ascii_2_morse_seq(MorseSeq *morse_dest, size_t morse_len, const char *ascii_src, const char **err_msg)
	{
		size_t   morse_ptr = 0;
		MorseSeq morse_seq = {0};
		int      token;
		void    *ignore_err;

		if (err_msg == NULL) {
			err_msg = (const char **)&ignore_err;
		}

		*err_msg = NULL;

		for (size_t i = 0;; i++) {
			token = 0;

			switch (ascii_src[i]) {
				case '_':
				case '-':
					token = 1;
				case '.': {
					if (morse_seq.len >= 4) {
						*err_msg = "token of invalid length in stream";

						return morse_ptr;
					}

					morse_seq.morse |= token << morse_seq.len++;

					break;
				}
				case '\0':
					token = 1;
				case '/':
					// NOTE: this increment sets token to two (end of stream) in case the string's
					//       end has been reached (marked by a 0x00 character), otherwise to one
					//       (word break)
					token++;
				case ' ':
				case '\t':
				case '\n':
				case '\r': {
					if (morse_seq.len) {
						if (morse_ptr < morse_len) {
							morse_dest[morse_ptr] = morse_seq;
						}

						morse_seq.morse = 0;
						morse_seq.len = 0;
						morse_ptr++;
					}

					if (token == 1) {
						if (morse_ptr < morse_len) {
							morse_dest[morse_ptr] = morse_seq;
						}

						morse_ptr++;
					} else if (token == 2) {
						return morse_ptr;
					}

					break;
				}
				default: {
					*err_msg = "unknown token in stream";

					return morse_ptr;
				}
			}
		}
	}

	size_t morse_seq_2_ascii(const MorseSeq *morse_src, size_t morse_len, char *ascii_dest, size_t ascii_buff_len)
	{
		size_t ascii_ptr = 0;
		size_t ascii_len = morse_symbol_count(morse_src, morse_len) + 1;

		if (ascii_buff_len < ascii_len) {
			return ascii_len;
		}

		for (size_t i = 0; i < morse_len; i++) {
			for (int j = 0; j < morse_src[i].len; j++) {
				ascii_dest[ascii_ptr++] = (morse_src[i].morse & 1 << j) ? '_' : '.';
			}

			if (!morse_src[i].len) {
				ascii_dest[ascii_ptr++] = '/';
			} else if (i != morse_len - 1 && morse_src[i + 1].len) {
				ascii_dest[ascii_ptr++] = ' ';
			}
		}

		ascii_dest[ascii_len - 1] = 0x00;

		return ascii_len;
	}


// >>)>>)>>) Morse Encoders / Decoders (<<

	// NOTE: speed can be greatly improved by using the input chars as indices in a
	//       separate array, saving the overhead of iterating over each character in
	//       the primary array and performing expensive arithmetic to retrieve the
	//       binary morse sequence corresponding to the current character (but i am
	//       too lazy so i am going to leave it at a "fuck you" for now)
	size_t morse_encode(MorseSeq *morse_dest, size_t morse_len, const char *msg_src)
	{
		char16_t  utf16_char;
		size_t    mb_size;
		mbstate_t mb_state;
		size_t    encode_off = 0;

		const size_t msg_len = strlen(msg_src);

		MorseSeq encoded_seq;
		int      seq_found;

		memset(&mb_state, 0, sizeof(mb_state));

		for (size_t i = 0; msg_src[i]; i += mb_size) {
			mb_size = mbrtoc16(&utf16_char, msg_src + i, msg_len - i, &mb_state);

			// NOTE: this normalizes the characters so that every lowercase character
			//       is changed to the uppercase counterpart, and all whitespace characters
			//       are converted to the SP character, or word break in morse terminology
			if (
				utf16_char >= u'a' &&
				utf16_char <= u'z'
			) {
				utf16_char = utf16_char - u'a' + u'A';
			} else switch (utf16_char) {
				// TODO: kill me
				case u'ä': utf16_char = u'Ä'; break;
				case u'ö': utf16_char = u'Ö'; break;
				case u'ü': utf16_char = u'Ü'; break;
				case u'\t':
				case u'\n':
				case u'\v':
				case u'\f':
				case u'\r': utf16_char = u' '; break;
			}

			encoded_seq.len = 0;
			seq_found = 0;

			for (size_t j = 0; j < sizeof(morse_alphabet) / sizeof(morse_alphabet[0]); j++) {
				// NOTE: this increments the sequence length each time j reaches
				//       the value of the next offset
				//
				//       example:
				//           j: 0b0000, len: 0, next_off: 0b0001 => NO INCREMENT  . 1
				//           j: 0b0001, len: 0, next_off: 0b0001 => INCREMENT     #
				//           j: 0b0010, len: 1, next_off: 0b0011 => NO INCREMENT  # 2
				//           j: 0b0011, len: 1, next_off: 0b0011 => INCREMENT     .
				//           j: 0b0100, len: 2, next_off: 0b0111 => NO INCREMENT  .
				//           j: 0b0101, len: 2, next_off: 0b0111 => NO INCREMENT  . 4
				//           j: 0b0110, len: 2, next_off: 0b0111 => NO INCREMENT  .
				//           j: 0b0111, len: 2, next_off: 0b0111 => INCREMENT     #
				//           j: 0b1000, len: 3, next_off: 0b1111 => NO INCREMENT  #
				//           j: 0b1001, len: 3, next_off: 0b1111 => NO INCREMENT  # 8
				//           [...]
				//           [...]
				//
				//       pay attention to the intervall lengths of "." and "#": as you can see,
				//       the lengths of the intervals add up with the amount of characters located
				//       at the offset corresponding to the current sequence length
				if (j == (1 << (encoded_seq.len + 1)) - 1) {
					encoded_seq.len++;
				}

				if (morse_alphabet[j] == utf16_char) {
					// NOTE: we subtract the offset from the index to retrieve the actual
					//       morse sequence (index = offset + sequence, thus sequence =
					//       index - offset)
					encoded_seq.morse = j - ((1 << encoded_seq.len) - 1);
					seq_found = 1;
					break;
				}
			}

			if (seq_found && encode_off < morse_len) {
				morse_dest[encode_off] = encoded_seq;
			}

			encode_off += seq_found;
		}

		return encode_off;
	}

	// NOTE: returns <= plain_buff_size on success, elsewise the minimum size required to hold the decoded string
	size_t morse_decode(const MorseSeq *morse_src, size_t morse_len, char *msg_dest, size_t plain_buff_size)
	{
		char      decode_buff[MB_CUR_MAX];
		size_t    decode_off = 0;
		size_t    mb_size;
		mbstate_t mb_state;

		memset(&mb_state, 0, sizeof(mb_state));

		for (size_t i = 0; i < morse_len; i++) {
			mb_size = c16rtomb(
				decode_buff,
				morse_alphabet[morse_src[i].morse + (1 << morse_src[i].len) - 1],
				&mb_state
			);

			if (decode_off + mb_size <= plain_buff_size) {
				memcpy(msg_dest + decode_off, decode_buff, mb_size);
			}

			decode_off += mb_size;
		}

		if (decode_off < plain_buff_size) {
			msg_dest[decode_off] = 0x00;
		}

		return decode_off + 1;
	}


// >>)>>)>>) CLI Main Function (<<

	int morse_translate(const char *in, char **out, int direction)
	{
		char     *out_buff;
		MorseSeq *morse_buff;

		size_t out_buff_size;
		size_t morse_buff_size;

		int morse_2_plain = direction == TRANS_MORSE_2_PLAIN;  // who did this?!


		if (direction > TRANS_MORSE_2_PLAIN) {
			return 1;
		}

		morse_buff_size = morse_2_plain
		                ? ascii_2_morse_seq(NULL, 0, in, NULL)
		                : morse_encode(NULL, 0, in);

		morse_buff = malloc(morse_buff_size);

		if (morse_buff == NULL) {
			return 1;
		}

		if (morse_2_plain) {
			ascii_2_morse_seq(morse_buff, morse_buff_size, in, NULL);
		} else {
			morse_encode(morse_buff, morse_buff_size, in);
		}

		out_buff_size = morse_2_plain
		              ? morse_decode(morse_buff, morse_buff_size, NULL, 0)
		              : morse_seq_2_ascii(morse_buff, morse_buff_size, NULL, 0);

		out_buff = malloc(out_buff_size);

		if (out_buff == NULL) {
			free(morse_buff);

			return 1;
		}

		if (morse_2_plain) {
			morse_decode(
				morse_buff,
				morse_buff_size,
				out_buff,
				out_buff_size
			);
		} else {
			morse_seq_2_ascii(
				morse_buff,
				morse_buff_size,
				out_buff,
				out_buff_size
			);
		}

		free(morse_buff);

		*out = out_buff;

		return 0;
	}

	int main(int argc, const char **argv)
	{
		char *text_output;
		int   direction;

		setlocale(LC_ALL, "C.utf8");

		if (argc != 3) {
			printf(
				"Usage: morbmorse {encode|decode} <msg>\n"
				"\n"
				"  <msg>: the message to encode/decode\n"
				"\n"
			);

			return 0;
		}

		if (!strcmp(argv[1], "encode")) {
			direction = TRANS_PLAIN_2_MORSE;
		} else if (!strcmp(argv[1], "decode")) {
			direction = TRANS_MORSE_2_PLAIN;
		} else {
			fprintf(stderr, "Unknown command: %s\n", argv[1]);

			return 1;
		}

		if (morse_translate(argv[2], &text_output, direction)) {
			fprintf(stderr, "Error translating message\n");

			return 1;
		}

		fputs(text_output, stdout);

		free(text_output);

		return 0;
	}
