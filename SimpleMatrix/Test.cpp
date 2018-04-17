#include <iostream>
#include "Matrix.h"
#include "MatrixPack.h"

using namespace std;

// #define CHECK(expr, )

int main(int argc, char** argv) 
{
  {
    MatrixBuff<int, 4, 4> m = { 1,2,3,4,5,6,7,8,9 };
    auto m1 = m;
    Matrix<int, 3, 3> matrix = { 1,2,3,4,5,6,7,8,9 };
    auto matrix1 = matrix;
    matrix = { 1,2,3,1 };
    matrix += matrix1;
    cout << matrix;
    auto matrix3 = matrix + matrix1;
    cout << matrix3;
    auto matrix4 = matrix * matrix1;
    cout << matrix4;
    Matrix<int, 3, 1> matrix5 = { 1, 1, 1 };
    cout << matrix * matrix5;
    matrix = { 1,2,3,1,2,3,0,0,1 };
    matrix.det();
  }
  {
    Matrix<int, 3, 3> matrix = { {1,2,3}, {1,3,4}, {5,6,7} };
    int summ = 0;
    for (auto itr : matrix) {
      summ += itr;
    }
    cout << summ << endl;

    cout << *std::max_element(matrix.begin(), matrix.end()) << endl;
    cout << *std::max_element(matrix.at(0), matrix.at(3)) << endl;

    *matrix.at(6) = 10;
    cout << matrix;

    *matrix.at(6) = -11;
    cout << *std::max_element(matrix.begin(), matrix.end(), 
      [](int a, int b) {return std::abs(a) < std::abs(b); }) 
      << endl;
  }
  {
    Matrix<int, 3, 3> matrix = 
    { 
      { 1,0,0 },
      { 0,2,0 },
      { 0,0,2 } 
    };
    cout << matrix.det() << endl;
  }
  

  getchar();
  return 0;
}