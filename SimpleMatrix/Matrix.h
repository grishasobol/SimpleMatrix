#pragma once
#include <initializer_list>
#include <array>
#include <cassert>
#include <ostream>
#include <ios>
#include <type_traits>
#include <algorithm>
#include <set>
#include <numeric>

namespace sm {

  template <typename T>
  class MatrixIterator;

  template <typename T, size_t N, size_t M>
  class MatrixBuff {
  private:
    T *buffer;

    static void buff_copy(const T* begin, const T* end, T* buffer) {
      for (const T* t = begin; t != end; t++) {
        *buffer = *t;
        buffer++;
      }
    }
  public:
    static constexpr int size = N * M;

    typedef MatrixIterator<T> iterator;
    typedef MatrixIterator<const T> const_iterator;

    iterator begin() {
      return iterator(buffer, 0);
    }
    iterator end() {
      return iterator(buffer + size, size);
    }
    const_iterator begin() const {
      return const_iterator(buffer, 0);
    }
    const_iterator end() const {
      return const_iterator(buffer + size, size);
    }
    iterator at(size_t pos) {
      return iterator(buffer + pos, pos);
    }
    const_iterator at(size_t pos) const {
      return const_iterator(buffer + pos, pos)
    }
    iterator row_begin(size_t row_num) {
      return at(row_num * M);
    }
    const_iterator row_begin(size_t row_num) const {
      return at(row_num * M);
    }
    iterator row_end(size_t row_num) {
      return at(row_num * M + M);
    }
    const_iterator row_end(size_t row_num) const {
      return at(row_num * M + M);
    }
  public:
    MatrixBuff() {
      buffer = new T[size];
    }

    MatrixBuff(const MatrixBuff& MB) {
      buffer = new T[size];
      try {
        buff_copy(MB.buffer, MB.buffer + MB.size, buffer);
      }
      catch (...) {
        delete[] buffer;
        throw;
      }
    }

    MatrixBuff(MatrixBuff<T, N, M>&& MB) {
      delete[] buffer;
      buffer = MB.buffer;
      MB.buffer = nullptr;
    }

    template<typename K>
    MatrixBuff(const MatrixBuff<K, N, M>& MB) {
      buffer = new T[size];
      try {
        for (unsigned i = 0; i < size; i++) {
          this->set(i, static_cast<T>(MB[i]));
        }
      }
      catch (...) {
        delete[] buffer;
        throw;
      }
    }

    MatrixBuff(const std::initializer_list<T>& i_list) {
      assert(i_list.size() <= size && "Too long initializer list");
      buffer = new T[size];
      try
      {
        buff_copy(i_list.begin(), i_list.end(), buffer);
      }
      catch (...)
      {
        delete[] buffer;
        throw;
      }
    }

    MatrixBuff(const std::initializer_list<std::initializer_list<T>>& i_list)
    {
      assert(i_list.size() <= N && "Too many rows in initializer list");
      buffer = new T[size];
      unsigned pos = 0;
      try {
        for (auto row : i_list) {
          assert(row.size() <= M && "Too long row in initializer list");
          buff_copy(row.begin(), row.end(), buffer + pos);
          pos += M;
        }
      }
      catch (...) {
        delete[] buffer;
        throw;
      }
    }

    template<typename Arg>
    MatrixBuff& operator=(Arg&& arg) {
      MatrixBuff tmp(std::forward<Arg>(arg));
      std::swap(*this, tmp);
      return *this;
    }

    MatrixBuff& operator=(MatrixBuff&& MB) {
      delete[] buffer;
      buffer = MB.buffer;
      MB.buffer = nullptr;
      return *this;
    }

    T get(size_t n) const {
      assert(n < size && "Out of the boundaries");
      return buffer[n];
    }

    T get(size_t n, size_t m) const {
      assert(n < N && m < M && "Out of the boundaries");
      return buffer[n * M + m];
    }

    void set(size_t n, const T& value) const {
      assert(n < size && "Out of the boundaries");
      buffer[n] = value;
    }

    void set(size_t n, size_t m, const T& value) const {
      assert(n < N && m < M && "Out of the boundaries");
      buffer[n * M + m] = value;
    }

    T operator[](size_t n) const {
      return buffer[n];
    }
    T& operator[](size_t n) {
      return buffer[n];
    }

    ~MatrixBuff() {
      delete[] buffer;
    }
  };

  template <typename T, size_t N, size_t M>
  class Matrix : public MatrixBuff<T, N, M>
  {
    template <typename, size_t, size_t>
    friend class Matrix;

    // Evaluate matrix determinant by Gauss algorithm
    T gauss_det();
  public:
    Matrix() : MatrixBuff() {};
    Matrix(const Matrix<T, N, M>& MB) : MatrixBuff(MB) {}
    Matrix(Matrix<T, N, M>&& MB) : MatrixBuff(move(MB)) {}
    Matrix(const std::initializer_list<T>& i_list) : MatrixBuff(i_list) {}
    Matrix(const std::initializer_list<std::initializer_list<T>>& i_list) : MatrixBuff(i_list) {}
    template<typename K>
    Matrix(const Matrix<K, N, M>& MB) : MatrixBuff(MB) {}

    template<typename Arg>
    Matrix& operator=(Arg&& arg) {
      MatrixBuff::operator=(std::forward<Arg>(arg));
      return *this;
    };

    Matrix& operator=(Matrix&& MB) {
      MatrixBuff::operator=(move(MB));
      return *this;
    }

    Matrix& operator+=(const Matrix& rv) {
      for (unsigned i = 0; i < size; i++)
        (*this)[i] += rv[i];
      return *this;
    }

    Matrix& operator-=(const Matrix& rv) {
      for (unsigned i = 0; i < size; i++)
        (*this)[i] -= rv[i];
      return *this;
    }

    void row_transform(unsigned row1, unsigned row2, T factor) {
      assert(row1 < N && row2 < N && "Out of the boundaries");
      assert(row1 != row2 && "Row transformation persume different rows");
      for (unsigned i = 0; i < M; i++) {
        (*this)[row1*N + i] += factor * (*this)[row2*N + i];
      }
    }

    long double det() const;
  };

  template<typename T, size_t N, size_t M>
  Matrix<T, M, N> get_transp(const Matrix<T, N, M> matrix) {
    Matrix<T, M, N> tr_matrix;
    for (size_t row_num = 0; row_num < N; row_num++) {
      for (size_t pos1 = row_num * M, pos2 = row_num;
        pos1 < matrix.size && pos2 < tr_matrix.size;
        ++pos1, pos2 += N) {
        tr_matrix[pos2] = matrix[pos1];
      }
    }
    return tr_matrix;
  }

  template<typename T, size_t N, size_t M>
  T Matrix<T, N, M>::gauss_det() {
    T det = 1;

    std::array<uint16_t, M> excluded_columns = { false };

    // Row position of the last diagonal element, affect element parity
    size_t split_point = 0;

    // For each row in matrix
    for (unsigned row_num = 0; row_num < N; row_num++) {
      auto row_begin = this->at(row_num * M);
      auto row_end = this->at(row_num * M + M);

      // Search for abs_max element in row
      auto diagonal_elem = std::max_element(row_begin, row_end,
        [](T left, T right) {return std::abs(left) < std::abs(right); });

      // Return 0, if all element in row equal 0
      if (*diagonal_elem == 0)
        return 0;

      size_t diag_row_pos = diagonal_elem.get_pos() % M;

      // Evaluate parity of diagonal element
      size_t diag_elem_parity = accumulate(excluded_columns.begin() + diag_row_pos,
        excluded_columns.end(), 0);
      diag_elem_parity += row_num + diag_row_pos;

      // Mult determinant by diagonal elem considering parity
      if (diag_elem_parity % 2 == 0) {
        det *= *diagonal_elem;
      }
      else {
        det *= (-1) * (*diagonal_elem);
      }

      // Divide all elems in row by diagonal
      std::for_each(row_begin, row_end,
        [val = *diagonal_elem](T& elem) {elem /= val; });

      // Substract this row from the underlying rows to get zeros in column
      for (size_t row_num_next = row_num + 1; row_num_next < N; row_num_next++) {
        row_transform(row_num_next, row_num,
          -this->get(row_num_next, diag_row_pos));
        // Set zeros to avoid errors
        this->set(row_num_next, diag_row_pos, 0);
      }

      split_point = diag_row_pos;
      excluded_columns[diag_row_pos] = true;
    }
    return det;
  }

  template<typename T, size_t N, size_t M>
  long double Matrix<T, N, M>::det() const {
    static_assert(N == M,
      "Determinant can be evaluated only for square matrixes");
    static_assert(std::is_arithmetic<T>::value,
      "Determinant can be evaluated only for numerical matrixes ");

    constexpr bool is_float_point = std::is_floating_point<T>::value;
    double det;
    if (is_float_point) {
      Matrix<long double, N, M> tmp = Matrix(*this);
      det = static_cast<long double>(tmp.gauss_det());
    }
    else {
      Matrix<long double, N, M> tmp = Matrix(*this);
      det = static_cast<long double>(tmp.gauss_det());
    }
    return det;
  }

  template<typename T, size_t N, size_t M>
  inline bool operator==(const Matrix<T, N, M>& matrix1, const Matrix<T, N, M>& matrix2) {
    Matrix<T, N, M> m;
    for (size_t i = 0; i < m.size; i++)
      if (matrix1[i] != matrix2[i])
        return false;
    return true;
  }

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
      m[i] = matrix[i] * value;
    return m;
  }

  template<typename T, size_t N, size_t M>
  inline Matrix<T, N, M> operator*(const Matrix<T, N, M>& matrix, const T& value) {
    Matrix<T, N, M> m;
    for (unsigned i = 0; i < m.size; i++)
      m[i] = value * matrix[i];
    return m;
  }

  template<typename T, size_t N, size_t M, size_t K>
  inline Matrix<T, N, K> operator*(const Matrix<T, N, M>& matrix1, const Matrix<T, M, K>& matrix2) {
    Matrix<T, N, K> m;
    Matrix<T, K, M> tr_matrix2 = get_transp(matrix2);
    for (size_t pos = 0; pos < m.size; pos++) {
      m[pos] = 0;
      size_t row_num1 = pos / K;
      size_t row_num2 = pos % K;
      for (size_t pos2 = 0; pos2 < M; pos2++) {
        m[pos] += matrix1.get(row_num1, pos2) * tr_matrix2.get(row_num2, pos2);
      }
    }
    //for (size_t pos = 0; pos < m.size; pos++) {
    //  m[pos] = 0;
    //  unsigned row_num = M * (pos / K);
    //  unsigned col_num = pos % K;
    //  for (size_t pos1 = row_num, pos2 = col_num;
    //    pos1 < matrix1.size && pos2 < matrix2.size;
    //    ++pos1, pos2 += K) {
    //    m[pos] += matrix1[pos1] * matrix2[pos2];
    //  }
    //  /*while (row_num < matrix1.size && col_num < matrix2.size) {
    //    m[i] += matrix1[row_num] * matrix2[col_num];
    //    row_num += 1;
    //    col_num += K;
    //  }*/
    //}
    return m;
  }

  template<typename T, size_t N, size_t M>
  inline std::ostream& operator<<(std::ostream& os, const Matrix<T, N, M>& matrix) {
    os << "<MATRIX " << N << "*" << M << ">" << '\n';
    for (unsigned i = 0; i < matrix.size; i++) {
      os << matrix[i];
      if ((i + 1) % M == 0)
        os << '\n';
      else
        os << " ";
    }
    os << endl;
    return os;
  }


  template<typename T>
  class MatrixIterator : public std::iterator<std::forward_iterator_tag, T>
  {
    template<typename, size_t, size_t>
    friend class MatrixBuffer;
    template<typename, size_t, size_t>
    friend class Matrix;
  public:
    MatrixIterator(T* p, size_t pos);
  public:
    MatrixIterator(const MatrixIterator &it);

    bool operator!=(MatrixIterator const& other) const;
    bool operator==(MatrixIterator const& other) const;
    T& operator*() const;
    MatrixIterator& operator++();
    size_t get_pos() const {
      return pos;
    }
  private:
    T* p;
    size_t pos;
  };

  template<typename T>
  MatrixIterator<T>::MatrixIterator(T* p, size_t pos) : p(p), pos(pos) {}

  template<typename T>
  MatrixIterator<T>::MatrixIterator(const MatrixIterator &it) : p(it.p), pos(it.pos) {}

  template<typename T>
  bool MatrixIterator<T>::operator!=(MatrixIterator const& other) const {
    return p != other.p;
  }

  template<typename T>
  bool MatrixIterator<T>::operator==(MatrixIterator const& other) const {
    return p == other.p;
  }

  template<typename T>
  T& MatrixIterator<T>::operator*() const {
    return *p;
  }

  template<typename T>
  MatrixIterator<T>& MatrixIterator<T>::operator++() {
    ++p;
    ++pos;
    return *this;
  }
}