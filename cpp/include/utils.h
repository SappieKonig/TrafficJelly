#ifndef UTILS_H
#define UTILS_H

#include <optional>

template <typename T>
using optional_reference = std::optional<std::reference_wrapper<T>>;
template <typename T>
using optional_const_reference = std::optional<std::reference_wrapper<T const>>;

class Car;
struct Node;
struct Edge;

#endif
