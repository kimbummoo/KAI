/*
	Copyright (c) 2020 Kim,Bummoo
	Read KAI.h
*/
#pragma once
#include "_include.h"
#include "KAI_Flow.h"

namespace KAI
{
	namespace NODE {
		// matrix calculator
		template <typename Type>
		class MatrixOperator : public KAIMatrixOperator<Type>
		{
			MatrixOperator(Node<Type>* TargetNode) : 
				KAIMatrixOperator<Type>(),
				TargetNode(TargetNode)
			{
			}

			friend Node<Type>;
		public:
			virtual KAINode<Type>& Operator_Mul(KAINode<Type>& Node) override
			{
				makeMatrixCalc(Node);
				KAINode<Type>* NewNode = this->TargetNode->getUser()->newNode(this->ReturnDimention);
				KAI::FLOW::STACK::MatrixMul<Type>* New = new KAI::FLOW::STACK::MatrixMul<Type>(TargetNode->getUser()->LastFlow, NewNode, TargetNode, &Node, Front, Overlap, Back);

				TargetNode->end_operator();
				delete this; return *NewNode;
			}
			virtual KAINode<Type>& Operator_Div(KAINode<Type>& Node) override
			{
				makeMatrixCalc(Node);
				KAINode<Type>* NewNode = this->TargetNode->getUser()->newNode(this->ReturnDimention);
				KAI::FLOW::STACK::MatrixDiv<Type>* New = new KAI::FLOW::STACK::MatrixDiv<Type>(TargetNode->getUser()->LastFlow, NewNode, TargetNode, &Node, Front, Overlap, Back);

				TargetNode->end_operator();
				delete this; return *NewNode;
			}

			//	dll function
		public:
			Node<Type>* TargetNode = nullptr;
			int Front = 1, Overlap = 1, Back = 1;
			std::vector<int> ReturnDimention;
			int OverlapDimention = 1;


			void makeMatrixCalc(KAINode<Type>& Node)
			{
				for (int Over = 0, Elum; Over < OverlapDimention; Over++)
					Elum = TargetNode->Dimention[Over], Front *= Elum, ReturnDimention.push_back(Elum);

				// 오류가 없다면 현재 노드의 차원을 뒤에서 부터 곱한 값과 오퍼레이터되는 노드의 차원을 앞에서 부터 곱한 값이 같아야함.
				for (int Over = 0, Elum; Over < OverlapDimention; Over++)
					Elum = Node.Dimention[Over], Overlap *= Elum;

				for (int Over = 0, Elum; Over < OverlapDimention; Over++)
					Elum = Node.Dimention[Node.Dimention.size() - 1 - Over], Back *= Elum, ReturnDimention.push_back(Elum);
			}

			virtual void close()
			{
				delete this;
			}
		};

		template <typename Type>
		class Node : public KAINode<Type>
		{
		protected:
			Node(KAIController<Type>* Owner, std::vector<int> Dimention)
			{
				CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				this->Owner = Owner;
				this->User = Owner;
				this->Dimention = Dimention;
			}
			Node(KAIController<Type>* Owner, std::vector<int> Dimention, KAIVal<Type>* ValRef) : Node(Owner, Dimention)
			{
				this->ValRef = ValRef;
			}
		public:
			virtual void DESTRUCTOR() override
			{
				CONSOL_MESSAGE(this, "DESTRUCTOR is called");
				if (!this->Owner->DestroyedState)
					this->Owner->Nodes->Remove(this);

				delete this;
			}

		public:
			virtual KAIVal<Type>* getVal() override
			{
				if (this->ValRef == nullptr)
				{
					this->ValRef = this->Owner->newVal(GetDimentionSize(this->Dimention));
					this->ValIsCreatedByThis = true;
				}
				return this->ValRef;
			}
			virtual const int getSize() override
			{ 
				return getVal()->Size;
			}
			virtual const std::vector<int>& getDimention() override 
			{ 
				return this->Dimention;
			}
			virtual KAINode<Type>& setUser(KAIController<Type>* ChangeUser, bool TempUser = true) override
			{
				this->User = ChangeUser;
				this->TempUser = TempUser;
				return *this;
			}
			virtual KAIController<Type>* getUser() override
			{
				return this->User;
			}
			virtual void init(Type InitVal) override
			{
				getVal()->calc_set_type(InitVal);
			}

			//
			//	Operator
			//

			virtual void end_operator()
			{
				if (this->TempUser)
					this->User = this->Owner;
				this->TempUser = false;
			}
			//	default process

			virtual KAINode<Type>& Operator_Add(Type Const) override
			{
				KAINode<Type>* NewNode = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::ArrayAddByConst<Type>* New = new KAI::FLOW::STACK::ArrayAddByConst<Type>(getUser()->LastFlow, NewNode, this, Const);

				end_operator(); 
				return *NewNode;
			}
			virtual KAINode<Type>& Operator_Sub(Type Const) override
			{
				KAINode<Type>* NewNode = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::ArraySubByConst<Type>* New = new KAI::FLOW::STACK::ArraySubByConst<Type>(getUser()->LastFlow, NewNode, this, Const);

				end_operator();
				return *NewNode;
			}
			virtual KAINode<Type>& Operator_Mul(Type Const) override
			{
				KAINode<Type>* NewNode = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::ArrayMulByConst<Type>* New = new KAI::FLOW::STACK::ArrayMulByConst<Type>(getUser()->LastFlow, NewNode, this, Const);

				end_operator();
				return *NewNode;
			}
			virtual KAINode<Type>& Operator_Div(Type Const) override
			{
				KAINode<Type>* NewNode = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::ArrayDivByConst<Type>* New = new KAI::FLOW::STACK::ArrayDivByConst<Type>(getUser()->LastFlow, NewNode, this, Const);

				end_operator();
				return *NewNode;
			}

			virtual KAINode<Type>& Operator_Add(KAINode<Type>& Node) override
			{
				KAINode<Type>* NewNode = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::ArrayAdd<Type>* New = new KAI::FLOW::STACK::ArrayAdd<Type>(getUser()->LastFlow, NewNode, this, &Node);

				end_operator();
				return *NewNode;
			}
			virtual KAINode<Type>& Operator_Sub(KAINode<Type>& Node) override
			{
				KAINode<Type>* NewNode = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::ArraySub<Type>* New = new KAI::FLOW::STACK::ArraySub<Type>(getUser()->LastFlow, NewNode, this, &Node);

				end_operator();
				return *NewNode;
			}
			virtual KAINode<Type>& Operator_Mul(KAINode<Type>& Node) override
			{
				KAINode<Type>* NewNode = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::ArrayMul<Type>* New = new KAI::FLOW::STACK::ArrayMul<Type>(getUser()->LastFlow, NewNode, this, &Node);

				end_operator();
				return *NewNode;
			}
			virtual KAINode<Type>& Operator_Div(KAINode<Type>& Node) override
			{
				KAINode<Type>* NewNode = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::ArrayDiv<Type>* New = new KAI::FLOW::STACK::ArrayDiv<Type>(getUser()->LastFlow, NewNode, this, &Node);

				end_operator();
				return *NewNode;
			}

			//assign

			virtual void Operator_Assign_Add(KAINode<Type>& Node) override
			{
				KAI::FLOW::STACK::AssignAdd<Type>* New = new KAI::FLOW::STACK::AssignAdd<Type>(getUser()->LastFlow, this, &Node);

				end_operator();
			}
			virtual void Operator_Assign_Sub(KAINode<Type>& Node) override
			{
				KAI::FLOW::STACK::AssignSub<Type>* New = new KAI::FLOW::STACK::AssignSub<Type>(getUser()->LastFlow, this, &Node);

				end_operator();
			}
			virtual void Operator_Assign_Mul(KAINode<Type>& Node) override
			{
				KAI::FLOW::STACK::AssignMul<Type>* New = new KAI::FLOW::STACK::AssignMul<Type>(getUser()->LastFlow, this, &Node);

				end_operator();
			}
			virtual void Operator_Assign_Div(KAINode<Type>& Node) override
			{
				KAI::FLOW::STACK::AssignDiv <Type>* New = new KAI::FLOW::STACK::AssignDiv<Type>(getUser()->LastFlow, this, &Node);

				end_operator();
			}

			virtual void Operator_Assign(Type TypeVal) override
			{
				KAI::FLOW::STACK::SetByType<Type>* New = new KAI::FLOW::STACK::SetByType<Type>(getUser()->LastFlow, this, TypeVal);

				end_operator();
			}
			virtual void Operator_Assign(KAINode<Type>& Node) override
			{
				//if (Node.ValRef == nullptr)
				//{	//Node가 다이나믹인 경우
				//	Node.ValRef = this->ValRef;
				//}
				//else
				//{	//getUser()Node인 경우
				//	KAI::FLOW::STACK::SetByNode<Type>* New = new KAI::FLOW::STACK::SetByNode<Type>(getUser()->LastFlow, this, &Node);
				//} removed this absolutly.
				KAI::FLOW::STACK::SetByNode<Type>* New = new KAI::FLOW::STACK::SetByNode<Type>(getUser()->LastFlow, this, &Node);

				end_operator();
			}

			// Power

			virtual KAINode<Type>& Operator_Power(int Times) override
			{
				KAINode<Type>* NewNode = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::Times<Type>* New = new KAI::FLOW::STACK::Times<Type>(getUser()->LastFlow, NewNode, this, Times);

				end_operator();
				return *NewNode;
			}

			virtual KAIMatrixOperator<Type>& matrix() override
			{
				return *new MatrixOperator<Type>(this); // 이부분에 오류있음. 임시 변수가 사라짐.
			}

			// transpose 
			virtual KAINode<Type>& transepose(int Row, int Column, std::vector<int> Dimention) override
			{
				KAINode<Type>* TransposedNode = this->getUser()->newNode(Dimention);
				KAI::FLOW::STACK::Transpose<Type>* New = new KAI::FLOW::STACK::Transpose<Type>(this->getUser()->LastFlow, TransposedNode, this, Row, Column);

				end_operator();
				return *TransposedNode;
			}
			// 앞에서 부터 곱해서 Front를 만듬. [0, Size)
			virtual KAINode<Type>& transepose(int Size) override
			{
				int Front = 1, Overlap = 1, Back = 1;
				std::vector<int> Dimention = this->getDimention();
				std::vector<int> FrontDimention;
				std::vector<int> BackDimention;

				for (int i = 0, Elum; i < (int)Dimention.size(); i++)
					if (i < Size)
						Elum = Dimention[i], Front *= Elum, FrontDimention.push_back(Elum);
					else
						Elum = Dimention[i], Back *= Elum, BackDimention.push_back(Elum);

				BackDimention.insert(BackDimention.end(), FrontDimention.begin(), FrontDimention.end());

				KAINode<Type>* TransposedNode = this->getUser()->newNode(BackDimention);
				KAI::FLOW::STACK::Transpose<Type>* New = new KAI::FLOW::STACK::Transpose<Type>(this->getUser()->LastFlow, TransposedNode, this, Front, Back);

				end_operator();
				return *TransposedNode;
			}
			//virtual KAINode<Type>& rowsum(int Row, int Column)
			//{

			//}
			//virtual KAINode<Type>& columnsum(int Row, int Column)
			//{

			//}

			//	Compare
			//

			virtual KAINode<Type>& Operator_Compare_Bigger(const Type& Const1) override
			{
				KAINode<Type>* result = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::CompareBiggerThenConst<Type>* new_stack = new KAI::FLOW::STACK::CompareBiggerThenConst<Type>(this->getUser()->LastFlow, result, this, Const1);

				end_operator();
				return *result;
			}
			virtual KAINode<Type>& Operator_Compare_Smaller(const Type& Const1) override
			{
				KAINode<Type>* result = this->getUser()->newNode(this->Dimention);
				KAI::FLOW::STACK::CompareSmallerThenCosnt<Type>* new_stack = new KAI::FLOW::STACK::CompareSmallerThenCosnt<Type>(this->getUser()->LastFlow, result, this, Const1);

				end_operator();
				return *result;
			}

			friend KAI::Controller<Type>;
		};
	}
}