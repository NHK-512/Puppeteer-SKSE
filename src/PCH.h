#pragma once
#include <iostream>
#include "SKSE/SKSE.h"
#include "RE/Skyrim.h"
#include <REL/Relocation.h>
#include "AIManager.h"
#define DLLEXPORT __declspec(dllexport)
//using namespace std::literals;
namespace logger = SKSE::log;
//
//namespace stl
//{
//	using namespace SKSE::stl;
//
//	template <class F, std::size_t idx, class T>
//	void write_vfunc() 
//	{
//		REL::Relocation vtbl{ F::VTABLE[0] };
//		T::func = vtbl.write_vfunc(T::idx, T::thunk);
//	}
//
//	//template <std::size_t idx, class T>
//	//void write_vfunc(REL::VariantID id)
//	//{
//	//	REL::Relocation<std::uintptr_t> vtbl{ id };
//	//	T::func = vtbl.write_vfunc(idx, T::thunk);
//	//}
//	// 
//	//template <typename T>
//	//constexpr auto write_thunk_call() noexcept
//	//{
//	//	SKSE::AllocTrampoline(14);
//	//	auto& trampoline{ SKSE::GetTrampoline() };
//	//	T::func = trampoline.write_call<5>(T::address, T::Thunk);
//	//}
//}