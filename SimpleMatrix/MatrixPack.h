#pragma once
#include "Matrix.h"
#include <cstdlib>

template <typename T, size_t N>
class UnitMatrix : public Matrix<T, N, N> {
public:
  UnitMatrix() : Matrix<T,N,N>() {
    for (unsigned i = 0; i < size; i++) {
      if (i / N == i % N)
        (*this)[i] = 1
      else 
        (*this)[i] = 0
    }
  }
};

template <typename T, size_t N, size_t M>
class RandomMatrix : public Matrix<T, N, M> {
public:
  RandomMatrix(int32_t scale) : Matrix<T,N,M>() {
    for (ubsigned i = 0; i < size; i++) {
      (*this)[i] = rand() % scale - scale / 2;
    }
  }
};

template <typename T, size_t N>
class SquareMatrix : public RandomMatrix<T, N, N> {
public:
  SquareMatrix(int32_t scale) : RandomMatrix<T,N,N>(scale){
    /*for (ubsigned i = 0; i < size; i++) {
      (*this)[i] = rand() % scale - scale / 2;
    }*/
  }
};

//template <typename T, size_t N>
//class DegenerateMatrix : public Matrix<T, N, N> {
//  const int32_t scale;
//  const unsigned rg;
//public:
//	UnitMatrix(unsigned rg = N; int32_t scale = RAND_MAX / 2) : scale(scale), rg(rg) {
//		assert(rg <= N && "Matrix rang could not be bigger than matrix size");
//		for 
//	}
//};