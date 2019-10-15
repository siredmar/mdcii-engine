#ifndef MATRIX_HPP
#define MATRIX_HPP

// Source: https://stackoverflow.com/questions/52131718/rotate-mn-matrix-90-degrees-clockwise-c

#include <array>
#include <iostream>
#include <vector>

template<typename Matrix2D>
void print_matrix(Matrix2D const& vec)
{
  std::cout << "size of matrix is [" << vec[0].size() << " x " << vec.size() << "]\n";
  for (auto const& inner_vec : vec)
  {
    for (auto const& item : inner_vec)
    {
      std::cout << item << ", ";
    }
    std::cout << std::endl;
  }
}

template<typename OriginalMatrix2D, typename RotatedMatrix2D>
void rotate_2d_matrix_clockwise_impl(OriginalMatrix2D const& matrix, RotatedMatrix2D& rotated_matrix, int const M, int const N)
{
  for (auto x = 0; x < N; ++x)
  {
    for (auto y = 0; y < M; ++y)
    {
      // Source : https://stackoverflow.com/questions/4780119/2d-euclidean-vector-rotations
      rotated_matrix[y][-x - 1 + N] = matrix[x][y];
    }
  }
}

template<typename T, size_t M, size_t N>
auto rotate_2d_matrix_clockwise(std::array<std::array<T, M>, N> const& original_matrix) -> std::array<std::array<T, N>, M>
{
  std::array<std::array<T, N>, M> rotated_matrix;
  rotate_2d_matrix_clockwise_impl(original_matrix, rotated_matrix, M, N);
  return rotated_matrix;
}

template<typename Matrix2D>
auto rotate_2d_matrix_clockwise(Matrix2D const& original_matrix) -> Matrix2D
{
  int const M = original_matrix[0].size();
  int const N = original_matrix.size();
  Matrix2D rotated_matrix;
  rotated_matrix.resize(M);
  for (auto x = 0; x < M; ++x)
  {
    rotated_matrix[x].resize(N);
  }
  rotate_2d_matrix_clockwise_impl(original_matrix, rotated_matrix, M, N);
  return rotated_matrix;
}

#endif