#include "ldpc.h"

#include <stdexcept>

namespace ldpc {

CodeInfo analyze_code(const Matrix& matrix)
{
    validate_matrix(matrix);
    CodeInfo info;
    info.rank = matrix_rank(matrix);
    info.dimension = matrix.length - info.rank;
    info.table.resize(Word{1} << matrix.rows.size());

    for (Word word = 0; word < (Word{1} << matrix.length); ++word) {
        const Word value = syndrome(matrix, word);
        const unsigned count = weight(word);
        auto& entry = info.table[value];
        if (!entry.reachable || count < entry.weight) {
            entry = {true, word, count};
        }
        if (value == 0) {
            ++info.codewords;
            if (word != 0 && (info.distance == 0 || count < info.distance)) {
                info.distance = count;
                info.minimum_word = word;
            }
        }
    }

    return info;
}

Word decode_syndrome(const Matrix& matrix, const SyndromeTable& table, Word received)
{
    validate_matrix(matrix);

    if (received >= (Word{1} << matrix.length) ||
        table.size() != (Word{1} << matrix.rows.size())) {
        throw std::runtime_error("Word or syndrome table has the wrong size.");
    }

    const auto& entry = table[syndrome(matrix, received)];
    if (!entry.reachable) {
        throw std::runtime_error("Missing correction for a reachable syndrome.");
    }

    return received ^ entry.correction;
}

FailureSearch find_minimum_failure(const Matrix& matrix)
{
    validate_matrix(matrix);
    for (unsigned bit = 0; bit < matrix.length; ++bit) {
        unsigned degree = 0;
        for (Word row : matrix.rows) degree += (row >> bit) & 1u;
        if (degree % 2 == 0) {
            throw std::runtime_error("Failure search requires odd column weights.");
        }
    }

    FailureSearch search;
    for (unsigned count = 0; count <= matrix.length; ++count) {
        WeightResult result;
        result.weight = count;
        for (Word error = 0; error < (Word{1} << matrix.length); ++error) {
            if (weight(error) != count) continue;
            auto decoded = decode_gallager(matrix, error);
            ++result.tested;
            if (decoded.reason == StopReason::codeword && decoded.word == 0) {
                ++result.corrected;
            } else {
                if (decoded.reason == StopReason::codeword) {
                    ++result.wrong_codeword;
                } else {
                    ++result.cycles;
                }
                if (!search.found) {
                    search.found = true;
                    search.error = error;
                    search.decoding = decoded;
                }
            }
        }
        search.results.push_back(result);
        if (search.found) break;
    }
    return search;
}

} // namespace ldpc