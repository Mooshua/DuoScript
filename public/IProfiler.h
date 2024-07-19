// Copyright (c) 2024 DuoScript Contributors.
// This file is licensed under the Lesser GPL (see public/LICENSE.md), and defines an
// interface that may be linked to without distributing the linkee under a GPL license.

#ifndef DUOSCRIPT_IPROFILER_H
#define DUOSCRIPT_IPROFILER_H

#include <Tracy.hpp>

//	Undefine unsafe macros for metamod
#undef ZoneNamed
#undef ZoneNamedS
#undef ZoneNamedNS
#undef ZoneNamedCS
#undef ZoneNamedNCS

//	Ugh... Well, thanks stackoverflow
//	https://stackoverflow.com/questions/1597007/creating-c-macro-with-and-line-token-concatenation-with-positioning-macr
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define DuoScope(name) ZoneTransientN( TOKENPASTE2( __scope_at_, __LINE__ ), #name, true )
#define DuoScopeDynamic(name, length) \
	tracy::ScopedZone TOKENPASTE2( __scope_at_, __LINE__ ) ( __LINE__, __FILE__, strlen( __FILE__ ), __FUNCTION__, strlen( __FUNCTION__ ), name, length, true )

namespace duo
{
	class Frame
	{
	public:
		inline Frame(const char* name)
		: _name(name)
		{
			tracy::Profiler::SendFrameMark( this->_name, tracy::QueueType::FrameMarkMsgStart );
		}
		inline ~Frame()
		{
			tracy::Profiler::SendFrameMark( this->_name, tracy::QueueType::FrameMarkMsgEnd );
		}

	protected:
		const char* _name;
	};

	class Plot
	{
	public:
		Plot(const char* name, tracy::PlotFormatType format)
		: _name(name)
		{
			tracy::Profiler::ConfigurePlot( _name, format, false, true,0 );
		}

		void Set(int64_t data)
		{
			tracy::Profiler::PlotData(_name, data);
		}

		void SetFloat(double data)
		{
			tracy::Profiler::PlotData(_name, data);
		}
	protected:
		const char* _name;
	};

}

#endif //DUOSCRIPT_IPROFILER_H
