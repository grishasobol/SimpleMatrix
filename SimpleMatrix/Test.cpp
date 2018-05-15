#include <iostream>
#include <algorithm>
#include <numeric> 
#include <ctime>
#include <string>
#include <iomanip>
#include <vector>

#include "Matrix.h"

using namespace std;
using namespace sm;

void CHECK(bool expr, string name) {
  cout << left << setfill('-') << setw(50) << name + " "
    << "> " << (expr ? "[CORRECT]" : "[ERROR]") << endl;
}

template<typename... Args>
void CHECK(bool expr, string name, Args... args) {
  CHECK(expr, name);
  if (!expr) {
    constexpr int size = sizeof...(args);
    int dummy[size] = { (std::cout << args << " ", 0)... };
    cout << endl;
  }
}

template<typename T>
bool almost_equal(const T& num1, const T& num2, double acc) {
  bool result;
  if (num1 == 0 && num2 == 0) {
    result = true;
  }
  else {
    result = abs(num1 - num2) / (abs(num1) + abs(num2)) < acc;
  }
  return result;
}

template<typename IterType>
bool get_parity(IterType begin, IterType end) {
  bool parity = true;
  for (auto itr1 = begin; itr1 != end; ++itr1) {
    for (auto itr2 = itr1 + 1; itr2 != end; ++itr2) {
      if (*itr1 > *itr2){
        parity = !parity;
      }
    }
  }
  return parity;
}

template<typename T, size_t N, size_t M>
bool shuffle_matrix(Matrix<T, N, M>& matrix) {
  Matrix<T, N, M> tmp = matrix;
  static array<size_t, N> rows_nums;
  iota(rows_nums.begin(), rows_nums.end(), 0);
  random_shuffle(rows_nums.begin(), rows_nums.end());
  for (size_t row_num = 0; row_num < N; row_num++) {
    for (size_t index = rows_nums[row_num] * M, index1 = row_num * M; 
      index < rows_nums[row_num] * M + M; ++index, ++index1) {
      matrix[index1] = tmp[index];
    }
  }
  return get_parity(rows_nums.begin(), rows_nums.end());
}

template<typename T, size_t N, size_t M>
unsigned linear_change(Matrix<T, N, M>& matrix, unsigned max_factor) {
  size_t row_num = rand() % N;
  unsigned factor = rand() % (max_factor + 1) + 1;
  for_each(matrix.row_begin(row_num), matrix.row_end(row_num),
    [factor](T& elem) {elem *= factor; });
    return factor;
}

float float_rand(float LO, float HI) {
  return LO + static_cast <float> (rand()) / 
    (static_cast <float> (RAND_MAX / (HI - LO)));
}

template<typename T, size_t N, size_t M>
Matrix<T, N, M> gen_random_matrix(size_t scale) {
  Matrix<T, N, M> matrix;
  generate(matrix.begin(), matrix.end(), 
    [scale]() {return static_cast<T>(rand() % scale); });
  return matrix;
}

template<size_t N, size_t M>
Matrix<float, N, M> gen_random_matrix(float LO, float HI) {
  Matrix<float, N, M> matrix;
  generate(matrix.begin(), matrix.end(), [LO, HI]() {return float_rand(LO, HI); });
  return matrix;
}

template<typename T, size_t N, size_t M>
Matrix<T, N, M> gen_unit_matrix() {
  Matrix<T, N, M> matrix;
  size_t pos = 0;
  generate(matrix.begin(), matrix.end(),
    [&pos]() {return (pos % M == pos++ / M) ? 1 : 0; });
  return matrix;
}

int main(int argc, char** argv) 
{
  //srand(time(nullptr));
  srand(42);
  {
    Matrix<int, 3, 6> matrix;
    matrix[10] = 20;
    matrix[11] = 42;
    CHECK(matrix[10] == 20 && matrix[11] == 42, "CHECK SQUARE BRACKET OPERATOR");
  }
  {
    Matrix<int, 3, 6> matrix;
    matrix.set(10, 20);
    matrix.set(11, 42);
    CHECK(matrix[10] == 20 && matrix[11] == 42, "CHECK ONE-INDEX SET");
  } 
  {
    Matrix<int, 3, 6> matrix;
    matrix.set(1, 4, 20);
    matrix.set(2, 0, 42);
    CHECK(matrix[10] == 20 && matrix[12] == 42, "CHECK TWO-INDEX SET");
  }
  {
    Matrix<int, 3, 6> matrix;
    matrix.set(10, 20);
    matrix.set(11, 42);
    CHECK(matrix.get(10) == 20 && matrix.get(11) == 42, "CHECK ONE-INDEX GET");
  }
  {
    Matrix<int, 3, 6> matrix;
    matrix.set(10, 20);
    matrix.set(12, 42);
    CHECK(matrix.get(1, 4) == 20 && matrix.get(2, 0) == 42, "CHECK TWO-INDEX GET");
  }
  {
    auto i_list = { 1, 2, 3, 4, 5, 6, 7, 8 };
    Matrix<int, 2, 4> matrix = i_list;
    Matrix<int, 2, 4> matrix1;
    matrix1 = i_list;
    bool res = true;
    auto itr = i_list.begin();
    for (size_t i = 0; i < matrix.get_size(); i++) {
      if (matrix[i] != *itr) {
        res = false;
        break;
      }
      ++itr;
    }
    CHECK(res, "CHECK INIT-LIST CONSTRUCTOR");
  }
  {
    auto i_list = { 1, 2, 3, 4, 5, 6, 7, 8 };
    Matrix<int, 2, 4> matrix =
    {
      { 1, 2, 3, 4 },
      { 5, 6, 7, 8 }
    };
    bool res = true;
    auto itr = i_list.begin();
    for (size_t i = 0; i < matrix.get_size(); i++) {
      if (matrix[i] != *itr) {
        res = false;
        break;
      }
      ++itr;
    }
    CHECK(res, "CHECK TWO_DIM INIT-LIST CONSTRUCTOR");
  }
  {
    Matrix<int, 3, 6> matrix;
    *matrix.at(10) = 20;
    CHECK(*matrix.at(10) == 20, "CHECK ITERATOR AT");
  }
  {
    Matrix<int, 3, 2> matrix = { { 1,2 },{ 1,3 },{ 3, 4 } };
    int list[] = { 1, 2, 1, 3, 3, 4 };
    size_t count = 0;
    bool res = true;
    for (auto itr : matrix) {
      if (itr != list[count]) {
        res = false;
        break;
      }
      count++;
    }
    CHECK(res, "CHECK ITERATOR LOOP");
  }
  {
    Matrix<int, 3, 2> matrix1 = { { 1,2 },{ 1,3 },{ 3, 4 } };
    Matrix<int, 3, 2> matrix2 = { { 1,2 },{ 1,3 },{ 3, 4 } };
    CHECK(matrix1 == matrix2, "CHECK OPERATOR EQUAL");
  }
  {
    auto matrix1 = gen_random_matrix<int, 11111, 12222>(9999);
    auto matrix2 = matrix1;
    CHECK(matrix1 == matrix2, "CHECK COPY CONSTRUCTOR");
  }
  {
    auto matrix1 = gen_random_matrix<int, 11111, 12222>(9999);
    auto matrix2 = matrix1;
    auto matrix3 = move(matrix1);
    CHECK(matrix2 == matrix3, "CHECK MOVE CONSTRUCTOR");
  }
  {
    const size_t N = 11111;
    const size_t M = 12222;
    const size_t scale = 9999;
    auto matrix1 = gen_random_matrix<float, N, M>(scale);
    auto matrix2 = gen_random_matrix<float, N, M>(scale);
    auto matrix3 = matrix1 + matrix2;
    auto matrix4 = matrix3 - matrix2;
    CHECK(matrix4 == matrix1, "CHECK ADD SUB OPERATION");
  }

  //Check determinant in 3 steps

  // First - check that det is skew-symmetric function
  {
    const size_t N = 100;
    const size_t M = N;
    auto matrix = gen_random_matrix<int, N, M>(10);
    auto det1 = matrix.det();
    auto parity = shuffle_matrix(matrix);
    auto det2 = matrix.det();
    CHECK(almost_equal(det1, parity ? det2 : -det2, 0.001), 
      "CHECK SKEW-SYMMETRIC DET (INT MATRIX)",
      "det1 =", det1, "det2 =", det2, "diff =", parity ? (det1 - det2) : (det1 + det2),
      "parity =", parity);
  }
  {
    const size_t N = 100;
    const size_t M = N;
    const size_t scale = 10;
    auto matrix = gen_random_matrix<N, M>(-0.5f, 0.5f);
    auto det1 = matrix.det();
    auto parity = shuffle_matrix(matrix);
    auto det2 = matrix.det();
    CHECK(almost_equal(det1, parity ? det2 : -det2, 0.001),
      "CHECK SKEW-SYMMETRIC DET (FLOAT MATRIX)",
      "det1 =", det1, "det2 =", det2, "diff =", parity ? (det1 - det2) : (det1 + det2),
      "parity =", parity);
  }
  // Second - check that det is linear by matrix's rows
  {
    const size_t N = 100;
    const size_t M = N;
    const size_t max_factor = 10;
    auto matrix = gen_random_matrix<int, N, M>(10);
    auto det1 = matrix.det();
    auto factor = linear_change(matrix, max_factor);
    factor *= linear_change(matrix, max_factor);
    factor *= linear_change(matrix, max_factor);
    factor *= linear_change(matrix, max_factor);
    auto det2 = matrix.det();
    CHECK(almost_equal(det1 * factor, det2, 0.001) , 
      "CHECK LINEARITY DET (INT MATRIX)", "det1 * factor =", 
      det1 * factor, "det2 =", det2, "diff =", det1 * factor - det2, "factor =", factor);
  }
  {
    const size_t N = 100;
    const size_t M = N;
    const size_t max_factor = 10;
    auto matrix = gen_random_matrix<N, M>(-0.5f, 0.5f);
    auto det1 = matrix.det();
    auto factor = linear_change(matrix, max_factor);
    factor *= linear_change(matrix, max_factor);
    factor *= linear_change(matrix, max_factor);
    factor *= linear_change(matrix, max_factor);
    auto det2 = matrix.det();
    CHECK(almost_equal(det1 * factor, det2, 0.001),
      "CHECK LINEARITY DET (FLOAT MATRIX)", "det1 * factor =",
      det1 * factor, "det2 =", det2, "diff =", det1 * factor - det2, "factor =", factor);
  }
  // Third - check that determinant of unit matrix is 1
  {
    const size_t N = 100;
    const size_t M = N;
    auto matrix = gen_unit_matrix<int, N, M>();
    CHECK(matrix.det() == 1, "CHECK UNIT MATRIX DET (INT)");
  }
  {
    const size_t N = 100;
    const size_t M = N;
    auto matrix = gen_unit_matrix<float, N, M>();
    CHECK(matrix.det() == 1, "CHECK UNIT MATRIX DET (FLOAT)");
  }

  // Transp matrix
  {
    Matrix<int, 4, 7> matrix = 
    {
      {1, 2, 3, 4, 5, 6, 7},
      {8, 9 , 10, 11, 12, 13, 14},
      {15, 16, 17, 18, 19, 20, 21},
      {22, 23, 24, 25, 26, 27, 28}
    };
    Matrix<int, 7, 4> tr_matrix = 
    {
      {1, 8, 15, 22},
      {2, 9, 16, 23},
      {3, 10, 17, 24},
      {4, 11, 18, 25},
      {5, 12, 19, 26},
      {6, 13, 20, 27},
      {7, 14, 21, 28}
    };
    CHECK(tr_matrix == get_transp(matrix), "CHECK TRANSP MATRIX", tr_matrix, get_transp(matrix));
  }

  // Check square matrix multiplication: det(C) = det(A) * det(B), if C = A * B
  {
    const size_t N = 100;
    const size_t M = N;
    auto A = gen_random_matrix<int, N, M>(10);
    auto B = gen_random_matrix<int, N, M>(10);
    auto C = A * B;
    CHECK(almost_equal(C.det(), A.det() * B.det(), 0.001), "CHECK SQUARE MATRIX MULT (INT)");
  }
  {
    const size_t N = 100;
    const size_t M = N;
    auto A = gen_random_matrix<N, M>(-0.5f, 0.5f);
    auto B = gen_random_matrix<N, M>(-0.5f, 0.5f);
    auto C = A * B;
    auto c_det = C.det();
    auto ab_det = A.det() * B.det();
    CHECK(almost_equal(c_det, ab_det, 0.001), "CHECK SQUARE MATRIX MULT (FLOAT)",
     "det(C) =", c_det, "det(A) * det(B) =", ab_det);
  }
  // Diff size matrixes multiplication
  {
    Matrix<int, 3, 2> A = 
    { { 1,2 },
      { 1,3 },
      { 5,6 } 
    };
    Matrix<int, 2, 4> B = 
    { 
      { 1, 2, 3, 4},
      { 1, 3, 5, 6 } 
    };
    Matrix<int, 3, 4> C = 
    { 
      {3, 8, 13, 16},
      {4, 11, 18, 22},
      {11, 28, 45, 56}
    };
    CHECK(C == A * B, "CHECK DIFF SIZE MATRIX MULT", "C =", C, "A * B =", A * B);
  }
  // Large diff size matrix multiplication
  {
    auto A = gen_random_matrix<1111, 3321>(-0.5f, 0.5f);
    auto B = gen_random_matrix<3321, 567>(-0.5f, 0.5f);
    auto C = A * B;
    CHECK(true, "CHECK LARGE DIFF SIZE MATRIX MULT");
  }

  cout << "END TESTING" << endl;

  getchar();
  return 0;
}