#include <iostream>
#include <vector>
#include <fstream>

static const double height = 1;

double rectangle(double x, double a, double b)
{
    if ((a < x) && (x < b))
        return height;
    else
        return 0;
}

void print_vector(const std::vector<double> vec)
{
    for (auto && x : vec)
        std::cout << x << "   ";
    std::cout << std::endl;
}

bool write_vec_to_file(const std::string& filename, const std::vector<double>& data, const std::string& delimiter = " ")
{
    if (std::remove(filename.c_str()) != 0) {
        if (errno != ENOENT) {
            std::cerr << "Ошибка при удалении файла: " << filename << std::endl;
            return false;
        }
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Ошибка: не удалось создать файл " << filename << std::endl;
        return false;
    }

    for (size_t i = 0; i < data.size(); ++i) {
        outFile << data[i];
        if (i != data.size() - 1) {
            outFile << delimiter;
        }
    }

    outFile.close();
    return true;
}
