#include "binary_insertion_sort.h"

template <typename T>
void binaryInsertionSort(T* arr, int size)
{
    if (arr == nullptr || size < 2) return;

    for (int i = 1; i < size; ++i)
    {
        T key = arr[i];

        // Бинарный поиск позиции вставки в отсортированной части [0, i)
        int left = 0, right = i;
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            if (key < arr[mid])
                right = mid;
            else
                left = mid + 1;      // равные идут правее -> сортировка устойчива
        }

        // Сдвиг элементов вправо и вставка
        for (int j = i; j > left; --j)
            arr[j] = arr[j - 1];
        arr[left] = key;
    }
}

template void binaryInsertionSort<int>(int*, int);
template void binaryInsertionSort<double>(double*, int);