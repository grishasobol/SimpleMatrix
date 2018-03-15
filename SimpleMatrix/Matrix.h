#pragma once
#include <initializer_list>
#include <cassert>
#include <ostream>

template <typename T, size_t N, size_t M>
class MatrixBuff {
private:
  T *buffer;
public:
  static constexpr int size = N * M;

  static void buff_copy(const T* begin, const T* end, T* buffer) {
    for (const T* t = begin; t != end; t++) {
      *buffer = *t;
      buffer++;
    }
  }

  MatrixBuff() {
    buffer = new T[size];
  }
  MatrixBuff(T*&& buffer) : buffer(buffer){
    buffer = nullptr;
  }
  MatrixBuff(const std::initializer_list<T>& i_list) {
    buffer = new T[size];
    buff_copy(i_list.begin(), i_list.end(), buffer);
  }
  MatrixBuff<T, N, M>& operator=(const std::initializer_list<T>& i_list) {
    buff_copy(i_list.begin(), i_list.end(), buffer);
    return *this;
  }
  MatrixBuff(const MatrixBuff<T, N, M>& MB) {
    buffer = new T[size];
    buff_copy(MB.buffer, MB.buffer + MB.size, buffer);
  }
  MatrixBuff<T, N, M>& operator=(const MatrixBuff<T, N, M>& MB) {
    buff_copy(MB.buffer, MB.buffer + MB.size, buffer);
    return *this;
  }
  MatrixBuff(MatrixBuff<T, N, M>&& MB) {
    buffer = MB.buffer;
    MB.buffer = nullptr;
  }
  MatrixBuff<T, N, M>& operator=(MatrixBuff<T, N, M>&& MB) {
    buffer = MB.buffer;
    MB.buffer = nullptr;
  }
  ~MatrixBuff() {
    delete[] buffer;
  }

  T get(size_t n) const {
    assert(n < size && "Out of the boundaries");
    return buffer[n];
  }

  T get(size_t n, size_t m) const {
    assert(n < N && m < M && "Out of the boundaries");
    return buffer[n * N + m];
  }

  void set(size_t n, const T& value) const {
    assert(n < size && "Out of the boundaries");
    buffer[n] = value;
  }

  void set(size_t n, size_t m, const T& value) const {
    assert(n < N && m < M && "Out of the boundaries");
    buffer[n * N + m] = value;
  }

  T& operator[](size_t n) const {
    return buffer[n];
  }
};

template <typename T, size_t N, size_t M>
class Matrix : public MatrixBuff<T, N, M>
{
public:
  Matrix() : MatrixBuff() {};
  Matrix(T*&& buffer) : MatrixBuff(move(buffer)) {}
  Matrix(const std::initializer_list<T>& i_list) : MatrixBuff(i_list) {}
  Matrix(const Matrix<T, N, M>& MB) : MatrixBuff(MB) {}
  Matrix(Matrix<T, N, M>&& MB) : MatrixBuff(move(MB)) {}
  Matrix<T, N, M>& operator=(const std::initializer_list<T>& i_list) {
    MatrixBuff::operator=(i_list);
    return *this;
  }
  Matrix& operator=(const Matrix& MB) {
    MatrixBuff::operator=(MB);
    return *this;
  }
  Matrix& operator=(Matrix&& MB) {
    MatrixBuff::operator=(move(MB));
    return *this;
  }

  Matrix& operator+=(const Matrix& rv){
    for (unsigned i = 0; i < size; i++)
      (*this)[i] += rv[i];
    return *this;
  }

  Matrix& operator-=(const Matrix& rv) {
    for (unsigned i = 0; i < size; i++)
      (*this)[i] -= rv[i];
    return *this;
  }

  bool operator==(const Matrix& rv) {
    for (unsigned i = 0; i < size; i++)
      if (this->get(i) != rv[i])
        return false;
    return true;
  }
};

template<typename T, size_t N, size_t M>
inline Matrix<T, N, M> operator+(const Matrix<T, N, M>& matrix1, const Matrix<T, N, M>& matrix2) {
  Matrix<T, N, M> m;
  for (unsigned i = 0; i < m.size; i++)
    m[i] = matrix1[i] + matrix2[i];
  return m;
}

template<typename T, size_t N, size_t M>
inline Matrix<T, N, M> operator-(const Matrix<T, N, M>& matrix1, const Matrix<T, N, M>& matrix2) {
  Matrix<T, N, M> m;
  for (unsigned i = 0; i < m.size; i++)
    m[i] = matrix1[i] - matrix2[i];
  return m;
}

template<typename T, size_t N, size_t M>
inline Matrix<T, N, M> operator*(const T& value, const Matrix<T, N, M>& matrix) {
  Matrix<T, N, M> m;
  for (unsigned i = 0; i < m.size; i++)
    m[i] = matrix1[i] * value;
  return m;
}

template<typename T, size_t N, size_t M>
inline Matrix<T, N, M> operator*(const Matrix<T, N, M>& matrix, const T& value) {
  Matrix<T, N, M> m;
  for (unsigned i = 0; i < m.size; i++)
    m[i] = value * matrix1[i];
  return m;
}

template<typename T, size_t N, size_t M, size_t K>
inline Matrix<T, N, K> operator*(const Matrix<T, N, M>& matrix1, const Matrix<T, M, K>& matrix2) {
  Matrix<T, N, K> m;
  for (unsigned i = 0; i < m.size; i++) {
    m[i] = 0;
    unsigned raw_num = N * (i / K);
    unsigned col_num = i % K;
    while (raw_num < matrix1.size && col_num < matrix2.size) {
      m[i] += matrix1[raw_num] * matrix2[col_num];
      raw_num += 1;
      col_num += K;
    }
  }
  return m;
}

template<typename T, size_t N, size_t M> 
inline std::ostream& operator<<(std::ostream& os, const Matrix<T, N, M>& matrix) {
  for (unsigned i = 0; i < matrix.size; i++) {
    os << matrix[i];
    if ((i + 1) % N == 0)
      os << endl;
    else
      os << " ";
  }
  return os;
}