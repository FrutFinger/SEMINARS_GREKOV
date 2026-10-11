#include "two_phase_merge_sort.h"
#include <fstream>
#include <cstdio>
#include <stdexcept>

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

    // Фаза 1: распределение серий длины len из src поочерёдно в b и c
    template <typename T>
    void distribute(const std::string& src, const std::string& nameB,
        const std::string& nameC, long long len)
    {
        std::ifstream in(src, std::ios::binary);
        std::ofstream b(nameB, std::ios::binary | std::ios::trunc);
        std::ofstream c(nameC, std::ios::binary | std::ios::trunc);

        bool toB = true;
        long long cnt = 0;
        T x;
        while (readElem(in, x))
        {
            writeElem(toB ? b : c, x);
            if (++cnt == len)
            {
                cnt = 0;
                toB = !toB;
            }
        }
    }

    // Фаза 2: слияние пар серий длины len из b и c в dst
    template <typename T>
    void mergePairs(const std::string& nameB, const std::string& nameC,
        const std::string& dst, long long len)
    {
        std::ifstream b(nameB, std::ios::binary);
        std::ifstream c(nameC, std::ios::binary);
        std::ofstream out(dst, std::ios::binary | std::ios::trunc);

        T xb{}, xc{};
        bool hasB = readElem(b, xb);
        bool hasC = readElem(c, xc);

        while (hasB || hasC)
        {
            long long cb = hasB ? len : 0;
            long long cc = hasC ? len : 0;

            while (cb > 0 || cc > 0)
            {
                bool takeB;
                if (cb == 0)       takeB = false;
                else if (cc == 0)  takeB = true;
                else               takeB = !(xc < xb);

                if (takeB)
                {
                    writeElem(out, xb);
                    --cb;
                    if (!(hasB = readElem(b, xb))) cb = 0;
                }
                else
                {
                    writeElem(out, xc);
                    --cc;
                    if (!(hasC = readElem(c, xc))) cc = 0;
                }
            }
        }
    }
}

template <typename T>
void twoPhaseMergeSort(const std::string& fileName)
{
    const long long total = countElements<T>(fileName);
    if (total < 2) return;

    const std::string fileB = fileName + ".tmpB";
    const std::string fileC = fileName + ".tmpC";

    for (long long len = 1; len < total; len *= 2)
    {
        distribute<T>(fileName, fileB, fileC, len);   // фаза 1
        mergePairs<T>(fileB, fileC, fileName, len);   // фаза 2
    }

    std::remove(fileB.c_str());
    std::remove(fileC.c_str());
}

template void twoPhaseMergeSort<int>(const std::string&);
template void twoPhaseMergeSort<double>(const std::string&);