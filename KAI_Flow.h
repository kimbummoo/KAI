/*
	Copyright (c) 2020 Kim,Bummoo
	Read KAI.h
*/
#pragma once
#include "_include.h"
#include "KAI_Controller.h"
#include "KAI_Val.h"

namespace KAI
{
	namespace FLOW {
		namespace STACK {
			template <typename Type>
			class Stack : public KAIStack<Type>
			{
			public:
				//Stack is created in node operator
				Stack(KAIFlow<Type>* Owner)
				{
					this->Owner = Owner;
					this->Owner->Stacks.push_back(this);
				}
				virtual void calc() override {}
				virtual void diff(KAIDiffResult<Type>& Con) override {}
			};
			//
			//	TwoParmCalc
			//	O1 = I1 ? I2	Done
			//
			template<typename Type>
			class TwoParmCalc : public Stack<Type>
			{
			public:
				TwoParmCalc(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2) : 
					Stack<Type>(Owner),
					O1(O1), I1(I1), I2(I2)
				{
				}
			public:
				KAINode<Type>* O1;	// 음? 이거는 한번에 하면 안됨??
				KAINode<Type>* I1;
				KAINode<Type>* I2;

				friend KAINode<Type>;
			};
			template<typename Type>
			class TwoParmCalc_Matrix : public TwoParmCalc<Type>
			{
			public:
				TwoParmCalc_Matrix(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2, int Row, int Overlap, int Column) :
					TwoParmCalc<Type>(Owner, O1, I1, I2),
					Row(Row), Overlap(Overlap), Column(Column)
				{
				}
			public:
				int Row, Overlap, Column;

				friend KAINode<Type>;
			};
			//
			//	OneParmCalc
			//	O1 ?= I1	Done
			//
			template <typename Type>
			class OneParmCalc : public Stack<Type>
			{
			public:
				OneParmCalc(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1) :
					Stack<Type>(Owner),
					O1(O1), I1(I1)
				{
				}
				KAINode<Type>* O1;
				KAINode<Type>* I1;
			};
			template <typename Type>
			class OneParmCalc_Matrix : public OneParmCalc<Type>
			{
			public:
				OneParmCalc_Matrix(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, int Row, int Column) :
					OneParmCalc<Type>(Owner, O1, I1),
					Row(Row), Column(Column)
				{
				}
				int Row, Column;
			};
			//
			//	OneParmOneTypeCalc
			//	O1 = I1 ? Const1
			//
			template<typename Type>
			class OneParmOneTypeCalc : public Stack<Type>
			{
			public:
				OneParmOneTypeCalc(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, Type Const1) : 
					Stack<Type>(Owner)
				{
					this->O1 = O1;
					this->I1 = I1;
					this->Const1 = Const1;
				}
			public:
				KAINode<Type>* O1;
				KAINode<Type>* I1;
				Type Const1;
			};
			//
			//	OneTypeCalc
			//	O1 ?= Const1
			//
			template<typename Type>
			class OneTypeCalc : public Stack<Type>
			{
			public:
				OneTypeCalc(KAIFlow<Type>* Owner, KAINode<Type>* O1, Type Const1) : Stack<Type>(Owner)
				{
					this->O1 = O1;
					this->Const1 = Const1;
				}
			public:
				KAINode<Type>* O1;
				Type Const1;
			};

			// Assign
			// 

			template<typename Type>
			class AssignAddByType : public OneTypeCalc<Type>
			{
			public:
				AssignAddByType(KAIFlow<Type>* Owner, KAINode<Type>* O1, Type TypeVal) :
					OneTypeCalc<Type>(Owner, O1, TypeVal) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_assign_add(this->O1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					//dO1 *= (Type)1;
				}
			};
			template<typename Type>
			class AssignSubByType : public OneTypeCalc<Type>
			{
			public:
				AssignSubByType(KAIFlow<Type>* Owner, KAINode<Type>* O1, Type TypeVal) :
					OneTypeCalc<Type>(Owner, O1, TypeVal) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_assign_sub(this->O1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					//dO1 *= (Type)1;
				}
			};
			template<typename Type>
			class AssignMulByType : public OneTypeCalc<Type>
			{
			public:
				AssignMulByType(KAIFlow<Type>* Owner, KAINode<Type>* O1, Type TypeVal) :
					OneTypeCalc<Type>(Owner, O1, TypeVal) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_assign_sub(this->O1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);

					dO1 *= (Type)this->Const1;
				}
			};
			template<typename Type>
			class AssignDivByType : public OneTypeCalc<Type>
			{
			public:
				AssignDivByType(KAIFlow<Type>* Owner, KAINode<Type>* O1, Type TypeVal) :
					OneTypeCalc<Type>(Owner, O1, TypeVal) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_assign_sub(this->O1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);

					dO1 /= (Type)this->Const1;
				}
			};

			template<typename Type>
			class AssignAdd : public OneParmCalc<Type>
			{
			public:
				AssignAdd(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1)
					: OneParmCalc<Type>(Owner, O1, I1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_assign_add(this->I1->getVal());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					//dO1 *= (Type)1;
					dI1 += dO1;
				}
			};
			template<typename Type>
			class AssignSub : public OneParmCalc<Type>
			{
			public:
				AssignSub(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1)
					: OneParmCalc<Type>(Owner, O1, I1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_assign_sub(this->I1->getVal());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					//dO1 *= (Type)1;
					dI1 -= dO1;// * (Type)-1;
				}
			};
			template<typename Type>
			class AssignMul : public OneParmCalc<Type>
			{
			public:
				AssignMul(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1)
					: OneParmCalc<Type>(Owner, O1, I1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_assign_mul(this->I1->getVal());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					dO1 *= (*this->I1);
					dI1 += dO1 * (*this->O1);
				}
			};
			template<typename Type>
			class AssignDiv : public OneParmCalc<Type>
			{
			public:
				AssignDiv(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1)
					: OneParmCalc<Type>(Owner, O1, I1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called"); 
					delete this; 
				}

				virtual void calc() override
				{
					this->O1->getVal()->calc_assign_div(this->I1->getVal());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					dO1 /= (*this->I1);
					//dI1 += dO1 * O1;
				}
			};

			// Array

			template<typename Type>
			class ArrayAddByConst : public OneParmOneTypeCalc<Type>
			{
			public:
				ArrayAddByConst(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, Type Const1) :
					OneParmOneTypeCalc<Type>(Owner, O1, I1, Const1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_array_add(this->I1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					dI1 += dO1;
				}
			};
			template<typename Type>
			class ArraySubByConst : public OneParmOneTypeCalc<Type>
			{
			public:
				ArraySubByConst(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, Type Const1) :
					OneParmOneTypeCalc<Type>(Owner, O1, I1, Const1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_array_sub(this->I1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					dI1 += dO1;
				}
			};
			template<typename Type>
			class ArrayMulByConst : public OneParmOneTypeCalc<Type>
			{
			public:
				ArrayMulByConst(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, Type Const1) :
					OneParmOneTypeCalc<Type>(Owner, O1, I1, Const1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_array_mul(this->I1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					dI1 += dO1 * this->Const1;
				}
			};
			template<typename Type>
			class ArrayDivByConst : public OneParmOneTypeCalc<Type>
			{
			public:
				ArrayDivByConst(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, Type Const1) :
					OneParmOneTypeCalc<Type>(Owner, O1, I1, Const1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_array_div(this->I1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					dI1 += dO1 / this->Const1;
				}
			};

			template<typename Type>
			class ArrayAdd : public TwoParmCalc<Type>
			{
			public:
				ArrayAdd(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2) : 
					TwoParmCalc<Type>(Owner, O1, I1, I2) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_array_add_looparrange(this->I1->getVal(), this->I2->getVal(), this->I1->getSize(), this->I2->getSize());
				}	
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					KAINode<Type>& dI2 = Con(*this->I2);

					dI1 += dO1;// *(Type)1;
					dI2 += dO1;// +(*I1);
				}
			};
			template<typename Type>
			class ArraySub : public TwoParmCalc<Type>
			{
			public:
				ArraySub(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2)
					: TwoParmCalc<Type>(Owner, O1, I1, I2) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_array_sub_looparrange(this->I1->getVal(), this->I2->getVal(), this->I1->getSize(), this->I2->getSize());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					KAINode<Type>& dI2 = Con(*this->I2);

					dI1 += dO1;// - (*I2);
					dI2 -= dO1;;
				}
			};
			template<typename Type>
			class ArrayMul : public TwoParmCalc<Type>
			{
			public:
				ArrayMul(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2)
					: TwoParmCalc<Type>(Owner, O1, I1, I2) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_array_mul_looparrange(this->I1->getVal(), this->I2->getVal(), this->I1->getSize(), this->I2->getSize());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					KAINode<Type>& dI2 = Con(*this->I2);

					dI1 += dO1 * (*this->I2);
					dI2 += dO1 * (*this->I1);
				}
			};
			template<typename Type>
			class ArrayDiv : public TwoParmCalc<Type>
			{
			public:
				ArrayDiv(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2)
					: TwoParmCalc<Type>(Owner, O1, I1, I2) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_array_div_looparrange(this->I1->getVal(), this->I2->getVal(), this->I1->getSize(), this->I2->getSize());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					KAINode<Type>& dI2 = Con(*this->I2);

					dI1 += dO1 / (*this->I2);
					dI2 += dO1 * (*this->I1);
				}
			};

			//
			//MatrixCalc
			//

			//template<typename Type>
			//class MatrixAdd : public TwoParmCalc_Matrix<Type>
			//{
			//public:
			//	MatrixAdd(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2, int Row, int Overlap, int Column)
			//		: TwoParmCalc_Matrix<Type>(Owner, O1, I1, I2, Row, Overlap, Column) {}
			//	virtual void calc() override
			//	{
			//		O1->getVal()->calc_matrix_add(I1->getVal(), I2->getVal(), Row, Overlap, Column);
			//	}
			//	virtual void diff(KAIDiffResult<Type>& Con) override
			//	{
			//		KAINode<Type>& dO1 = Con(*O1);
			//		KAINode<Type>& dI1 = Con(*I1);
			//		KAINode<Type>& dI2 = Con(*I2);

			//		dI1 = dI1 + dO1[columnsum]; // temporary
			//		dI2 = dI2 + dO1[rowsum]; // temporary
			//	}
			//};
			//template<typename Type>
			//class MatrixSub : public TwoParmCalc_Matrix<Type>
			//{
			//public:
			//	MatrixSub(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2, int Row, int Overlap, int Column)
			//		: TwoParmCalc_Matrix<Type>(Owner, O1, I1, I2, Row, Overlap, Column) {}
			//	virtual void calc() override
			//	{
			//		O1->getVal()->calc_matrix_sub(I1->getVal(), I2->getVal(), Row, Overlap, Column);
			//	}
			//	virtual void diff(KAIDiffResult<Type>& Con) override
			//	{
			//		KAINode<Type>& dO1 = Con(*O1);
			//		KAINode<Type>& dI1 = Con(*I1);
			//		KAINode<Type>& dI2 = Con(*I2);

			//		dI1 = dI1 + dO1[columnsum]; // temporary
			//		dI2 = dI2 - dO1[rowsum]; // temporary
			//	}
			//};
			template<typename Type>
			class MatrixMul : public TwoParmCalc_Matrix<Type>
			{
			public:
				MatrixMul(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2, int Row, int Overlap, int Column)
					: TwoParmCalc_Matrix<Type>(Owner, O1, I1, I2, Row, Overlap, Column) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_matrix_mul(this->I1->getVal(), this->I2->getVal(), this->Row, this->Overlap, this->Column);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					KAINode<Type>& dI2 = Con(*this->I2);

					dI1 += dO1 * (*this->I2).transepose(this->Row, this->Column, dI1.getDimention());
					/*KAINode<Type>& cI1 = *Con.Based->newNode(I1->getDimention());*/
					dI2 += (*this->I1).transepose(this->Row, this->Column, dI2.getDimention()).setUser(Con.Based) * dO1;
				}
			};
			template<typename Type>
			class MatrixDiv : public TwoParmCalc_Matrix<Type>
			{
			public:
				MatrixDiv(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, KAINode<Type>* I2, int Row, int Overlap, int Column)
					: TwoParmCalc_Matrix<Type>(Owner, O1, I1, I2, Row, Overlap, Column) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_matrix_div(this->I1->getVal(), this->I2->getVal(), this->Row, this->Overlap, this->Column);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);
					KAINode<Type>& dI2 = Con(*this->I2);
					
					dI1 += dO1 * (*this->I2).transepose(this->Row, this->Column, dI1.getDimention());
					/*KAINode<Type>& cI1 = *Con.Based->newNode(I1->getDimention());*/
					dI2 -= (*this->I1).transepose(this->Row, this->Column, dI2.getDimention()).setUser(Con.Based) * dO1;
				}
			};

			//OtherCalc


			template<typename Type>
			class SetByType : public OneTypeCalc<Type>
			{
			public:
				SetByType(KAIFlow<Type>* Owner, KAINode<Type>* O1, Type Const1)
					: OneTypeCalc<Type>(Owner, O1, Const1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_set_type(this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
				}
			};
			template<typename Type>
			class SetByNode : public OneParmCalc<Type>
			{
			public:
				SetByNode(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1)
					: OneParmCalc<Type>(Owner, O1, I1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_set_looparrange(this->I1->getVal(), this->I1->getSize());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					dI1 = dO1;
				}
			};
			template<typename Type>
			class Times : public OneParmCalc<Type>
			{
			public:
				Times(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, int Times)
					: OneParmCalc<Type>(Owner, O1, I1), Loop(Times) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				int Loop;
				virtual void calc() override
				{
					this->O1->getVal()->calc_times(this->I1->getVal(), Loop);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					this->I1->setUser(Con.Based);
					{
						dI1 += dO1*((*this->I1)^(Loop-1));
					}
					this->I1->setUser(this->I1->Owner);
				}
			};
			template<typename Type>
			class Transpose : public OneParmCalc_Matrix<Type>
			{
			public:
				Transpose(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, int Row, int Column) :
					OneParmCalc_Matrix<Type>(Owner, O1, I1, Row, Column) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_set_transpose(this->I1->getVal(), this->Row, this->Column);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					dI1 += dO1.transepose(this->Row, this->Column, dI1.getDimention());
				}
			};
			template<typename Type>
			class RowSum : public OneParmCalc<Type>
			{
			public:
				RowSum(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1)
					: OneParmCalc<Type>(Owner, O1, I1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_rowsum(this->I1->getVal(), this->I1->getRow());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					dI1 += dO1;
				}
			};
			template<typename Type>
			class ColumnSum : public OneParmCalc<Type>
			{
			public:
				ColumnSum(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1)
					: OneParmCalc<Type>(Owner, O1, I1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}
				virtual void calc() override
				{
					this->O1->getVal()->calc_columnsum(this->I1->getVal(), this->I1->getColumn());
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					dI1 += dO1;
				}
			};

			//	Compare
			//

			template <typename Type>
			class CompareBiggerThenConst : public OneParmOneTypeCalc<Type>
			{
			public:
				CompareBiggerThenConst(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, Type Const1) :
					OneParmOneTypeCalc<Type>(Owner, O1, I1, Const1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}

				virtual void calc() override
				{
					this->O1->getVal()->calc_compare_bigger(this->I1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					// I1				 O1
					//	x.x	x.x	x.x			0	0	1
					//	x.x	x.x	x.x			0	1	0
					//dI1 = O1 * I1
					dI1 += dO1 * (*this->O1); // O1에서 ELum이 0이라면 전달되는 편미분값은 0이 올것인가?
									 // 안된다!.
				}
			};
			template <typename Type>
			class CompareSmallerThenCosnt : public OneParmOneTypeCalc<Type>
			{
			public:
				CompareSmallerThenCosnt(KAIFlow<Type>* Owner, KAINode<Type>* O1, KAINode<Type>* I1, Type Const1) :
					OneParmOneTypeCalc<Type>(Owner, O1, I1, Const1) {
					CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				}
				virtual void DESTRUCTOR() override {
					CONSOL_MESSAGE(this, "DESTRUCTOR is called");
					delete this;
				}

				virtual void calc() override
				{
					this->O1->getVal()->calc_compare_smaller(this->I1->getVal(), this->Const1);
				}
				virtual void diff(KAIDiffResult<Type>& Con) override
				{
					KAINode<Type>& dO1 = Con(*this->O1);
					KAINode<Type>& dI1 = Con(*this->I1);

					// I1				 O1
					//	x.x	x.x	x.x			0	0	1
					//	x.x	x.x	x.x			0	1	0
					//dI1 = O1 * I1
					dI1 += dO1 * (*this->O1);
				}
			};
		}

		//미분결과는 컨트롤러가 기억하지 않음.
		template <typename Type>
		class DiffResult : public KAIDiffResult<Type>
		{	
		public:
			DiffResult(KAIController<Type>* Based)
			{
				CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				this->Based = Based;
				this->DiffFlow = Based->newFlow();
			}
			virtual void DESTRUCTOR() override
			{
				CONSOL_MESSAGE(this, "DESTRUCTOR is called");
				for (auto Pair : this->Map)
					Pair.second->DESTRUCTOR();
				this->DiffFlow->DESTRUCTOR();
				delete this;
			}
			virtual KAINode<Type>& Operator_Bracket(KAINode<Type>& Key, int InitDiff = 0)
			{
				KAINode<Type>* Return;
				if (this->Map.find(&Key) == this->Map.end())
				{
					Return = this->Based->newNode(Key.getDimention());
					(*Return) = (Type)InitDiff;	// Init의 경우 DiffFlow를 through를 여러번 호출할 때 초기화가 안되는 문제가 있음.
					this->Map.insert(std::make_pair(&Key, Return));
				}
				else
				{
					Return = this->Map[&Key];
				}
				return *Return;
			}
		};
		template <typename Type>
		class Flow : public KAIFlow<Type>
		{
		protected:
			Flow(Controller<Type>* Owner)
			{
				CONSOL_MESSAGE(this, "CONSTRUCTOR is called");
				this->Owner = Owner;
			}
		public:
			virtual void DESTRUCTOR() override
			{
				CONSOL_MESSAGE(this, "DESTRUCTOR is called");
				if (!this->Owner->DestroyedState)
				{
					if (this->Owner->MainFlow == this)
						this->Owner->MainFlow = this->Owner->newFlow();
					if (this->Owner->LastFlow == this)
						this->Owner->LastFlow = this->Owner->MainFlow;
					this->Owner->Flows->Remove(this);
				}
				for (auto Elum : this->Stacks) Elum->DESTRUCTOR();
				delete this;
			}
			// _context
			virtual void through() override
			{
				for (auto Elum : this->Stacks) Elum->calc();
			}
			// _context
			virtual void trace(KAIDiffResult<Type>& DiffResultContainer, KAINode<Type>& DiffTarget) override
			{
				KAINode<Type>& DiffTargetPair = DiffResultContainer(DiffTarget, 1);
				for (auto it = this->Stacks.rbegin(); it != this->Stacks.rend(); it++) (*it)->diff(DiffResultContainer);
			}
			// _context
			virtual KAIDiffResult<Type>* GetEmptyDiffResult(KAIController<Type>* Controller) override
			{
				return new DiffResult<Type>(Controller);
			}
			friend Controller<Type>;
		};
	}
}