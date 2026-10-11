#include "one_phase_merge_sort.h"
#include <fstream>
#include <cstdio>
#include <stdexcept>
#include <utility>

namespace
{
    template <typename T>
    bool readElem(std::ifstream& f, T& x)
    {
        return static_cast<bool>(f.read(reinterpret_cast<char*>(&x), sizeof(T)));
    }

    template <typename T>
    void writeElem(std::ofstream& f, const T& x)
    {
        f.write(reinterpret_cast<const char*>(&x), sizeof(T));
    }

    template <typename T>
    long long countElements(const std::string& name)
    {
        std::ifstream f(name, std::ios::binary | std::ios::ate);
        if (!f) throw std::runtime_error("Cannot open file: " + name);
        return static_cast<long long>(f.tellg()) / static_cast<long long>(sizeof(T));
    }

    // Начальное распределение: серии длины 1 поочерёдно в два файла
    template <typename T>
    void initialDistribute(const std::string& src, const std::string& name1,
        const std::string& name2)
    {
        std::ifstream in(src, std::ios::binary);
        std::ofstream f1(name1, std::ios::binary | std::ios::trunc);
        std::ofstream f2(name2, std::ios::binary | std::ios::trunc);

        bool first = true;
        T x;
        while (readElem(in, x))
        {
            writeElem(first ? f1 : f2, x);
            first = !first;
        }
    }

    // Один проход: слияние пар серий длины len из in1, in2;
    // результаты пишутся поочерёдно в out1 и out2.
    // Возвращает количество слитых пар (= число серий на выходе).
    template <typename T>
    long long mergePass(const std::string& in1, const std::string& in2,
        const std::string& out1, const std::string& out2,
        long long len)
    {
        std::ifstream a(in1, std::ios::binary);
        std::ifstream b(in2, std::ios::binary);
        std::ofstream o1(out1, std::ios::binary | std::ios::trunc);
        std::ofstream o2(out2, std::ios::binary | std::ios::trunc);

        T xa{}, xb{};
        bool hasA = readElem(a, xa);
        bool hasB = readElem(b, xb);

        bool toFirst = true;
        long long pairs = 0;

        while (hasA || hasB)
        {
            std::ofstream& out = toFirst ? o1 : o2;
            long long ca = hasA ? len : 0;
            long long cb = hasB ? len : 0;

            while (ca > 0 || cb > 0)
            {
                bool takeA;
                if (ca == 0)       takeA = false;
                else if (cb == 0)  takeA = true;
                else               takeA = !(xb < xa);

                if (takeA)
                {
                    writeElem(out, xa);
                    --ca;
                    if (!(hasA = readElem(a, xa))) ca = 0;
                }
                else
                {
                    writeElem(out, xb);
                    --cb;
                    if (!(hasB = readElem(b, xb))) cb = 0;
                }
            }
            toFirst = !toFirst;
            ++pairs;
        }
        return pairs;
    }
}

template <typename T>
void onePhaseMergeSort(const std::string& fileName)
{
    const long long total = countElements<T>(fileName);
    if (total < 2) return;

    std::string in1 = fileName + ".tmp1";
    std::string in2 = fileName + ".tmp2";
    std::string out1 = fileName + ".tmp3";
    std::string out2 = fileName + ".tmp4";

    initialDistribute<T>(fileName, in1, in2);

    for (long long len = 1; ; len *= 2)
    {
        long long pairs = mergePass<T>(in1, in2, out1, out2, len);

        if (pairs <= 1)
        {
            // Вся последовательность слита в одну серию (она в out1)
            std::remove(fileName.c_str());
            std::rename(out1.c_str(), fileName.c_str());
            break;
        }

        // Выходные файлы становятся входными
        std::swap(in1, out1);
        std::swap(in2, out2);
    }

    std::remove(in1.c_str());
    std::remove(in2.c_str());
    std::remove(out1.c_str());
    std::remove(out2.c_str());
}

template void onePhaseMergeSort<int>(const std::string&);
template void onePhaseMergeSort<double>(const std::string&);