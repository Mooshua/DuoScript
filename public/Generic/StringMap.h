// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_STRINGMAP_H
#define DUOSCRIPT_STRINGMAP_H

#include <string>
#include <amtl/am-hashmap.h>

namespace ke
{

	static int Hash(const char *string)
	{
		int x = 0b10101010;
		char c = *string;

		while ((c = *string++)) {
			x = x + c;
			//	XorShift
			x ^= x << 13;
			x ^= x >> 17;
			x ^= x << 5;
		}

		return x;
	}

	static bool Equals(const char *a, const char *b)
	{
		return strcmp(a, b) == 0;
	}

	class StringKey
	{
	public:
		StringKey(const char *str)
		: _str(str)
		//, _hash(Hash(str))
		{

		}
	public:
		inline operator const char*() { return _str; }
		int hash() {
			if (_hash == -1)
				_hash = Hash(_str);
			return _hash;
		}

	protected:
		const char* _str;
		int _hash = -1;
	};

	class StringHashPolicy
	{
	public:
		static inline bool matches(StringKey lookup, StringKey key) {
			//if (lookup.hash() != key.hash())
			//	return false;

			return Equals(lookup, key);
		}
		static inline uint32_t hash(StringKey key) {
			return key.hash();
		}
	};

	template <typename T>
	class StringMap : public ke::HashMap<StringKey, T, StringHashPolicy>
	{

	};

}
#endif //DUOSCRIPT_STRINGMAP_H
