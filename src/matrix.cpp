#include "ldpc.h"

#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>
#include <stdexcept>

namespace ldpc {
namespace {

unsigned random_below(std::mt19937& random, unsigned bound)
{
    const std::uint64_t range = std::uint64_t{1} << 32;
    const auto limit = range - range % bound;
    std::uint32_t value;

    do {
        value = static_cast<std::uint32_t>(random());
    } while (value >= limit);
    return value % bound;
}

} // namespace

unsigned weight(Word word)
{
    unsigned count = 0;
    while (word != 0) {
        word &= word - 1;
        ++count;
    }
    return count;
}

std::string word_string(Word word, unsigned length)
{
    if (length == 0 || length > 20 || word >= (Word{1} << length)) {
        throw std::runtime_error("Word does not fit the requested length.");
    }
    std::string text(length, '0');
    for (unsigned bit = 0; bit < length; ++bit) {
        if ((word >> bit) & 1u) text[bit] = '1';
    }
    return text;
}

void validate_matrix(const Matrix& matrix)
{
    if (matrix.length == 0 || matrix.length > 20 ||
        matrix.rows.empty() || matrix.rows.size() > 20) {
        throw std::runtime_error("Expected 1-20 columns and 1-20 rows.");
    }
    for (Word row : matrix.rows) {
        if (row >= (Word{1} << matrix.length)) {
            throw std::runtime_error("Matrix row has bits outside its width.");
        }
    }
}

Matrix generate_matrix(const Parameters& parameters, std::uint32_t seed)
{
    const auto& p = parameters;

    if (p.length == 0 || p.length > 20 || p.checks == 0 || p.checks > 20 ||
        p.row_weight == 0 || p.row_weight > p.length ||
        p.column_weight == 0 || p.column_weight > p.checks ||
        p.length % p.row_weight != 0 ||
        p.checks * p.row_weight != p.length * p.column_weight) {
        throw std::runtime_error("Invalid parameters for Gallager construction.");
    }

    Matrix matrix{p.length, std::vector<Word>(p.checks)};
    const unsigned block_rows = p.length / p.row_weight;

    for (unsigned bit = 0; bit < p.length; ++bit) {
        matrix.rows[bit / p.row_weight] |= Word{1} << bit;
    }

    std::mt19937 random(seed);
    std::vector<unsigned> permutation(p.length);
    for (unsigned block = 1; block < p.column_weight; ++block) {
        std::iota(permutation.begin(), permutation.end(), 0);

        for (unsigned i = p.length; i > 1; --i) {
            std::swap(permutation[i - 1], permutation[random_below(random, i)]);
        }

        for (unsigned bit = 0; bit < p.length; ++bit) {
            const unsigned row = permutation[bit] / p.row_weight;
            matrix.rows[block * block_rows + row] |= Word{1} << bit;
        }
    }
    return matrix;
}

void write_matrix(const std::string& path, const Matrix& matrix)
{
    validate_matrix(matrix);
    std::ofstream file(path);
    file << matrix.rows.size() << ' ' << matrix.length << '\n';

    for (Word row : matrix.rows) {
        file << word_string(row, matrix.length) << '\n';
    }

    file.close();
    if (!file) throw std::runtime_error("Cannot write matrix: " + path);
}

Word syndrome(const Matrix& matrix, Word word)
{
    Word result = 0;

    for (unsigned row = 0; row < matrix.rows.size(); ++row) {
        result |= static_cast<Word>(weight(matrix.rows[row] & word) % 2) << row;
    }

    return result;
}

unsigned matrix_rank(const Matrix& matrix)
{
    validate_matrix(matrix);
    auto rows = matrix.rows;
    unsigned rank = 0;

    for (unsigned bit = 0; bit < matrix.length && rank < rows.size(); ++bit) {
        unsigned pivot = rank;

        while (pivot < rows.size() && ((rows[pivot] >> bit) & 1u) == 0) ++pivot;

        if (pivot == rows.size()) continue;
        std::swap(rows[rank], rows[pivot]);

        for (unsigned row = rank + 1; row < rows.size(); ++row) {
            if ((rows[row] >> bit) & 1u) rows[row] ^= rows[rank];
        }
        ++rank;
    }
    return rank;
}

} // namespace ldpc