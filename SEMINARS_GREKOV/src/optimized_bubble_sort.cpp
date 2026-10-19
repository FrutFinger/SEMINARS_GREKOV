#include "optimized_bubble_sort.h"
#include <utility>


template <typename T>
void optimizedBubbleSort(T* arr, int size)
{
    if (arr == nullptr || size < 2) return;

    int border = size;
    while (border > 1)
    {
        int lastSwap = 0;
        for (int j = 1; j < border; ++j)
        {
            if (arr[j - 1] > arr[j])
            {
                std::swap(arr[j - 1], arr[j]);
                lastSwap = j;
            }
        }
        border = lastSwap;   // если обменов не было - 0, цикл завершится
    }
}

template void optimizedBubbleSort<int>(int*, int);
template void optimizedBubbleSort<double>(double*, int);