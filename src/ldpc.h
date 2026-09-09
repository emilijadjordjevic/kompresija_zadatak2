#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ldpc {

using Word = std::uint32_t;

struct Parameters {
    unsigned length = 15;
    unsigned checks = 9;
    unsigned row_weight = 5;
    unsigned column_weight = 3;
};

struct Matrix {
    unsigned length = 0;
    std::vector<Word> rows;
};

struct SyndromeEntry {
    bool reachable = false;
    Word correction = 0;
    unsigned weight = 0;
};

using SyndromeTable = std::vector<SyndromeEntry>;

struct CodeInfo {
    unsigned rank = 0;
    unsigned dimension = 0;
    unsigned distance = 0;
    unsigned codewords = 0;
    Word minimum_word = 0;
    SyndromeTable table;
};

enum class StopReason {
    codeword,
    cycle
};

struct GallagerResult {
    Word word = 0;
    StopReason reason = StopReason::cycle;
    std::vector<Word> trace;
};

struct WeightResult {
    unsigned weight = 0;
    unsigned tested = 0;
    unsigned corrected = 0;
    unsigned wrong_codeword = 0;
    unsigned cycles = 0;
};

struct FailureSearch {
    bool found = false;
    Word error = 0;
    GallagerResult decoding;
    std::vector<WeightResult> results;
};

unsigned weight(Word word);
std::string word_string(Word word, unsigned length);
void validate_matrix(const Matrix& matrix);

Matrix generate_matrix(const Parameters& parameters, std::uint32_t seed);
void write_matrix(const std::string& path, const Matrix& matrix);

Word syndrome(const Matrix& matrix, Word word);
unsigned matrix_rank(const Matrix& matrix);
CodeInfo analyze_code(const Matrix& matrix);
Word decode_syndrome(const Matrix& matrix, const SyndromeTable& table, Word received);

GallagerResult decode_gallager(const Matrix& matrix, Word received, double threshold_zero = 0.5, double threshold_one = 0.5);
FailureSearch find_minimum_failure(const Matrix& matrix);

void write_report(const std::string& directory, std::uint32_t seed);

} // namespace ldpc
