#ifndef UTILS_H
#define UTILS_H

#include <optional>
#include <iostream>
#include <memory>

template <typename T>
using optional_reference = std::optional<std::reference_wrapper<T>>;
template <typename T>
using optional_const_reference = std::optional<std::reference_wrapper<T const>>;

class Car;
class Node;
class Edge;

#endif
