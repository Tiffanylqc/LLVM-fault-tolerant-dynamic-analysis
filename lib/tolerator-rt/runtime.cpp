
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

extern "C" {


// This macro allows us to prefix strings so that they are less likely to
// conflict with existing symbol names in the examined programs.
// e.g. TOLERATE(entry) yields ToLeRaToR_entry
#define TOLERATE(X) ToLeRaToR_##X

// map is in descending order of keys
// map stores all valid memory ranges
std::map<uintptr_t, uint64_t, std::greater<uintptr_t>>* TOLERATE(ptrtomap);
// map only stores memory ranges allocated by malloc
std::map<uintptr_t, uint64_t, std::greater<uintptr_t>>* TOLERATE(mallocmap);

// current frame of current function(collection of alloca)
std::vector<uintptr_t>* TOLERATE(curframe);

std::vector<std::vector<uintptr_t>*>* TOLERATE(frames);

extern int TOLERATE(analysistype);

void
TOLERATE(helloworld)() {
  printf("==============================\n"
         "\tHello, World!\n"
         "==============================\n");
}

void
TOLERATE(print)(){
  printf("========PRINT TOTAL MAP========\n");
  for(auto i: *TOLERATE(ptrtomap)){
    printf("%lu : %lu \n", i.first, i.second);
  }
  printf("========END========\n");
  printf("========PRINT MALLOC MAP========\n");
  for(auto i: *TOLERATE(mallocmap)){
    printf("%lu : %lu \n", i.first, i.second);
  }
  printf("========END========\n");
}

void
TOLERATE(goodbyeworld)() {
  printf("==============================\n"
         "\tGoodbye, World!\n"
         "==============================\n");
}

bool
TOLERATE(isdividebyzeroint)(int divisor) {
  if(divisor!=0){
    return false;
  }
  fprintf(stderr, "FOUND: Division by zero\n");
  if(TOLERATE(analysistype) == 0 || TOLERATE(analysistype) == 1) {
    exit(-1);
  }
  return true;
}

bool 
TOLERATE(isbypass)(){
  return TOLERATE(analysistype) == 3;
}

bool 
TOLERATE(isignoreordefault)(){
  return TOLERATE(analysistype) == 2 || TOLERATE(analysistype) == 1;
}

void
TOLERATE(initmap)(){
  TOLERATE(ptrtomap) = new std::map<uintptr_t, uint64_t, std::greater<uintptr_t>>();
  TOLERATE(mallocmap) = new std::map<uintptr_t, uint64_t, std::greater<uintptr_t>>();
  TOLERATE(curframe) = new std::vector<uintptr_t>();
  TOLERATE(frames) = new std::vector<std::vector<uintptr_t>*>();
}


void
TOLERATE(alloca)(uintptr_t base, uint64_t size){
  (*TOLERATE(ptrtomap))[base] = size; 
  (*TOLERATE(curframe)).push_back(base);
  // printf("alloca %lu %lu\n", base, size);
  // TOLERATE(print)();
}

void
TOLERATE(ret)(){
  for(auto i: *TOLERATE(curframe)){
    // printf("%lu : %lu \n", i.first, i.second);
    (*TOLERATE(ptrtomap)).erase(i);
  }
  if((*TOLERATE(frames)).size()>0){
    // printf("in ret\n");
    TOLERATE(curframe) = (*TOLERATE(frames)).back();
    (*TOLERATE(frames)).pop_back();
  } else {
    TOLERATE(curframe) = new std::vector<uintptr_t>();
  }
}

void
TOLERATE(callentry)(){
  // printf("in call entry\n");
  (*TOLERATE(frames)).push_back(TOLERATE(curframe));
  TOLERATE(curframe) = new std::vector<uintptr_t>();
}

void
TOLERATE(malloc)(uintptr_t base, uint64_t size){
  (*TOLERATE(mallocmap))[base] = size; 
  (*TOLERATE(ptrtomap))[base] = size; 
  // printf("malloc %lu %lu\n", base, size);
}

void
TOLERATE(global)(uintptr_t base, uint64_t size){
  (*TOLERATE(ptrtomap))[base] = size; 
  // TOLERATE(print)();
  // printf("global %lu %lu\n", base, size);
}

bool
TOLERATE(isloadvalid)(uintptr_t base, uint64_t size){
  // printf("load %lu %lu\n", base, size);
  std::map<uintptr_t, uint64_t>::iterator itlow;
  itlow = (*TOLERATE(ptrtomap)).lower_bound(base);

  // if can't find key <= base, throw error
  if(itlow==(*TOLERATE(ptrtomap)).end()){
    fprintf(stderr, "FOUND: Invalid read from memory\n");
    if(TOLERATE(analysistype) == 0 || TOLERATE(analysistype) == 1) {
      exit(-1);
    }
    return false; 
  }

  uintptr_t low_base = itlow->first;
  uint64_t low_size = itlow->second;
  if(low_base<=base&&base+size<=low_base+low_size){
    return true;
  } else {
    fprintf(stderr, "FOUND: Invalid read from memory\n");
    if(TOLERATE(analysistype) == 0 || TOLERATE(analysistype) == 1) {
      exit(-1);
    }
    return false;
  }
}

bool
TOLERATE(isstorevalid)(uintptr_t base, uint64_t size){
  std::map<uintptr_t,uint64_t>::iterator itlow;
  itlow = (*TOLERATE(ptrtomap)).lower_bound(base);
  // printf(" in checking store, base addr: %lu\n", base);
  // TOLERATE(print)();

  // if can't find key <= base, throw error
  if(itlow==(*TOLERATE(ptrtomap)).end()){
    fprintf(stderr, "FOUND: Invalid write to memory\n");
    if(TOLERATE(analysistype) == 0) {
      exit(-1);
    }
    return false;
  }

  uintptr_t low_base = itlow->first;
  uint64_t low_size = itlow->second;
  if(low_base<=base&&base+size<=low_base+low_size){
    return true;
  } else {
    fprintf(stderr, "FOUND: Invalid write to memory\n");
    //only terminate the program in log analysisType
    if(TOLERATE(analysistype) == 0){
      exit(-1);
    } 
    return false;
  }
}

bool
TOLERATE(isfreevalid)(uintptr_t ptr){
  // TOLERATE(print)();
  // printf("in free: %lu\n", ptr);

  // printf("find: %lu\n", (*TOLERATE(ptrtomap)).find(ptr)->first);
  // printf("find2: %lu\n", (*TOLERATE(ptrtomap)).find(ptr)->second);
  if(ptr!=0 && (*TOLERATE(mallocmap)).find(ptr)==(*TOLERATE(mallocmap)).end()){
    fprintf(stderr, "FOUND: Invalid free of memory\n");
    // printf("analysis type is %d\n", analysisType);
    //only terminate the program in log analysisType
    if(TOLERATE(analysistype) == 0){
      exit(-1);  
    }
    return false;
  }

  //if not null ptr, remove it from map
  if(ptr!=0){
    (*TOLERATE(mallocmap)).erase(ptr);
    (*TOLERATE(ptrtomap)).erase(ptr);
  }
  return true;
}
  
}
