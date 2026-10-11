#include "natural_merge_sort.h"
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

    // –аспределение естественных серий поочерЄдно в b и c.
    // ¬озвращает количество найденных серий.
    template <typename T>
    long long distributeRuns(const std::string& src, const std::string& nameB,
        const std::string& nameC)
    {
        std::ifstream in(src, std::ios::binary);
        if (!in) throw std::runtime_error("Cannot open file: " + src);
        std::ofstream b(nameB, std::ios::binary | std::ios::trunc);
        std::ofstream c(nameC, std::ios::binary | std::ios::trunc);

        long long runs = 0;
        bool toB = true;
        T prev{}, x{};
        bool first = true;

        while (readElem(in, x))
        {
            if (first)
            {
                runs = 1;
                first = false;
            }
            else if (x < prev)          // сери€ закончилась
            {
                toB = !toB;
                ++runs;
            }
            writeElem(toB ? b : c, x);
            prev = x;
        }
        return runs;
    }

    // —ли€ние серий из b и c в dst (сери€ кончаетс€, когда следующий < предыдущего,
    // либо закончилс€ файл).
    template <typename T>
    void mergeRuns(const std::string& nameB, const std::string& nameC,
        const std::string& dst)
    {
        std::ifstream b(nameB, std::ios::binary);
        std::ifstream c(nameC, std::ios::binary);
        std::ofstream out(dst, std::ios::binary | std::ios::trunc);

        T xb{}, xc{};
        bool hasB = readElem(b, xb);
        bool hasC = readElem(c, xc);

        while (hasB || hasC)
        {
            bool endB = !hasB;   // текуща€ сери€ в B закончилась?
            bool endC = !hasC;

            while (!endB || !endC)
            {
                bool takeB;
                if (endB)       takeB = false;
                else if (endC)  takeB = true;
                else            takeB = !(xc < xb);

                if (takeB)
                {
                    T prev = xb;
                    writeElem(out, xb);
                    hasB = readElem(b, xb);
                    if (!hasB || xb < prev) endB = true;
                }
                else
                {
                    T prev = xc;
                    writeElem(out, xc);
                    hasC = readElem(c, xc);
                    if (!hasC || xc < prev) endC = true;
                }
            }
        }
    }
}

template <typename T>
void naturalMergeSort(const std::string& fileName)
{
    const std::string fileB = fileName + ".tmpB";
    const std::string fileC = fileName + ".tmpC";

    while (true)
    {
        long long runs = distributeRuns<T>(fileName, fileB, fileC);
        if (runs <= 1) break;                    // уже отсортировано
        mergeRuns<T>(fileB, fileC, fileName);
    }

    std::remove(fileB.c_str());
    std::remove(fileC.c_str());
}

template void naturalMergeSort<int>(const std::string&);
template void naturalMergeSort<double>(const std::string&);