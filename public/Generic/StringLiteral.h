// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_STRINGLITERAL_H
#define DUOSCRIPT_STRINGLITERAL_H

#include <utility>
#include <algorithm>

template<size_t N>
struct StringLiteral {
	constexpr StringLiteral(const char (&str)[N]) {
		std::copy(str, str + N, value);
	}

	char value[N];
};

#endif //DUOSCRIPT_STRINGLITERAL_H
