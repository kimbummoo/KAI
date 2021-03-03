/*
	Copyright (c) 2020 Kim,Bummoo
	Read KAI.h
*/
#pragma once

#include "../pch.h"
#include <amp.h>
#include "_context_KAI.h"

#include <iostream>
#include <typeinfo>
template <typename Type>
inline void CONSOL_MESSAGE(Type* This, const char* Message)
{
	//std::cout << typeid(Type).name() << " : " << Message << std::endl;
}

namespace KAI
{
	template <typename ConType, typename ValType>
	class RefController;

	template <typename Type>
	class Controller;

	namespace VAL {
		template <typename Type>
		class Val;
	}
	namespace FLOW {
		namespace STACK {
			template <typename Type>
			class Stack;
		}
		template <typename Type>
		class Flow;
	}
	namespace NODE {
		template <typename Type>
		class Node;
	}

	static int GetDimentionSize(std::vector<int>& Dimention)
	{
		if (Dimention.size() == 0)
			return 0;
		int Size = 1;
		for (auto& Elum : Dimention)
			Size *= Elum;
		return Size;
	}
}