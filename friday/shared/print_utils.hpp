#ifndef PRINT_UTILS_HPP_LGEXLTDD
#define PRINT_UTILS_HPP_LGEXLTDD

#include <iterator> // needed for std::ostram_iterator
#include <vector>

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
  out << "[ ";
  for (unsigned int i = 0; i < v.size(); i++)
    out << v[i] << " ";
  out << " ]";
  return out;
}
#endif /* end of include guard: PRINT_UTILS_HPP_LGEXLTDD */
