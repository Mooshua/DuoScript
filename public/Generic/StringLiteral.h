// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

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
