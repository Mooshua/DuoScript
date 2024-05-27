// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under Affero GPL (see LICENSE.md). This file MUST be
// accessible by any user able to connect to a server running this program.

#ifndef DUOSCRIPT_MACRO_H
#define DUOSCRIPT_MACRO_H

#ifdef NOT_COMPILING_DUOSCRIPT
#define DUO_EXPORT extern "C" __declspec(dllimport)
#else
#define DUO_EXPORT extern "C" __declspec(dllexport)
#endif

#endif //DUOSCRIPT_MACRO_H
