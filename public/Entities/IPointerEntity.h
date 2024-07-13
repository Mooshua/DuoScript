// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

#ifndef DUOSCRIPT_IPOINTERENTITY_H
#define DUOSCRIPT_IPOINTERENTITY_H

///	@brief An entity type that can be turned into a pointer
///
///	This should be used when entities wrap an underlying game object
///	so that modders can get a pointer to the underlying object.
class IPointerEntity
{
	void* AsPointer();
};

#endif //DUOSCRIPT_IPOINTERENTITY_H
