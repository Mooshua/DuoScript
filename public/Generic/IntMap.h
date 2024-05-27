// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

#ifndef DUOSCRIPT_INTMAP_H
#define DUOSCRIPT_INTMAP_H

#include <amtl/am-hashmap.h>

namespace ke
{

	static size_t Hash(int index)
	{
		index ^= index << 13;
		index ^= index >> 7;
		index ^= index << 5;

		return index;
	}

	class IntHashPolicy
	{
	public:
		static inline bool matches(int lookup, int key)
		{
			return lookup == key;
		}

		static inline uint32_t hash(int key)
		{
			return Hash(key);
		}
	};

	template<typename T>
	class IntMap : public ke::HashMap<int, T, IntHashPolicy>
	{

	};
}

#endif //DUOSCRIPT_INTMAP_H
