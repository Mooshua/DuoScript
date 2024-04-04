// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_MACRO_H
#define DUOSCRIPT_MACRO_H

#ifdef NOT_COMPILING_DUOSCRIPT
#define DUO_EXPORT extern "C" __declspec(dllimport)
#else
#define DUO_EXPORT extern "C" __declspec(dllexport)
#endif

#endif //DUOSCRIPT_MACRO_H
