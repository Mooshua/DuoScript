// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_AWESOMEHOOK_H
#define DUOSCRIPT_AWESOMEHOOK_H

#include <type_traits>
#include "AwesomeHook/Templating.h"

#include "ISmmAPI.h"
#include "sourcehook.h"

namespace SourceHook
{
	namespace detail
	{
		//	build the paramsPassInfo for a prototype definition
		class PrototypeBuilderFunctor
		{
		public:
			constexpr PrototypeBuilderFunctor(PassInfo* p)
			: params(p)
			{

			}
			PassInfo* params;

			template<int Index, typename Now>
			constexpr void step()
			{
				//	Note: first param is always the thisptr!
				params[Index + 1] = __SH_GPI(Now);
			}
		};

		//	Since void is a zero-sized type,
		//	we have to break out all writes to the zero-sized type to this.
		template<ISourceHook** SourceHook, typename Result, typename... Args>
		struct BaseMethodInvoker
		{
		public:
			typedef Result (EmptyClass::*EmptyDelegate)(Args...);
		};

		template<ISourceHook** SourceHook, typename IDelegate, typename... Args>
		struct VoidMethodInvoker
		{
		public:
			typedef std::bool_constant<false> has_return;
			typedef BaseMethodInvoker<SourceHook,void, Args...> base;

			static void Invoke(IDelegate* delegate, void* result, Args... args)
			{
				delegate->Call(args...);
			}

			static void Original(EmptyClass* self, typename base::EmptyDelegate mfp, void* result, Args... args)
			{
				(self->*mfp)(args...);
			}

			static void Dereference(const void* arg)
			{
				return;
			}
		};

		template<ISourceHook** SourceHook, typename IDelegate, typename Result, typename... Args>
		struct ReturningMethodInvoker
		{
		public:
			typedef std::bool_constant<true> has_return;
			typedef BaseMethodInvoker<SourceHook, void, Result, Args...> base;


			static void Invoke(IDelegate* delegate, Result* result, Args... args)
			{
				*result = delegate->Call(args...);
			}

			static void Original(EmptyClass* self, typename base::EmptyDelegate mfp, Result* result, Args... args)
			{
				*result = (self->*mfp)(args...);
			}

			static Result Dereference(const Result* arg)
			{
				return *arg;
			}
		};
	}

	template<ISourceHook** SH, typename Interface, auto Method, typename Result, typename... Args>
	class Hook
	{
		friend Hook* Make();

		//typedef Hook<SH, Interface, Method, Result, Args...> Self;
		typedef Result (Interface::*MemberMethod)(Args...);
		typedef fastdelegate::FastDelegate<Result, Args...> Delegate;
		typedef decltype(Method) MethodType;

		static_assert( std::is_same<MethodType, MemberMethod>::type::value, "Mismatched template parameters!" );

		::SourceHook::MemFuncInfo MFI;
		::SourceHook::IHookManagerInfo *HI;
		::SourceHook::ProtoInfo Proto;

		static Hook Instance;

	private:
		constexpr Hook()
		{
			//	build protoinfo
			Proto.numOfParams = sizeof...(Args) + 1;
			Proto.convention = ProtoInfo::CallConv_Unknown;

			if constexpr (std::is_void_v<Result>) {
				Proto.retPassInfo = {0, 0, 0};
			} else {
				Proto.retPassInfo = __SH_GPI(Result);
			}

			//	Iterate over the args... type pack
			auto paramsPassInfo = new PassInfo[sizeof...(Args) + 1];
			paramsPassInfo[0] = { 1, 0, 0 };
			Proto.paramsPassInfo = paramsPassInfo;

			detail::PrototypeBuilderFunctor argsBuilder(paramsPassInfo);
			for_each_template_nullable<detail::PrototypeBuilderFunctor, Args...>(&argsBuilder);


			//	Build the backwards compatible paramsPassInfoV2 field
			Proto.retPassInfo2 = {0, 0, 0, 0};
			auto paramsPassInfo2 = new PassInfo::V2Info[sizeof...(Args) + 1];
			Proto.paramsPassInfo2 = paramsPassInfo2;

			for (int i = 0; i /* lte to include thisptr! */ <= sizeof...(Args); ++i) {
				//	todo: help
			}
		}

	public:
		static constexpr Hook* Make()
		{
			Hook::Instance = Hook();

			return &Hook::Instance;
		}

	public:	//	Public Interface
		int Add(PluginId id, Interface* iface, bool post, Delegate handler, ::SourceHook::ISourceHook::AddHookMode mode = ISourceHook::AddHookMode::Hook_Normal)
		{
			using namespace ::SourceHook;
			MemFuncInfo mfi = {true, -1, 0, 0};
			GetFuncInfo((static_cast<void (Interface::*)(bool, bool, bool)>(Method)), mfi);
			if (mfi.thisptroffs < 0 || !mfi.isVirtual)
				return false;

			CMyDelegateImpl* delegate = new CMyDelegateImpl(handler);
			return (*SH)->AddHook(id, mode, iface, mfi.thisptroffs, Hook::HookManPubFunc, delegate, post);
		}

		int Remove(PluginId id, Interface* iface, bool post, Delegate handler)
		{
			using namespace ::SourceHook;
			MemFuncInfo mfi = {true, -1, 0, 0};
			GetFuncInfo((static_cast<void (Interface::*)(bool, bool, bool)>(Method)), mfi);

			//	Temporary delegate for .IsEqual() comparison.
			CMyDelegateImpl temp(handler);
			return (*SH)->RemoveHook(id, iface, mfi.thisptroffs, Hook::HookManPubFunc, &temp, post);
		}

	protected:
		static int HookManPubFunc(bool store, ::SourceHook::IHookManagerInfo *hi)
		{
			//	Build the MemberFuncInfo for the hooked method
			GetFuncInfo<MemberMethod>(static_cast<MemberMethod>(Method), Instance.MFI);

			if ((*SH)->GetIfaceVersion() != 5 || (*SH)->GetImplVersion() < 5)
				return 1;

			if (store)
				Instance.HI = hi;

			if (hi) {
				//	Build a memberfuncinfo for our hook processor.
				MemFuncInfo our_mfi = {true, -1, 0, 0};
				GetFuncInfo(&Hook::Func, our_mfi);

				void* us = reinterpret_cast<void **>(reinterpret_cast<char *>(&Instance) + our_mfi.vtbloffs)[our_mfi.vtblindex];
				hi->SetInfo(1, Instance.MFI.vtbloffs, Instance.MFI.vtblindex, &Instance.Proto, us);
			}

			return 0;
		}

		struct IMyDelegate : ::SourceHook::ISHDelegate
		{
			virtual void Call(Args... args) = 0;
		};

		struct CMyDelegateImpl : IMyDelegate
		{
			Delegate _delegate;

			CMyDelegateImpl(Delegate deleg) : _delegate(deleg)
			{}
			virtual~CMyDelegateImpl() {}
			void Call(Args... args) { _delegate(args...); }
			void DeleteThis() { delete this; }
			bool IsEqual(ISHDelegate *pOtherDeleg) { return _delegate == static_cast<CMyDelegateImpl *>(pOtherDeleg)->_delegate; }

		};

	protected:

		typedef typename if_else<
				std::is_void<Result>::value,
				detail::VoidMethodInvoker<SH, IMyDelegate, Args...>,
				detail::ReturningMethodInvoker<SH, IMyDelegate, Result, Args...>
		>::type Invoker;

		typedef typename if_else<
				std::is_void<Result>::value,
				void*,
				Result
		>::type VoidSafeResult;

		virtual Result Func(Args... args)
		{
			using namespace ::SourceHook;
			void *ourvfnptr = reinterpret_cast<void *>(
					*reinterpret_cast<void ***>(reinterpret_cast<char *>(this) + MFI.vtbloffs) + MFI.vtblindex);
			void *vfnptr_origentry;

			META_RES status = MRES_IGNORED;
			META_RES prev_res;
			META_RES cur_res;

			VoidSafeResult original_ret;
			VoidSafeResult override_ret;
			VoidSafeResult current_ret;

			IMyDelegate *iter;
			IHookContext *context = (*SH)->SetupHookLoop(
					HI,
					ourvfnptr,
					reinterpret_cast<void *>(this),
					&vfnptr_origentry,
					&status,
					&prev_res,
					&cur_res,
					&original_ret,
					&override_ret);

			//	Call all pre-hooks
			prev_res = MRES_IGNORED;
			while ((iter = static_cast<IMyDelegate *>(context->GetNext()))) {
				cur_res = MRES_IGNORED;
				Invoker::Invoke(iter, &current_ret, args...);
				prev_res = cur_res;

				if (cur_res > status)
					status = cur_res;

				if (cur_res >= MRES_OVERRIDE)
					*reinterpret_cast<VoidSafeResult *>(context->GetOverrideRetPtr()) = current_ret;
			}

			//	Call original method
			if (status != MRES_SUPERCEDE && context->ShouldCallOrig()) {
				typename Invoker::base::EmptyDelegate original;
				reinterpret_cast<void **>(&original)[0] = vfnptr_origentry;
				Invoker::Original( reinterpret_cast<EmptyClass*>(this), original, original_ret, args...);
			} else {
				original_ret = override_ret;
			}

			//	Call all post-hooks
			prev_res = MRES_IGNORED;
			while ((iter = static_cast<IMyDelegate *>(context->GetNext()))) {
				cur_res = MRES_IGNORED;
				Invoker::Invoke(iter, &current_ret, args...);
				prev_res = cur_res;

				if (cur_res > status)
					status = cur_res;

				if (cur_res >= MRES_OVERRIDE)
					*reinterpret_cast<VoidSafeResult *>(context->GetOverrideRetPtr()) = current_ret;
			}

			const VoidSafeResult* result_ptr = reinterpret_cast<const VoidSafeResult *>((status >= MRES_OVERRIDE)
																			  ? context->GetOverrideRetPtr()
																			  : context->GetOrigRetPtr());

			(*SH)->EndContext(context);

			return Invoker::Dereference(result_ptr);
		}

	};

	//	You're probably wondering what the hell this does.
	//	https://stackoverflow.com/questions/11709859/how-to-have-static-data-members-in-a-header-only-library/11711082#11711082
	//	I hate C++.
	template<ISourceHook** SH, typename Interface, auto Method, typename Result, typename... Args>
	Hook<SH, Interface, Method, Result, Args...> Hook<SH, Interface, Method, Result, Args...>::Instance;

}

#endif //DUOSCRIPT_AWESOMEHOOK_H
