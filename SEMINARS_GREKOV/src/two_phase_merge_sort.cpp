#include "two_phase_merge_sort.h"
#include "merge_utils.h"
#include <filesystem>
#include <stdexcept>

namespace
{
    template <typename T>
    void mergePhase(const std::string& b, const std::string& c,
        const std::string& dst, std::size_t len)
    {
        merge_utils::Reader<T> rb(b), rc(c);
        merge_utils::Writer<T> out(dst);
        while (rb.valid() || rc.valid())
            merge_utils::mergeFixed(rb, rc, out, len);
    }
}

template <typename T>
void twoPhaseMergeSort(const std::string& inputFile, const std::string& tempDir,
    std::size_t runSize, bool keepTemp)
{
    namespace fs = std::filesystem;
    if (!fs::exists(inputFile))
        throw std::runtime_error("Input file not found: " + inputFile);
    if (runSize == 0) runSize = 1;
    fs::create_directories(tempDir);

    const std::string fileB = merge_utils::tempPath(tempDir, "two_phase_b.txt");
    const std::string fileC = merge_utils::tempPath(tempDir, "two_phase_c.txt");

    std::size_t runs = merge_utils::distributeSorted<T>(inputFile, fileB, fileC, runSize);

    if (runs > 0)
    {
        std::size_t len = runSize;
        while (true)
        {
            mergePhase<T>(fileB, fileC, inputFile, len);  
            runs = (runs + 1) / 2;
            if (runs == 1) break;                        

            len *= 2;
            merge_utils::distributeFixed<T>(inputFile, fileB, fileC, len); 
        }
    }

    if (!keepTemp)
    {
        merge_utils::removeIfExists(fileB);
        merge_utils::removeIfExists(fileC);
    }
}

template void twoPhaseMergeSort<int>(const std::string&, const std::string&, std::size_t, bool);
template void twoPhaseMergeSort<double>(const std::string&, const std::string&, std::size_t, bool);
template void twoPhaseMergeSort<float>(const std::string&, const std::string&, std::size_t, bool);