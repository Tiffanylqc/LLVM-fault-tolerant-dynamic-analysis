
#pragma once


#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"


namespace tolerator {


enum class AnalysisType {
  LOGGING,
  IGNORING,
  DEFAULTING,
  BYPASSING
};


struct Tolerator : public llvm::ModulePass {
  static char ID;
  AnalysisType analysisType;

  Tolerator(AnalysisType analysisType) : llvm::ModulePass(ID), analysisType(analysisType) {}

  bool runOnModule(llvm::Module& m) override;
  void handleIfDivideByZero(llvm::Instruction &I, llvm::LLVMContext& context, llvm::Value* isDivideByZeroInt, llvm::Value* isBypass);
  bool isDivision(llvm::Instruction &I);
  bool isAlloca(llvm::Instruction &I);
  bool isStore(llvm::Instruction &I);
  bool isLoad(llvm::Instruction &I);
  bool isCallMalloc(llvm::CallSite cs);
  bool isCallFree(llvm::CallSite cs);
  bool isCallEntry(llvm::CallSite cs);
  bool isReturn(llvm::Instruction &I);
  void handleCallMalloc(llvm::Instruction &I, llvm::Value* func, const llvm::DataLayout &DL, llvm::LLVMContext& context);
  void checkFree(llvm::Instruction &I, llvm::Value* isLoadValid, llvm::Value* isBypass, llvm::LLVMContext& context, llvm::Type* ptrTy);
  void checkLoad(llvm::Instruction &I, llvm::Value* isLoadValid, llvm::Value* isBypass, llvm::LLVMContext& context, llvm::Type* ptrTy, const llvm::DataLayout &DL);
  void checkStore(llvm::Instruction &I, llvm::Value* isStoreValid, llvm::Value* isBypass, llvm::LLVMContext& context, llvm::Type* ptrTy, const llvm::DataLayout &DL);
  void handleAlloca(llvm::Instruction &I, const llvm::DataLayout &DL, llvm::Value* func, llvm::LLVMContext& context);
  void handleGlobal(llvm::Instruction &I, llvm::Value& g, llvm::Value* func, llvm::Type* ptrTy);
  void handleRet(llvm::Instruction &I, llvm::Value* func);
  void handleCallEntry(llvm::Instruction &I, llvm::Value* func);
};


}  // namespace tolerator


