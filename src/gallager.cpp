#include "ldpc.h"

#include <stdexcept>

namespace ldpc {
namespace {

Word next_word(const Matrix& matrix, Word current, Word received, double threshold_zero, double threshold_one)
{
    Word next = received;
    for (unsigned bit = 0; bit < matrix.length; ++bit) {
        const Word mask = Word{1} << bit;
        unsigned zeros = 0;
        unsigned ones = 0;

        for (Word row : matrix.rows) {
            if ((row & mask) == 0) continue;
            const Word others = current & row & ~mask;
            if (weight(others) % 2 == 0) ++zeros;
            else ++ones;
        }

        const unsigned votes = zeros + ones;

        if (votes == 0) continue;
        if (zeros >= threshold_zero * votes) next &= ~mask;
        else if (ones >= threshold_one * votes) next |= mask;
    }
    return next;
}

} // namespace

GallagerResult decode_gallager(const Matrix& matrix, Word received, double threshold_zero, double threshold_one)
{
    validate_matrix(matrix);
    if (received >= (Word{1} << matrix.length) ||
        !(threshold_zero >= 0.0 && threshold_zero <= 1.0) ||
        !(threshold_one >= 0.0 && threshold_one <= 1.0)) {
        throw std::runtime_error("Invalid word or decision thresholds.");
    }

    GallagerResult result;
    std::vector<bool> seen(Word{1} << matrix.length);
    Word current = received;

    while (true) {
        result.trace.push_back(current);
        result.word = current;
        if (syndrome(matrix, current) == 0) {
            result.reason = StopReason::codeword;
            return result;
        }
        if (seen[current]) {
            result.reason = StopReason::cycle;
            return result;
        }
        seen[current] = true;
        current = next_word(matrix, current, received, threshold_zero, threshold_one);
    }
}

} // namespace ldpc
