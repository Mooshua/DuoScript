// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_TEMPLATING_H
#define DUOSCRIPT_TEMPLATING_H

#include "AwesomeHook/Common.h"

namespace SourceHook
{
	namespace detail
		{
			template<class Functor, int Index, typename Now, typename... Rest>
			constexpr void KA_INLINE for_each_template_impl(Functor *f)
		{
			f->template step<Index, Now>();

			if constexpr (sizeof...(Rest) > 0) {
			for_each_template_impl<Functor, Index + 1, Rest...>(f);
		}
	}
	}

	///	Iterate over a varardic template, invoking a functor
	///	for each vararg argument.
	template<class Functor, typename First, typename... Rest>
	constexpr void KA_INLINE for_each_template(Functor* f)
	{
	detail::for_each_template_impl<Functor, 0, First, Rest...>(f);
	}

	///	Iterate over a varardic template, invoking a functor
	///	for each vararg argument. Does not invoke the functor
	///	if there are no type arguments passed.
	template<class Functor, typename... Rest>
	constexpr void KA_INLINE for_each_template_nullable(Functor* f)
	{
	for_each_template<Functor, Rest...>(f);
	}

	template<class Functor>
	constexpr void KA_INLINE for_each_template_nullable(Functor* f)
	{
	//	Empty varargs
	}

	template <bool Test, class Yes = void, class No = void>
	struct if_else {
	public:
		typedef No type;
	};

	template <class Yes, class No>
	struct if_else<true, Yes, No> {
	public:
		typedef Yes type;
	};

	template<typename Builder>
	class empty_daisy_chain
	{
	public:
		template<typename... Existing>
		static KA_INLINE void step(Builder *builder, int idx, Existing... args)
		{
			std::abort();
		}
	};

	///	Welcome to hell.
	template<typename Builder, typename Now = void, typename... Args>
	class daisy_chain
	{
		friend class daisy_chain;
	public:
		static KA_INLINE void run(Builder *builder)
		{
			first_step(builder);
		}

	protected:

		static KA_INLINE void first_step(Builder* builder)
		{
			Now now = builder->template fetch<Now>(0);

			downstream.template step<Now>(builder, 1, now);
		}

		///	Step down the daisy chain, adding our current
		template<typename... Existing>
		static KA_INLINE void step(Builder *builder, int idx, Existing... args)
		{
			Now now = builder->template fetch<Now>(idx);

			if constexpr (sizeof...(Args) == 0) {
				//	We are at the end of this daisy chain,
				//	and this->downstream does not exist.
				//	finish the invocation
				builder->finish(args..., now);
			} else {
				downstream.template step<Existing..., Now>(builder, idx + 1, args..., now);
			}
		}

	protected:

		///	The downstream daisy chain that continues the .step invocations
		///	We use enable_if_t to disable the daisy chain once we run out of arguments
		static typename SourceHook::if_else<
				std::integral_constant<bool, sizeof...(Args) != 0>::value,
				daisy_chain<Builder, Args...>,
				empty_daisy_chain<Builder>>::type downstream;
	};
}

#endif //DUOSCRIPT_TEMPLATING_H
