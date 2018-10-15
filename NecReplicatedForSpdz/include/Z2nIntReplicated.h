#ifndef Z2N_INT_REPLICATED_H_
#define Z2N_INT_REPLICATED_H_

using namespace std;

#include <stdint.h>
#include <iostream>

template <typename T>
class Z2nIntReplicated {

 private:
  static const unsigned int n = 32; //
  uint32_t numOfData;

 public:
  T *elem;
  
  Z2nIntReplicated() {elem = NULL;};
  Z2nIntReplicated(uint32_t num)
    {
      numOfData = num;
      elem = new T[numOfData];
      memset(elem, 0, numOfData*sizeof(T));
    }

  uint32_t getNumOfData() {return numOfData;}

  void init(uint32_t num) {
    if (elem != NULL)
      {
	cout << "[Z2nIntReplicated] init failed: already allocated" << endl;
	exit(1);
      }
    numOfData = num;
    elem = new T[numOfData];
    memset(elem, 0, numOfData*sizeof(T));
  }
  
  Z2nIntReplicated& operator=(const Z2nIntReplicated& other) {
    if (numOfData != other.numOfData) {
      cout << "[Z2nIntReplicated] operator= failed: sizes are different" << endl;
      exit(1);
    }
    for (int i=0; i<numOfData; i++) { elem[i] = other.elem[i]; }
    return *this;
  }
  
  bool operator!=(const Z2nIntReplicated& other) {
    for (int i=0; i<numOfData; i++) {
      if (elem[i] != other.elem[i]) return false;
    }
    return true;
  }
  
  Z2nIntReplicated& operator+(const Z2nIntReplicated& other)
    {
      Z2nIntReplicated tmp(numOfData);
      tmp.numOfData = numOfData;
#pragma ivdep
      for (int i=0; i<numOfData; i++) {
	tmp.elem[i] = elem[i] + other.elem[i];
      }
      return tmp;
    }
  
  Z2nIntReplicated& operator-(const Z2nIntReplicated& other)
    {
      Z2nIntReplicated tmp(numOfData);
      tmp.numOfData = numOfData;
#pragma ivdep
      for (int i=0; i<numOfData; i++) {
	tmp.elem[i] = elem[i] - other.elem[i];
      }
      return tmp;
    }
  Z2nIntReplicated& operator*(const Z2nIntReplicated& other)
    {
      Z2nIntReplicated tmp(numOfData);
      tmp.numOfData = numOfData;
#pragma ivdep
      for (int i=0; i<numOfData; i++) {
	tmp.elem[i] = elem[i] * other.elem[i];
      }
      return tmp;
    }
  
  Z2nIntReplicated& operator+=(const Z2nIntReplicated& other)
    {
#pragma ivdep
      for (int i=0; i<numOfData; i++) {
	elem[i] = elem[i] + other.elem[i];
      }
      return *this;
    }

  Z2nIntReplicated& operator-=(const Z2nIntReplicated& other)
    {
#pragma ivdep
      for (int i=0; i<numOfData; i++) {
	elem[i] = elem[i] - other.elem[i];
      }
      return *this;
    }

  Z2nIntReplicated& operator*=(const Z2nIntReplicated& other)
    {
#pragma ivdep
      for (int i=0; i<numOfData; i++) {
	elem[i] = elem[i] * other.elem[i];
      }
      return *this;
    }
  
  unsigned int getModulus() {return n;}

  void dump() {
    for (int i=0; i<numOfData; i++) {
      cout << "i=" << i << ": " << elem[i] <<endl;
    }
  }
  
};

#endif //Z2N_INT_REPLICATED_H_
