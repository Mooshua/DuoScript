// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

#ifndef DUOSCRIPT_STRINGMAP_H
#define DUOSCRIPT_STRINGMAP_H

#include "Vendor/XXHash64.h"
#include "Vendor/Robin/robin_map.h"
#include "Vendor/Robin/robin_growth_policy.h"

#include <string>
#include <amtl/am-hashmap.h>

namespace ke
{

	static constexpr size_t Hash(std::string_view string)
	{
		XXHash64 hash = XXHash64(0b10101010);
		hash.add(string.data(), string.size());
		return hash.hash();
	}


	class StringKey
	{
	public:
		constexpr StringKey(const char *str)
		{
			if (str != nullptr)
				_str = std::string_view(str);
		}

		constexpr StringKey(const std::string_view str)
				: _str(str)
		{
		}
	public:
		inline operator const char*() { return _str.data(); }
		inline bool operator==(const StringKey other) const {
			if (_str.length() != other._str.length())
				return false;

			return _str.compare(other._str) == 0;
		}

		size_t hash() {
			if (_hash == -1)
				_hash = Hash(_str);
			return _hash;
		}
		size_t hash() const
		{
			return Hash(_str);
		}

	protected:
		std::string_view _str;
		size_t _hash = -1;
	};

	class StringHashPolicy
	{
	public:
		static inline bool matches(StringKey lookup, StringKey key) {
			//if (lookup.hash() != key.hash())
			//	return false;

			return lookup == key;
		}
		static inline uint32_t hash(StringKey key) {
			return key.hash();
		}

		//	For robinhash
		size_t operator()(const StringKey &key) const noexcept
		{
			return key.hash();
		}
	};

	template <typename T>
	class StringMap : public ke::HashMap<StringKey, T, StringHashPolicy>
	{

	};

	template <class T, class Key = StringKey, class Hash = StringHashPolicy,
			class KeyEqual = std::equal_to<Key>,
			class Allocator = std::allocator<std::pair<Key, T>>,
			bool StoreHash = false>
	using FastStringMap = tsl::robin_map<Key, T, Hash, KeyEqual, Allocator, StoreHash,
			tsl::rh::power_of_two_growth_policy<4>>;

}
#endif //DUOSCRIPT_STRINGMAP_H
