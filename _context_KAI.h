#pragma once

//// Memory leak test
//#include <iostream>
//#include <crtdbg.h>
//#if _DEBUG
//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
////#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
//#endif

//_CrtDumpMemoryLeaks();

//#include <set>
#include <vector>
#include <map>

//////////////////////////////////////////////////////////////////////////////////////////////
//
//	Typedef
//
//////////////////////////////////////////////////////////////////////////////////////////////

template <typename Type>
class KAIController;
template <typename ConType, typename ValType>
class KAIRefController;

template <typename Type>
class KAIFlow;
template <typename Type>
class KAIDiffResult;
template <typename Type>
class KAIStack;

template <typename Type>
class KAINode;
template <typename Type>
class KAIVal;

//////////////////////////////////////////////////////////////////////////////////////////////
//	LIB

class KAI_LIB
{
public:
	virtual void amp_uninitialize() = 0;
	virtual KAIController<float>* GetKAIController_float() = 0;
	virtual KAIController<double>* GetKAIController_double() = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////
//
//	Controller
//
//////////////////////////////////////////////////////////////////////////////////////////////

/*
	KAI Controrller
*/
template <typename Type>
class KAIController
{
protected:
	KAIController() {}
	~KAIController() {}
public:
	virtual void DESTRUCTOR() = 0;

	bool DestroyedState = false;

	KAIRefController<KAIFlow<Type>, Type>* Flows;
	KAIRefController<KAINode<Type>, Type>* Nodes;
	KAIRefController<KAIVal<Type>, Type>* Vals;

	bool IsMakeStackMemory = true;
	virtual void RememberStack(KAIStack<Type>* RequiredStack) = 0;
public:
	KAIFlow<Type>* LastFlow = nullptr;
	KAIFlow<Type>* MainFlow = nullptr;
public:
	virtual KAIVal<Type>* newVal(int Size) = 0;
	virtual KAIFlow<Type>* newFlow() = 0;
	virtual KAINode<Type>* newNode(std::vector<int> Dimention, KAIVal<Type>* ValPtr = NULL) = 0;
};

/*
	KAI RefController
	Sub class for KAI Controller
*/
template <typename ConType, typename ValType>
class KAIRefController
{
protected:
	KAIRefController() {}
	~KAIRefController() {}
public:
	virtual void DESTRUCTOR() = 0;

	KAIController<ValType>* Owner = nullptr;
	std::map<ConType*, int> Map;

	virtual void Add(ConType* Target) = 0;
	virtual void Remove(ConType* Target) = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////
//
//	Flow
//
//////////////////////////////////////////////////////////////////////////////////////////////

/*
	KAI Flow
*/
template <typename Type>
class KAIFlow
{
protected:
	KAIFlow() {}
	~KAIFlow() {}
public:
	virtual void DESTRUCTOR() = 0;

	KAIController<Type>* Owner = nullptr;
	std::vector<KAIStack<Type>*> Stacks;

	virtual void through() = 0;
	virtual void trace(KAIDiffResult<Type>& DiffResultContainer, KAINode<Type>& DiffTarget) = 0;
	virtual KAIDiffResult<Type>* GetEmptyDiffResult(KAIController<Type>* Controller) = 0;
};

/*
	KAI Stack

	stack is hide
*/
template <typename Type>
class KAIStack
{
protected:
	KAIStack() {}
	~KAIStack() {}
public:
	virtual void DESTRUCTOR() = 0;

	KAIFlow<Type>* Owner = nullptr;

	virtual void calc() = 0;
	virtual void diff(KAIDiffResult<Type>& Con) = 0;
};

/*
	KAI Diff Result
*/
template <typename Type>
class KAIDiffResult
{
protected:
	KAIDiffResult() {}
	~KAIDiffResult() {}
public:
	virtual void DESTRUCTOR() = 0;

	KAIController<Type>* Based;
	KAIFlow<Type>* DiffFlow;	// DO not change this.
	std::map<KAINode<Type>*, KAINode<Type>*> Map;
public:
	virtual KAINode<Type>& Operator_Bracket(KAINode<Type>& Key, int InitDiff = 0) = 0;
	KAINode<Type>& operator()(KAINode<Type>& Key, int InitDiff = 0)
	{
		return Operator_Bracket(Key, InitDiff);
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////
//
//	Node
//
//////////////////////////////////////////////////////////////////////////////////////////////

// matrix operator
//
// ex) Node_A.matrix(3) + Node_B

template <typename Type>
class KAIMatrixOperator
{
public:
	virtual KAINode<Type>& Operator_Mul(KAINode<Type>& Node) = 0;
	virtual KAINode<Type>& Operator_Div(KAINode<Type>& Node) = 0;
	KAINode<Type>& operator * (KAINode<Type>& Node) { return Operator_Mul(Node); }
	KAINode<Type>& operator / (KAINode<Type>& Node) { return Operator_Div(Node); }

	virtual void close() = 0;
};

/*
	KAI Node
*/
template <typename Type>
class KAINode
{
protected:
	KAINode() {}
	~KAINode() {}
public:
	virtual void DESTRUCTOR() = 0;

	KAIController<Type>* Owner = nullptr;
	KAIController<Type>* User = nullptr;

	bool TempUser = false;
	bool ValIsCreatedByThis = false; // ?
	KAIVal<Type>* ValRef = nullptr;
	std::vector<int> Dimention;

	KAINode<Type>* TransposedNode = nullptr; // temp, unfinished
	KAINode<Type>* RowSumNode = nullptr; // temp, unfinished
	KAINode<Type>* ColumnSumNode = nullptr; // temp, unfinished

public:
	virtual KAIVal<Type>* getVal() = 0;
	virtual const int getSize() = 0;
	virtual const std::vector<int>& getDimention() = 0;

	virtual KAINode<Type>& setUser(KAIController<Type>* ChangeUser, bool TempUser = true) = 0;
	virtual KAIController<Type>* getUser() = 0;
	virtual void init(Type InitVal) = 0;

	// Operator
public:
	virtual KAINode<Type>& Operator_Add(Type Const) = 0;
	virtual KAINode<Type>& Operator_Sub(Type Const) = 0;
	virtual KAINode<Type>& Operator_Mul(Type Const) = 0;
	virtual KAINode<Type>& Operator_Div(Type Const) = 0;
	KAINode<Type>& operator + (Type Const) { return Operator_Add(Const); }
	KAINode<Type>& operator - (Type Const) { return Operator_Sub(Const); }
	KAINode<Type>& operator * (Type Const) { return Operator_Mul(Const); }
	KAINode<Type>& operator / (Type Const) { return Operator_Div(Const); }

	virtual KAINode<Type>& Operator_Add(KAINode<Type>& Node) = 0;
	virtual KAINode<Type>& Operator_Sub(KAINode<Type>& Node) = 0;
	virtual KAINode<Type>& Operator_Mul(KAINode<Type>& Node) = 0;
	virtual KAINode<Type>& Operator_Div(KAINode<Type>& Node) = 0;
	KAINode<Type>& operator + (KAINode<Type>& Node) { return Operator_Add(Node); }
	KAINode<Type>& operator - (KAINode<Type>& Node) { return Operator_Sub(Node); }
	KAINode<Type>& operator * (KAINode<Type>& Node) { return Operator_Mul(Node); }
	KAINode<Type>& operator / (KAINode<Type>& Node) { return Operator_Div(Node); }

	virtual void Operator_Assign_Add(KAINode<Type>& Node) = 0;
	virtual void Operator_Assign_Sub(KAINode<Type>& Node) = 0;
	virtual void Operator_Assign_Mul(KAINode<Type>& Node) = 0;
	virtual void Operator_Assign_Div(KAINode<Type>& Node) = 0;
	void operator += (KAINode<Type>& Node) { Operator_Assign_Add(Node); }
	void operator -= (KAINode<Type>& Node) { Operator_Assign_Sub(Node); }
	void operator /= (KAINode<Type>& Node) { Operator_Assign_Mul(Node); }
	void operator *= (KAINode<Type>& Node) { Operator_Assign_Div(Node); }

	virtual void Operator_Assign(Type TypeVal) = 0;
	virtual void Operator_Assign(KAINode<Type>& Node) = 0;
	void operator = (Type TypeVal) { Operator_Assign(TypeVal); }
	void operator = (KAINode<Type>& Node) { Operator_Assign(Node); }

	virtual KAINode<Type>& Operator_Power(int Times) = 0;
	KAINode<Type>& operator ^ (int Times) { return Operator_Power(Times); }

	// # Important
	// 1. Matrix is must be close when use not operater
	virtual KAIMatrixOperator<Type>& matrix() = 0;

	// transepose operator
	//

	virtual KAINode<Type>& transepose(int Row, int Column, std::vector<int> Dimention) = 0;
	virtual KAINode<Type>& transepose(int OverlapSize) = 0;
	//virtual KAINode<Type>& rowsum(int Row, int Column) = 0; // unfinished
	//virtual KAINode<Type>& columnsum(int Row, int Column) = 0; // unfinished

	// Compare
	//

	virtual KAINode<Type>& Operator_Compare_Bigger(const Type& Const1) = 0;
	virtual KAINode<Type>& Operator_Compare_Smaller(const Type& Const1) = 0;
	KAINode<Type>& operator > (const Type& Const1) { return Operator_Compare_Bigger(Const1); }
	KAINode<Type>& operator < (const Type& Const1) { return Operator_Compare_Smaller(Const1); }
};

//////////////////////////////////////////////////////////////////////////////////////////////
//
//	Node
//
//////////////////////////////////////////////////////////////////////////////////////////////

/*
	KAI VAL

	val is hide
*/
template <typename Type>
class KAIVal
{
protected:
	KAIVal(KAIController<Type>* Owner, int Size) : Owner(Owner), Size(Size) {}
	~KAIVal() {}
public:
	virtual void DESTRUCTOR() = 0;

	KAIController<Type>* Owner;
	int Size;
	virtual void* GetRef() = 0;
	virtual Type& Get(const int& Index) = 0;
	Type& operator[](const int& Index) { return Get(Index); }

	// calculator
	//

	virtual void calc_set_type(Type InitVal) = 0;
	virtual void calc_set_val(KAIVal<Type>* I1) = 0;
	virtual void calc_set_looparrange(KAIVal<Type>* I1, int I1arrange) = 0;
	virtual void calc_set_transpose(KAIVal<Type>* I1, int Row, int Column) = 0;
	// unfinished
	//     - rowsum
	//     - columsum
	virtual void calc_times(KAIVal* I1, int Loop) = 0;

	// Assign
	//

	virtual void calc_assign_add(Type Val) = 0;
	virtual void calc_assign_sub(Type Val) = 0;
	virtual void calc_assign_mul(Type Val) = 0;
	virtual void calc_assign_div(Type Val) = 0;
	virtual void calc_assign_add(KAIVal<Type>* I1) = 0;
	virtual void calc_assign_sub(KAIVal<Type>* I1) = 0;
	virtual void calc_assign_mul(KAIVal<Type>* I1) = 0;
	virtual void calc_assign_div(KAIVal<Type>* I1) = 0;

	// Array
	//

	virtual void calc_array_add(KAIVal<Type>* I1, Type Const1) = 0;
	virtual void calc_array_sub(KAIVal<Type>* I1, Type Const1) = 0;
	virtual void calc_array_mul(KAIVal<Type>* I1, Type Const1) = 0;
	virtual void calc_array_div(KAIVal<Type>* I1, Type Const1) = 0;
	virtual void calc_array_add(KAIVal<Type>* I1, KAIVal<Type>* I2) = 0;
	virtual void calc_array_sub(KAIVal<Type>* I1, KAIVal<Type>* I2) = 0;
	virtual void calc_array_mul(KAIVal<Type>* I1, KAIVal<Type>* I2) = 0;
	virtual void calc_array_div(KAIVal<Type>* I1, KAIVal<Type>* I2) = 0;
	virtual void calc_array_add_looparrange(KAIVal<Type>* I1, KAIVal<Type>* I2, int I1arrange, int I2arrange) = 0;
	virtual void calc_array_sub_looparrange(KAIVal<Type>* I1, KAIVal<Type>* I2, int I1arrange, int I2arrange) = 0;
	virtual void calc_array_mul_looparrange(KAIVal<Type>* I1, KAIVal<Type>* I2, int I1arrange, int I2arrange) = 0;
	virtual void calc_array_div_looparrange(KAIVal<Type>* I1, KAIVal<Type>* I2, int I1arrange, int I2arrange) = 0;

	// Matrix
	//

	virtual void calc_matrix_add(KAIVal<Type>* I1, KAIVal<Type>* I2, int Row, int Column, int Matrix) = 0;
	virtual void calc_matrix_sub(KAIVal<Type>* I1, KAIVal<Type>* I2, int Row, int Column, int Matrix) = 0;
	virtual void calc_matrix_mul(KAIVal<Type>* I1, KAIVal<Type>* I2, int Row, int Column, int Matrix) = 0;
	virtual void calc_matrix_div(KAIVal<Type>* I1, KAIVal<Type>* I2, int Row, int Column, int Matrix) = 0;

	// Compare
	//

	virtual void calc_compare_bigger(KAIVal<Type>* I1, Type Const1) = 0;
	virtual void calc_compare_smaller(KAIVal<Type>* I1, Type Const2) = 0;
};