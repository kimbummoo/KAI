/*
	Copyright (c) 2020 Kim,Bummoo
	Read KAI.h
*/
#pragma once
#include "_include.h"
namespace KAI
{
	//
	namespace VAL {
		template <typename Type>
		class Val : public KAIVal<Type>
		{
		protected:
			//constructor is called by controller
			Val(KAIController<Type>* Owner, int Size) : KAIVal<Type>(Owner, Size), Ref(Size)
			{
				CONSOL_MESSAGE(this, "CONSTRUCTOR is called");

			}
		public:
			virtual void DESTRUCTOR() override
			{
				CONSOL_MESSAGE(this, "DESTRUCTOR is called");

				if(!this->Owner->DestroyedState)
					this->Owner->Vals->Remove(this);
				delete this;
			}
			concurrency::array_view<Type> Ref;
		public:
			virtual void* GetRef() override
			{
				return (void*)&this->Ref;
			}
			virtual Type& Get(const int& Index) 
			{
				return (Ref)[Index];
			}

			// Calculator
			// Default

			virtual void calc_set_type(Type InitVal) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = InitVal;
				});
			}
			virtual void calc_set_val(KAIVal<Type>* I1) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx) restrict(amp)
				{
					o[idx] = i1[idx];
				});
			}
			virtual void calc_set_looparrange(KAIVal<Type>* I1, int I1arrange) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx) restrict(amp)
				{
					o[idx] = i1[idx % I1arrange];
				});
			}
			virtual void calc_set_transpose(KAIVal<Type>* I1, int Row, int Column) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				//
				// 0 1     0 2 4 6
				// 2 3 >>> 1 3 5 7
				// 4 5
				// 6 7
				//
				// Idx	0 1 2 3 4 ...
				// IPR	Idx % RowSize
				//		0 1 2 3 1 2 3 4 ...
				// IRMC IPR * ColumnSize
				//		0 2 4 6 0 2 4 6 ...
				//
				// Idx	0 1 2 3 4 ...
				// IDR  Idx / RowSize
				//		0 0 0 0 1 ...
				//
				// i1 idx = IRMC + IDR
				//
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[ (idx % Row * Column) + idx / Row ];
					});
			}
			//void calc_set_transpose(Val* I1, int Bundle, int Row, int Column, int Elum)
			//{
			//	concurrency::array_view<Type>& o = *Ref;
			//	concurrency::array_view<Type>& i1 = *I1->Ref;

			//	// 2차 메트릭스의 트렌스 포즈는 다음과 같이 표현될 수 있다.
			//	// Node({ 1, 3, 2, 1 })[transpose] == Node({ 3, 2 })[transpose] == Node_Transpose({ 2, 3 })
			//	
			//	// 2차원 이상인 경우
			//	// Node({ 1, 2, 3 }) == Node_Transpose({ 3, 2, 1 )}
			//	// 홀수의 경우 가운데 위치한 차원은 남아있고 나머지는 트렌스 포즈 된다.
			//	// Node({ 1, 2, 3, 4 }) == Node_Transpose({ 4, 3, 2, 1 })
			//	// 짝수의 모두 트렌즈 포즈 된다.

			//	// 2개 이상의 트렌즈 포즈가 존재할 경우 한번에 트렌즈 포즈 되는 것이 아니라 나눠서 트렌즈 포즈 된다.
			//	// 두 순서중 무엇을 먼저 하든 차이가 없는 것처럼 보인다.
			//	// { 1, 2, 3, 4 } >>> { 1, 3, 2, 4 } >>> { 4, 3, 2, 1 }
			//	// { 1, 2, 3, 4 } >>> { 4, 2, 3, 1 } >>> { 4, 3, 2, 1 }

			//	// 2 차원의 트렌즈 포즈는 다음과 같이 이루어 진다.
			//	//   1 2 3     1 4 7
			//	//   4 5 6 >>> 2 5 8
			//	//   7 8 9     3 6 9

			//	concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
			//		//int ElumIndex = idx / Elum;		//idx는 int로 형변환 되지 않음.
			//		//int ElumOffset = idx % Elum;
			//		//int BundleIndex = idx / Bundle;

			//		o[idx] = i1[(idx / Bundle) * Bundle + (idx / Elum) / Column * Row + (idx / Elum) % Column + (idx % Elum)];
			//	});
			//}
			//void calc_rowsum(Val* I1, int I1_Row)
			//{
			//	concurrency::array_view<Type>& o = *Ref;
			//	concurrency::array_view<Type>& i1 = *I1->Ref;
			//	concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
			//		for (int i = 0; i < I1_Row; i++)
			//		{
			//			o[idx] = i1[idx + i * I1_Row];
			//		}
			//	});
			//}
			//void calc_columnsum(Val* I1, int I1_Column)
			//{
			//	concurrency::array_view<Type>& o = *Ref;
			//	concurrency::array_view<Type>& i1 = *I1->Ref;
			//	concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
			//		for (int i = 0; i < I1_Column; i++)
			//		{
			//			o[idx] = i1[idx * I1_Column  + i];
			//		}
			//	});
			//}
			virtual void calc_times(KAIVal<Type>* I1, int Loop) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				if (Loop >= 0)
				{
					concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
						o[idx] = 1;
						for(int i = 0; i < Loop; i++) o[idx] *= i1[idx];
					});
				}
				else // (0 > Loop)
				{
					concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
						o[idx] = 1;
						for (int i = 0; i > Loop; i--) o[idx] /= i1[idx];
					});
				}
			}
			//Assign
			virtual void calc_assign_add(Type Val) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] += Val;
				});
			}
			virtual void calc_assign_sub(Type Val) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] -= Val;
				});
			}
			virtual void calc_assign_mul(Type Val) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] *= Val;
				});
			}
			virtual void calc_assign_div(Type Val) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] /= Val;
				});
			}
			virtual void calc_assign_add(KAIVal<Type>* I1)
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] += i1[idx];
				});
			}
			virtual void calc_assign_sub(KAIVal<Type>* I1)
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] -= i1[idx];
				});
			}
			virtual void calc_assign_mul(KAIVal<Type>* I1)
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] *= i1[idx];
				});
			}
			virtual void calc_assign_div(KAIVal<Type>* I1)
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] /= i1[idx];
				});
			}
			//Array
			virtual void calc_array_add(KAIVal<Type>* I1, Type Const1)
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx] + Const1;
				});
			}
			virtual void calc_array_sub(KAIVal<Type>* I1, Type Const1)
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx] - Const1;
				});
			}
			virtual void calc_array_mul(KAIVal<Type>* I1, Type Const1) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx] * Const1;
				});
			}
			virtual void calc_array_div(KAIVal<Type>* I1, Type Const1) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx] / Const1;
				});
			}
			virtual void calc_array_add(KAIVal<Type>* I1, KAIVal<Type>* I2) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx] + i2[idx];
				});
			}
			virtual void calc_array_sub(KAIVal<Type>* I1, KAIVal<Type>* I2) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx] - i2[idx];
				});
			}
			virtual void calc_array_mul(KAIVal<Type>* I1, KAIVal<Type>* I2) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx] * i2[idx];
				});
			}
			virtual void calc_array_div(KAIVal<Type>* I1, KAIVal<Type>* I2) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx] / i2[idx];
				});
			}
			virtual void calc_array_add_looparrange(KAIVal<Type>* I1, KAIVal<Type>* I2, int I1arrange, int I2arrange) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx % I1arrange] + i2[idx % I2arrange];
				});
			}
			virtual void calc_array_sub_looparrange(KAIVal<Type>* I1, KAIVal<Type>* I2, int I1arrange, int I2arrange) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx % I1arrange] - i2[idx % I2arrange];
				});
			}
			virtual void calc_array_mul_looparrange(KAIVal<Type>* I1, KAIVal<Type>* I2, int I1arrange, int I2arrange) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx % I1arrange] * i2[idx % I2arrange];
				});
			}
			virtual void calc_array_div_looparrange(KAIVal<Type>* I1, KAIVal<Type>* I2, int I1arrange, int I2arrange) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx)restrict(amp) {
					o[idx] = i1[idx % I1arrange] / i2[idx % I2arrange];
				});
			}
			
			//
			//	Matrix
			//

			virtual void calc_matrix_add(KAIVal<Type>* I1, KAIVal<Type>* I2, int Row, int Column, int Matrix) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx) restrict(amp)
				{
					o[idx] = 0;
					for (int loop = 0; loop < Matrix; loop++)
					{
						o[idx] += i1[(idx / Row)*Matrix + loop] + i2[idx%Column + (loop*Column)];
					}
				});
			}
			virtual void calc_matrix_sub(KAIVal<Type>* I1, KAIVal<Type>* I2, int Row, int Column, int Matrix) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx) restrict(amp)
				{
					o[idx] = 0;
					for (int loop = 0; loop < Matrix; loop++)
					{
						o[idx] += i1[(idx / Row)*Matrix + loop] - i2[idx%Column + (loop*Column)];
					}
				});
			}
			virtual void calc_matrix_mul(KAIVal<Type>* I1, KAIVal<Type>* I2, int Row, int Column, int Matrix) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx) restrict(amp)
				{
					o[idx] = 0;
					for (int loop = 0; loop < Matrix; loop++)
					{
						o[idx] += i1[(idx / Row)*Matrix + loop] * i2[idx%Column + (loop*Column)];
					}
				});
			}
			virtual void calc_matrix_div(KAIVal<Type>* I1, KAIVal<Type>* I2, int Row, int Column, int Matrix) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::array_view<Type>& i2 = ((Val<Type>*)I2)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx) restrict(amp)
				{
					o[idx] = 0;
					for (int loop = 0; loop < Matrix; loop++)
					{
						o[idx] += i1[(idx / Row)*Matrix + loop] / i2[idx%Column + (loop*Column)];
					}
				});
			}

			//
			//	Compare
			//

			virtual void calc_compare_bigger(KAIVal<Type>* I1, Type Const1) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx) restrict(amp)
					{
						o[idx] = i1[idx] > Const1;
					});
			}
			virtual void calc_compare_smaller(KAIVal<Type>* I1, Type Const1) override
			{
				concurrency::array_view<Type>& o = this->Ref;
				concurrency::array_view<Type>& i1 = ((Val<Type>*)I1)->Ref;
				concurrency::parallel_for_each(o.extent, [=](concurrency::index<1> idx) restrict(amp)
					{
						o[idx] = i1[idx] < Const1;
					});
			}
			friend Controller<Type>;
		};
	}
}