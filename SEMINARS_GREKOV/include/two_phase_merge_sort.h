#pragma once
#include <string>

// Сортирует бинарный файл с элементами типа T (двухфазное простое слияние).
template <typename T>
void twoPhaseMergeSort(const std::string& fileName);