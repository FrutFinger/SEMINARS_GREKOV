#include <chrono>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "two_phase_merge_sort.h"
#include "one_phase_merge_sort.h"
#include "natural_merge_sort.h"

namespace fs = std::filesystem;

struct Algo
{
    std::string tag;
    std::function<void(const std::string&, const std::string&, std::size_t, bool)> run;
};

static std::vector<Algo> algos()
{
    return {
        { "two_phase", [](const std::string& f, const std::string& t, std::size_t r, bool k)
                       { twoPhaseMergeSort<int>(f, t, r, k); } },
        { "one_phase", [](const std::string& f, const std::string& t, std::size_t r, bool k)
                       { onePhaseMergeSort<int>(f, t, r, k); } },
        { "natural",   [](const std::string& f, const std::string& t, std::size_t, bool k)
                       { naturalMergeSort<int>(f, t, k); } }
    };
}

static void writeFile(const std::string& name, const std::vector<int>& v)
{
    std::ofstream out(name, std::ios::trunc);
    for (int x : v) out << x << '\n';
}

static void printFile(const std::string& name)
{
    std::ifstream in(name);
    int x;
    while (in >> x) std::cout << x << ' ';
    std::cout << '\n';
}

static void generateRandomFile(const std::string& name, std::size_t count)
{
    std::random_device rd;                        // новый набор при каждом запуске
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 999999999);
    std::ofstream out(name, std::ios::trunc);
    for (std::size_t i = 0; i < count; ++i) out << dist(gen) << '\n';
}

// ѕровер€ет, что файл отсортирован и содержит expectedCount чисел
static bool checkSorted(const std::string& name, std::size_t expectedCount)
{
    std::ifstream in(name);
    int prev, x;
    std::size_t n = 0;
    bool ok = true;
    while (in >> x)
    {
        if (n > 0 && x < prev) ok = false;
        prev = x;
        ++n;
    }
    return ok && n == expectedCount;
}

static void smallDemo()
{
    fs::create_directories("data");
    const std::vector<int> variant = { 0, 8, 2, 17, 9, 13, 7, 6 };
    const std::string input = "data/variant_input.txt";
    writeFile(input, variant);

    std::cout << "=== Variant 6 demo ===\n";
    std::cout << "input:        ";
    printFile(input);

    for (const Algo& a : algos())
    {
        const std::string result = "data/variant_result_" + a.tag + ".txt";
        fs::copy_file(input, result, fs::copy_options::overwrite_existing);
        a.run(result, "data/tmp_variant_" + a.tag, 2, true);
        std::cout << a.tag << ": ";
        printFile(result);
    }
    std::cout << "Files v papke: " << fs::absolute("data").string() << "\n\n";
}

static void bigDemo()
{
    const std::size_t COUNT = 8000000; 
    const std::size_t RUN_SIZE = 1000000;
    const std::string input = "data/big_input.txt";

    std::cout << "=== Big file demo ===\nGenerating " << COUNT << " random numbers...\n";
    generateRandomFile(input, COUNT);
    std::cout << "big_input.txt: " << fs::file_size(input) / (1024 * 1024) << " MB\n";

    for (const Algo& a : algos())
    {
        const std::string result = "data/big_result_" + a.tag + ".txt";
        fs::copy_file(input, result, fs::copy_options::overwrite_existing);

        const auto t0 = std::chrono::steady_clock::now();
        a.run(result, "data/tmp_big_" + a.tag, RUN_SIZE, false);
        const auto t1 = std::chrono::steady_clock::now();

        std::cout << a.tag << ": "
            << std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() << " s, "
            << (checkSorted(result, COUNT) ? "SORTED OK" : "ERROR!") << '\n';
    }
}

int main()
{
    try
    {
        smallDemo();

        std::cout << "zapustit' bolshoi file? (y/n): ";
        char c = 'n';
        std::cin >> c;
        if (c == 'y' || c == 'Y') bigDemo();
    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}