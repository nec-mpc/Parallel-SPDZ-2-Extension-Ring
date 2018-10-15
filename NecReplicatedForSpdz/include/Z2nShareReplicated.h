#ifndef Z2N_SHARE_REPLICATED_H_
#define Z2N_SHARE_REPLICATED_H_

using namespace std;

#include <stdint.h>
#include <iostream>

template <typename T>
class Z2nShareReplicated {
 private:
  static const unsigned int n = 32; // tentative for debug
  uint32_t numOfShares;

 public:
  T *elem1;
  T *elem2;
  
  Z2nShareReplicated() {elem1=NULL; elem2=NULL; numOfShares=0;}
  Z2nShareReplicated(uint32_t num) {
    numOfShares = num;
    elem1 = new T[numOfShares];
    elem2 = new T[numOfShares];
    memset(elem1,0,numOfShares*sizeof(T));
    memset(elem2,0,numOfShares*sizeof(T));
  }

  uint32_t getNumOfShares() {return numOfShares;}

  void init(uint32_t num) {
    if ((elem1 != NULL) || (elem2!= NULL))
      {
	cout << "[Z2nShareReplicated] init failed: already allocated"<< endl;
	exit(1);
      }
    numOfShares = num;
    elem1 = new T[numOfShares];
    elem2 = new T[numOfShares];
    memset(elem1, 0, numOfShares*sizeof(T));
    memset(elem2, 0, numOfShares*sizeof(T));
  }

  Z2nShareReplicated& operator=(const Z2nShareReplicated& other) {
#pragma ivdep
    if (numOfShares != other.numOfShares) {
      cout << "[Z2nShareReplicated] operator= failed: sizes are different" << endl;
      exit(1);
    }
    for (int i=0; i<numOfShares; i++) {
      elem1[i] = other.elem1[i];
      elem2[i] = other.elem2[i];
    }
    return *this;
  }

  bool operator !=(const Z2nShareReplicated& other) {
    for (int i=0; i<numOfShares; i++) {
      if (elem1[i] != other.elem1[i] || elem2[i] != other.elem2[i]) return false;
    }
    return true;
  }

  Z2nShareReplicated operator+(const Z2nShareReplicated& other) {
    Z2nShareReplicated tmp(numOfShares);
    tmp.numOfShares = numOfShares;
#pragma ivdep
    for (int i=0; i<numOfShares; i++) {
      tmp.elem1[i] = elem1[i] + other.elem1[i];
      tmp.elem2[i] = elem2[i] + other.elem2[i];
    }
    return tmp;
  }


  Z2nShareReplicated operator-(const Z2nShareReplicated& other) {
    Z2nShareReplicated tmp(numOfShares);
    tmp.numOfShares = numOfShares;
#pragma ivdep
    for (int i=0; i<numOfShares; i++) {
      tmp.elem1[i] = elem1[i] - other.elem1[i];
      tmp.elem2[i] = elem2[i] - other.elem2[i];
    }
    return tmp;
  }

  
  Z2nShareReplicated operator*(const Z2nShareReplicated& other) {
    Z2nShareReplicated tmp(numOfShares);
    tmp.numOfShares = numOfShares;
#pragma ivdep
    for (int i=0; i<numOfShares; i++) {
      tmp.elem1[i] = elem1[i] * other.elem1[i];
      tmp.elem2[i] = elem2[i] * other.elem2[i];
    }
    return tmp;
  }

  Z2nShareReplicated& operator+=(const Z2nShareReplicated& other) {
#pragma ivdep    
    for (int i=0; i<numOfShares; i++) {
      elem1[i] = elem1[i] + other.elem1[i];
      elem2[i] = elem2[i] + other.elem2[i];
    }
    return *this;
  }

  Z2nShareReplicated& operator-=(const Z2nShareReplicated& other) {
#pragma ivdep
    for (int i=0; i<numOfShares; i++) {
      elem1[i] = elem1[i] - other.elem1[i];
      elem2[i] = elem2[i] - other.elem2[i];
    }
    return *this;
  }

  Z2nShareReplicated& operator*=(const Z2nShareReplicated& other) {
#pragma ivdep
    for (int i=0; i<numOfShares; i++) {
      elem1[i] = elem1[i] * other.elem1[i];
      elem2[i] = elem2[i] * other.elem2[i];
    }
    return *this;
  }

  void dump() {
    for (int i=0; i<numOfShares; i++) {
      cout << "i=" << i << ": " << elem1[i]<< ", " << elem2[i] <<endl;
    }
  }

};

#endif //Z2N_SHARE_REPLICATED_H_
