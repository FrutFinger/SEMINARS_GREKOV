#pragma once
#include <string>

// Сортирует бинарный файл с элементами типа T (однофазное простое слияние, 4 файла).
template <typename T>
void onePhaseMergeSort(const std::string& fileName);