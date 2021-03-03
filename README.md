# KAI

- controller

 변수를 선언하고 레퍼런스를 저장하는 공간. controller의 DESTRUCTOR를 호출하면 Controller를 Owner로 가지는 모든 Val, Node, Flow의 DESTRUCTOR를 호출한다.
 
    // Controller<float>
    // Controller<double>
 
- Val

 AMP 변수의 주소를 저장하는 공간. 클래스를 상속받아 새롭게 만들 수 있음. (연산 오버라이드 필요)

- Node

 오퍼라이터 오버라이드가 구현되어 있고 AMP 라이브러리의 변수를 가지고 있는 공간.

- Flow

 연산 순서가 정의되어 있는 공간.
 
 - Diff
 
  Flow를 받아 미분된 flow를 만드는 객체.

example
    
    ////
    // Make val
    
    auto CON = new KAI::Controller<float>();
    
    auto& N00 = *CON->newNode({ 100, 100}); // A
    auto& N01 = *CON->newNode({ 100, 100}); // B
    auto& N02 = *CON->newNode({ 100, 100}); // Output
    auto& N03 = *CON->newNode({ 100, 100}); // Desire
    
    //////
    // Make flow
    // f = Error = ( Desire - Output )^2, Output = A * B
    
    N02 = N00 * N01;
    auto& Error = ( N04 - N02 )^2;  // Error remember dynamic node.
    CON->MainFlow->through();       // Compute the f
    
    //////
    // Make diff
    
    auto Diff = CON->MainFlow->GetEmptyDiffResult(CON); // Make diff flow. Diff is returned pointer.
    CON->MainFlow->trace(*Diff, N03);                   // Make ∂f/∂A, ∂f/∂B,∂f/∂Desire...
    Diff->DiffFlow->through();                          // Compute the ∂f/∂A, ∂f/∂B,∂f/∂Desire...
    
    Diff->DiffFlow->DESTRUCTOR(); // release Diff.
    CON->MainFlow->DESTRUCTOR();  // release Controller. CON.Vals, CON.Nodes, CON.Flows are also destructed.
