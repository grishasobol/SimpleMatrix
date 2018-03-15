#include <iostream>
#include "Matrix.h"

using namespace std;

int main(int argc, char** argv) 
{
  MatrixBuff<int, 4, 4> m = {1,2,3,4,5,6,7,8,9};
  auto m1 = m;
  Matrix<int, 3, 3> matrix = {1,2,3,4,5,6,7,8,9};
  auto matrix1 = matrix;
  matrix = { 1,2,3,1 };
  matrix += matrix1;
  cout << matrix;
  auto matrix3 = matrix + matrix1;
  cout << matrix3;
  auto matrix4 = matrix * matrix1;
  cout << matrix4;
  Matrix<int, 3, 1> matrix5 = { 1, 1, 1};
  cout << matrix * matrix5;
  getchar();
  return 0;
}