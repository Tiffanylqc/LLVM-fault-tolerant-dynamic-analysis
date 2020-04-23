

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "Tolerator.h"

using namespace llvm; 
using tolerator::Tolerator;


namespace tolerator {

char Tolerator::ID = 0;

}


bool
Tolerator::runOnModule(Module& m) {
  auto& context = m.getContext();
  auto& dataLayout = m.getDataLayout();

  // This analysis just prints a message when the program starts or exits.
  // You should modify this code as you see fit.
  auto* voidTy = Type::getVoidTy(context);
  auto* int32Ty = Type::getInt32Ty(context); 
  auto* int64Ty = Type::getInt64Ty(context); 
  auto* int8Ty = Type::getInt8Ty(context); 
  auto* int1Ty = Type::getInt1Ty(context); 
  auto* dblTy = Type::getDoubleTy(context); 
  auto* ptrTy = dataLayout.getIntPtrType(context);

  new GlobalVariable( m,
                      int32Ty,
                      true,
                      GlobalValue::ExternalLinkage,
                      ConstantInt::get(int32Ty, static_cast<int>(this->analysisType), false),
                      "ToLeRaToR_analysistype");
  
  auto helloworld = m.getOrInsertFunction("ToLeRaToR_helloworld", voidTy);
  appendToGlobalCtors(m, llvm::cast<Function>(helloworld.getCallee()), 0);

  auto initmap = m.getOrInsertFunction("ToLeRaToR_initmap", voidTy);
  appendToGlobalCtors(m, llvm::cast<Function>(initmap.getCallee()), 0);

  auto goodbyeworld = m.getOrInsertFunction("ToLeRaToR_goodbyeworld", voidTy);
  appendToGlobalDtors(m, llvm::cast<Function>(goodbyeworld.getCallee()), 0);

  auto isBypass = m.getOrInsertFunction("ToLeRaToR_isbypass", int1Ty);
  auto isIgnoreOrDefault = m.getOrInsertFunction("ToLeRaToR_isignoreordefault", int1Ty);
  auto* helperTy1 = FunctionType::get(int1Ty, int32Ty, false);
  auto isDivideByZeroInt =  m.getOrInsertFunction("ToLeRaToR_isdividebyzeroint", helperTy1);

  std::vector<Type*> args2;
  args2.push_back(ptrTy);
  args2.push_back(int64Ty);
  auto *helperTy2 = FunctionType::get(voidTy, ArrayRef<Type*>(args2), false);
  auto mallocFunc = m.getOrInsertFunction("ToLeRaToR_malloc", helperTy2);
  auto globalFunc = m.getOrInsertFunction("ToLeRaToR_global", helperTy2);
  auto allocaFunc = m.getOrInsertFunction("ToLeRaToR_alloca", helperTy2);

  auto* helperTy4 = FunctionType::get(int1Ty, ptrTy, false);
  auto isFreeValid = m.getOrInsertFunction("ToLeRaToR_isfreevalid", helperTy4);

  std::vector<Type*> args5;
  args5.push_back(ptrTy);
  args5.push_back(int64Ty);
  auto* helperTy5 = FunctionType::get(int1Ty, args5, false);
  auto isLoadValid = m.getOrInsertFunction("ToLeRaToR_isloadvalid", helperTy5);
  auto isStoreValid = m.getOrInsertFunction("ToLeRaToR_isstorevalid", helperTy5);

  auto retFunc = m.getOrInsertFunction("ToLeRaToR_ret", voidTy);
  auto callEntryFunc = m.getOrInsertFunction("ToLeRaToR_callentry", voidTy);

  std::vector<Instruction*> divInsts;
  std::vector<Instruction*> callFreeInsts;
  std::vector<Instruction*> loadInsts;
  std::vector<Instruction*> storeInsts;
  std::vector<Instruction*> callEntrys;
  std::vector<Instruction*> retInsts;

  // bool isFirstFunc = true;
  bool isFirstBB = true;
  for (auto& f : m) {
    isFirstBB=true;
    for (auto& bb : f) {
      for (auto& i : bb) {
        // if(isFirstFunc){
        //   for(auto& g:m.globals()){
        //     if(g.getName().str().find("llvm.")==std::string::npos){
        //       handleGlobal(i, g, globalFunc.getCallee(), ptrTy);
        //     }
        //   }
        //   isFirstFunc = false;
        // }
        if(isFirstBB){
          callEntrys.push_back(&i);
          isFirstBB = false;

          for(auto& g:m.globals()){
            if(g.getName().str().find("llvm.")==std::string::npos){
              handleGlobal(i, g, globalFunc.getCallee(), ptrTy);
            }
          }
        }
        if(isDivision(i)){
          divInsts.push_back(&i);
        }else if(isAlloca(i)){
          handleAlloca(i, dataLayout, allocaFunc.getCallee(), context);
        }else if(isLoad(i)){
          loadInsts.push_back(&i);
        }else if(isStore(i)){
          storeInsts.push_back(&i);
        }else if(isCallMalloc(CallSite(&i))){
          handleCallMalloc(i, mallocFunc.getCallee(), dataLayout, context);
        }else if(isCallFree(CallSite(&i))){
          callFreeInsts.push_back(&i);
        }else if(isReturn(i)){
          retInsts.push_back(&i);
        }
      }
    }
  }

  for(auto* i: divInsts){
    handleIfDivideByZero(*i, context, isDivideByZeroInt.getCallee(), isBypass.getCallee());
  }
  for(auto* i: loadInsts){
    checkLoad(*i, isLoadValid.getCallee(), isBypass.getCallee(), context, ptrTy, dataLayout);
  }
  for(auto* i: storeInsts){
    checkStore(*i, isStoreValid.getCallee(), isBypass.getCallee(), context, ptrTy, dataLayout);
  }
  for(auto* i: callFreeInsts){
    checkFree(*i, isFreeValid.getCallee(), isBypass.getCallee(), context, ptrTy);
  }
  for(auto* i: retInsts){
    handleRet(*i, retFunc.getCallee());
  }
  for(auto* i: callEntrys){
    handleCallEntry(*i, callEntryFunc.getCallee());
  }
  
  return true;
}

bool
Tolerator::isDivision(Instruction &I){
  return (I.getOpcode() == Instruction::UDiv ||
          I.getOpcode() == Instruction::SDiv );
}

bool
Tolerator::isLoad(Instruction &I){
  return (I.getOpcode() == Instruction::Load);
}

bool
Tolerator::isStore(Instruction &I){
  return (I.getOpcode() == Instruction::Store);
}

bool
Tolerator::isAlloca(Instruction &I){
  return (I.getOpcode() == Instruction::Alloca);
}

bool
Tolerator::isCallMalloc(CallSite cs){
  if(!cs.getInstruction()){
    return false;
  }
  // Check whether the called function is directly invoked
  auto called = dyn_cast<Function>(cs.getCalledValue()->stripPointerCasts());
  if (!called) {
    return false;
  }
  return called->getName().str() == "malloc";
}

bool
Tolerator::isCallFree(CallSite cs){
  if(!cs.getInstruction()){
    return false;
  }
  // Check whether the called function is directly invoked
  auto called = dyn_cast<Function>(cs.getCalledValue()->stripPointerCasts());
  if (!called) {
    return false;
  }
  return called->getName().str() == "free";
}

bool
Tolerator::isReturn(Instruction &I){
  return (I.getOpcode() == Instruction::Ret);
}

void 
Tolerator::handleIfDivideByZero(llvm::Instruction& I, LLVMContext& context, llvm::Value* isDivideByZeroInt, llvm::Value* isBypass){
  Value* divisor = I.getOperand(1);
  Instruction** thenTerm;
  thenTerm = (Instruction**)malloc(sizeof(Instruction*));
  Instruction** elseTerm;
  elseTerm = (Instruction**)malloc(sizeof(Instruction*));

  //create call to isDividedByZero before I
  CallInst* callIsZero = CallInst::Create(isDivideByZeroInt, divisor,"checkzeroint", &I);
  
  // split to THEN and ELSE
  SplitBlockAndInsertIfThenElse(callIsZero, &I, thenTerm, elseTerm);
  BasicBlock* thenBlock = (*thenTerm)->getParent();
  BasicBlock* elseBlock = (*elseTerm)->getParent();

  auto* callIsBypass = CallInst::Create(isBypass, "div_zero_isbypass", (*thenTerm));
  auto* bbByPass = BasicBlock::Create(context, "div_zero_bypass", I.getFunction(), I.getParent());
  auto* callerFunc = I.getFunction();
  auto* callerFuncRetTy = callerFunc->getReturnType();
  if(callerFuncRetTy->isVoidTy()){
    auto* retInst = ReturnInst::Create(context, nullptr, bbByPass);
  } else {
    auto* zeroRetVal = Constant::getNullValue(callerFuncRetTy);
    auto* retInst = ReturnInst::Create(context, zeroRetVal, bbByPass);
  }
  auto* brTerminator = SplitBlockAndInsertIfThen(callIsBypass, (*thenTerm), false, nullptr, nullptr, nullptr, bbByPass);
  
  // replace div instrution with phi node
  auto* phiNode = PHINode::Create(I.getType(), 2, "div_phi", &I);
  auto* divTy = I.getType();
  auto* zeroDefaultVal = Constant::getNullValue(divTy);
  phiNode->addIncoming(zeroDefaultVal, (*thenTerm)->getParent());
  I.replaceAllUsesWith(phiNode);
  I.moveBefore(*elseTerm);
  phiNode->addIncoming(&I, elseBlock);
}

void
Tolerator::handleCallMalloc(Instruction &I, Value* func,  const DataLayout& DL, LLVMContext& context){
  auto* callInst = dyn_cast<CallInst>(&I);
  auto* size = callInst->getArgOperand(0);

  IRBuilder<> builder(&I);
  BasicBlock::iterator it(&I);
  it++;
  builder.SetInsertPoint(I.getParent(), it);
  auto* ptrToIntInst = builder.CreatePointerCast(callInst, DL.getIntPtrType(context), "callmalloc_cast");

  std::vector<Value*> args;
  args.push_back(ptrToIntInst);
  args.push_back(size);
  builder.CreateCall(func, ArrayRef<Value*>(args));  
}

void
Tolerator::handleRet(Instruction &I, Value* func){
  IRBuilder<> builder(&I);
  builder.CreateCall(func);
}

void
Tolerator::handleCallEntry(Instruction &I, Value* func){
  IRBuilder<> builder(&I);
  builder.CreateCall(func);
}

void
Tolerator::checkFree(Instruction &I, Value* isFreeValid, llvm::Value* isBypass, LLVMContext& context, Type* ptrTy){
  // auto* int64Ty = Type::getInt64Ty(context); 
  Instruction** thenTerm;
  thenTerm = (Instruction**)malloc(sizeof(Instruction*));
  Instruction** elseTerm;
  elseTerm = (Instruction**)malloc(sizeof(Instruction*));

  auto* callInst = dyn_cast<CallInst>(&I);
  auto* ptrVal = callInst->getArgOperand(0);
  auto* ptrToInt = CastInst::CreatePointerCast(ptrVal, ptrTy, "checkfree_ptrcast", &I);
  auto* callIsFreeValid = CallInst::Create(isFreeValid, ptrToInt, "checkfree_call", &I);

  // split to THEN and ELSE on whether load is valid
  // no need to deal with thenBlock, just jump to I as default
  SplitBlockAndInsertIfThenElse(callIsFreeValid, &I, thenTerm, elseTerm);

  // split elseBlock on whether analysis is bypass or not
  auto* callIsBypass = CallInst::Create(isBypass, "checkfree_isbypass", (*elseTerm));
  // block when bypass
  auto* bbByPass = BasicBlock::Create(context, "checkfree_bypass", I.getFunction(), I.getParent());
  auto* callerFunc = I.getFunction();
  auto* callerFuncRetTy = callerFunc->getReturnType();
  if(callerFuncRetTy->isVoidTy()){
    auto* retInst = ReturnInst::Create(context, nullptr, bbByPass);
  } else {
    auto* zeroRetVal = Constant::getNullValue(callerFuncRetTy);
    auto* retInst = ReturnInst::Create(context, zeroRetVal, bbByPass);
  }
  auto* brTerminator = SplitBlockAndInsertIfThen(callIsBypass, (*elseTerm), false, nullptr, nullptr, nullptr, bbByPass); 

  I.replaceAllUsesWith(I.getNextNode());
  I.moveBefore(*thenTerm);
}

void
Tolerator::handleGlobal(Instruction& I, Value& g, llvm::Value* func, Type* ptrTy){
  // auto* valTy = g.getValueType();
  auto* globalVal = dyn_cast<GlobalVariable>(&g);
  auto* valSize = ConstantExpr::getSizeOf(globalVal->getValueType());
  // auto* ptrToInt = CastInst::CreatePointerCast(globalVal, ptrTy, "global_ptrcast", &I);

  IRBuilder<> builder(&I);
  auto* ptrToIntInst = builder.CreatePointerCast(globalVal, ptrTy, "global_ptrcast");
  std::vector<Value*> args;
  args.push_back(ptrToIntInst);
  // args.push_back(ptrToInt);
  args.push_back(valSize);
  builder.CreateCall(func, ArrayRef<Value*>(args));  

}

void
Tolerator::handleAlloca(Instruction &I, const DataLayout& DL, llvm::Value* func, LLVMContext& context){
  auto* int64Ty = Type::getInt64Ty(context); 
  auto* allocInst = dyn_cast<AllocaInst>(&I);
  ConstantInt *CI = dyn_cast<ConstantInt>(allocInst->getArraySize());
  auto arraySize = CI->getZExtValue();
  uint64_t SizeInBytes = DL.getTypeAllocSize(allocInst->getAllocatedType());
  Value* allocBytesVal = ConstantInt::get(int64Ty, SizeInBytes * arraySize, false);  
  
  IRBuilder<> builder(&I);
  BasicBlock::iterator it(&I);
  it++;
  builder.SetInsertPoint(I.getParent(), it);
  auto* ptrToIntInst = builder.CreatePointerCast(allocInst, DL.getIntPtrType(context), "alloca_cast");

  std::vector<Value*> args;
  args.push_back(ptrToIntInst);
  args.push_back(allocBytesVal);
  builder.CreateCall(func, ArrayRef<Value*>(args));  
}


void
Tolerator::checkLoad(Instruction &I, Value* isLoadValid, llvm::Value* isBypass, LLVMContext& context, Type* ptrTy, const DataLayout& DL){
  auto* int64Ty = Type::getInt64Ty(context); 
  Instruction** thenTerm;
  thenTerm = (Instruction**)malloc(sizeof(Instruction*));
  Instruction** elseTerm;
  elseTerm = (Instruction**)malloc(sizeof(Instruction*));

  auto* loadInst = dyn_cast<LoadInst>(&I);
  Value* ptrVal = loadInst->getPointerOperand();
  //get value type of the pointer
  // operand type is always a pointer, so convert it to PointerType
  auto* ptrOpTy = dyn_cast<PointerType>(loadInst->getPointerOperandType());
  auto* ptrElementTy = ptrOpTy->getElementType();
  uint64_t SizeInBytes = DL.getTypeAllocSize(ptrElementTy);
  Value* loadBytesVal = ConstantInt::get(int64Ty, SizeInBytes, false); 
  auto* ptrToInt = CastInst::CreatePointerCast(ptrVal, ptrTy, "checkload_ptrcast", &I);

  std::vector<Value*> args;
  args.push_back(ptrToInt);
  args.push_back(loadBytesVal);
  auto* callIsLoadValid = CallInst::Create(isLoadValid, args,"checkload_call", &I);

  // split to THEN and ELSE on whether load is valid
  // no need to deal with thenBlock, just jump to I as default
  SplitBlockAndInsertIfThenElse(callIsLoadValid, &I, thenTerm, elseTerm);

  // split elseBlock on whether analysis is bypass or not
  auto* callIsBypass = CallInst::Create(isBypass, "checkload_isbypass", (*elseTerm));
  // block when bypass
  auto* bbByPass = BasicBlock::Create(context, "checkload_bypass", I.getFunction(), I.getParent());
  auto* callerFunc = I.getFunction();
  auto* callerFuncRetTy = callerFunc->getReturnType();
  if(callerFuncRetTy->isVoidTy()){
    auto* retInst = ReturnInst::Create(context, nullptr, bbByPass);
  } else {
    auto* zeroRetVal = Constant::getNullValue(callerFuncRetTy);
    auto* retInst = ReturnInst::Create(context, zeroRetVal, bbByPass);
  }
  auto* brTerminator = SplitBlockAndInsertIfThen(callIsBypass, (*elseTerm), false, nullptr, nullptr, nullptr, bbByPass);

  auto* phiNode = PHINode::Create(I.getType(), 2, "checkload_phi", &I);
  auto* zeroDefaultVal = Constant::getNullValue(I.getType());
  phiNode->addIncoming(zeroDefaultVal, (*elseTerm)->getParent());
  I.replaceAllUsesWith(phiNode);
  I.moveBefore(*thenTerm);
  phiNode->addIncoming(&I, (*thenTerm)->getParent());
}

void
Tolerator::checkStore(Instruction &I, Value* isStoreValid, llvm::Value* isBypass, LLVMContext& context, Type* ptrTy, const DataLayout& DL){
  auto* int64Ty = Type::getInt64Ty(context); 
  Instruction** thenTerm;
  thenTerm = (Instruction**)malloc(sizeof(Instruction*));
  Instruction** elseTerm;
  elseTerm = (Instruction**)malloc(sizeof(Instruction*));

  auto* storeInst = dyn_cast<StoreInst>(&I);
  Value* ptrVal = storeInst->getPointerOperand();
  auto* ptrToInt = CastInst::CreatePointerCast(ptrVal, ptrTy, "checkstore_ptrcast", &I);
  auto* ptrOpTy = dyn_cast<PointerType>(storeInst->getPointerOperandType());
  auto* ptrElementTy = ptrOpTy->getElementType();
  uint64_t SizeInBytes = DL.getTypeAllocSize(ptrElementTy);
  Value* loadBytesVal = ConstantInt::get(int64Ty, SizeInBytes, false); 

  std::vector<Value*> args;
  args.push_back(ptrToInt);
  args.push_back(loadBytesVal);
  auto* callIsStoreValid = CallInst::Create(isStoreValid, args, "checkstore_call", &I);
  // split to THEN and ELSE on whether load is valid
  // no need to deal with thenBlock, just jump to I as default
  SplitBlockAndInsertIfThenElse(callIsStoreValid, &I, thenTerm, elseTerm);

  // split elseBlock on whether analysis is bypass or not
  auto* callIsBypass = CallInst::Create(isBypass, "checkstore_isbypass", (*elseTerm));
  // block when bypass
  auto* bbByPass = BasicBlock::Create(context, "checkstore_bypass", I.getFunction(), I.getParent());
  auto* callerFunc = I.getFunction();
  auto* callerFuncRetTy = callerFunc->getReturnType();
  if(callerFuncRetTy->isVoidTy()){
    auto* retInst = ReturnInst::Create(context, nullptr, bbByPass);
  } else {
    auto* zeroRetVal = Constant::getNullValue(callerFuncRetTy);
    auto* retInst = ReturnInst::Create(context, zeroRetVal, bbByPass);
  }
  auto* brTerminator = SplitBlockAndInsertIfThen(callIsBypass, (*elseTerm), false, nullptr, nullptr, nullptr, bbByPass); 

  // I.replaceAllUsesWith(I.getNextNode());
  // I.moveBefore(*thenTerm);
}




