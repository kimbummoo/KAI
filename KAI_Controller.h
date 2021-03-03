/*
	Copyright (c) 2020 Kim,Bummoo
	Read KAI.h
*/
#pragma once
#include "_include.h"
#include "KAI_Val.h"
#include "KAI_Flow.h"
#include "KAI_Node.h"

namespace KAI
{
	template <typename ConType, typename ValType>
	class RefController : public KAIRefController<ConType, ValType>
	{
	protected:
		RefController(KAI::Controller<ValType>* Owner)
		{
			CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
			this->Owner = Owner;
		}
	public:
		virtual void DESTRUCTOR() override
		{
			CONSOL_MESSAGE(this, "DESTRUCTOR is called");
			ConType* Ptr;
			while (this->Map.begin() != this->Map.end())
			{
				Ptr = this->Map.begin()->first;
				this->Map.erase(this->Map.begin()); //음.. 아직도 모르는게 많구만.
				Ptr->DESTRUCTOR();
			}
			delete this;
		}

		virtual void Add(ConType* Target) override
		{
			this->Map.insert_or_assign(Target, 0);
		}
		//		REFCONTROLLER
		// ptr을 delete할 경우 자동으로 호출됩니다. 따로 손댈 필요는 없습니다.
		virtual void Remove(ConType* Target) override
		{
			this->Map.erase(Target);
		}

		friend Controller<ValType>;
		friend KAI::VAL::Val<ValType>;
		friend KAI::FLOW::Flow<ValType>;
		friend KAI::NODE::Node<ValType>;
	};
	template <typename Type>
	class Controller : public KAIController<Type>
	{
	public:
		Controller()
		{
			CONSOL_MESSAGE(this, "CONSTRUCTOR is called");

			this->Flows = new RefController<KAIFlow<Type>, Type>(this);
			this->Nodes = new RefController<KAINode<Type>, Type>(this);
			this->Vals = new RefController<KAIVal<Type>, Type>(this);

			this->MainFlow = newFlow();
		}
		virtual void DESTRUCTOR() override
		{
			CONSOL_MESSAGE(this, "DESTRUCTOR is called");

			this->DestroyedState = true;

			this->Flows->DESTRUCTOR();
			this->Nodes->DESTRUCTOR();
			this->Vals->DESTRUCTOR();

			delete this;
		}
	protected:
		virtual void RememberStack(KAIStack<Type>* RequiredStack) override
		{
			if (this->LastFlow == nullptr)
			{
				this->LastFlow = newFlow();
			}
			this->LastFlow->Stacks.push_back(RequiredStack);
		}
	public:
		virtual KAIVal<Type>* newVal(int Size) override
		{
			VAL::Val<Type>* New = new VAL::Val<Type>(this, Size);
			this->Vals->Add(New);
			return New;
		}
		virtual KAIFlow<Type>* newFlow() override
		{
			FLOW::Flow<Type>* New = new FLOW::Flow<Type>(this);
			this->Flows->Add(New);
			this->LastFlow = New;
			return New;
		}
		virtual KAINode<Type>* newNode(std::vector<int> Dimention, KAIVal<Type>* ValPtr = NULL) override
		{
			NODE::Node<Type>* New;
			if(ValPtr)
				New = new NODE::Node<Type>(this, Dimention, (KAI::VAL::Val<Type>*)ValPtr);
			else
				New = new NODE::Node<Type>(this, Dimention);

			New->ValIsCreatedByThis = true;
			this->Nodes->Add(New);

			return New;
		}
		friend KAI::VAL::Val<Type>;
		friend KAI::FLOW::Flow<Type>;
		friend KAI::NODE::Node<Type>;
	};
}