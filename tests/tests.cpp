#include "ldpc.h"
#include <iostream>

using namespace ldpc;

int main()
{
    Matrix matrix = generate_matrix({}, 1);
    for (Word row : matrix.rows) {
        if (weight(row) != 5) return 1;
    }
    for (unsigned bit = 0; bit < 15; ++bit) {
        unsigned count = 0;
        for (Word row : matrix.rows) count += (row >> bit) & 1u;
        if (count != 3) return 1;
    }

    CodeInfo info = analyze_code(matrix);
    Word received = 1; // Jedna greska u nultoj poslatoj reci.
    Word decoded = decode_syndrome(matrix, info.table, received);
    if (syndrome(matrix, decoded) != 0) return 1;

    GallagerResult result = decode_gallager(matrix, 0);
    if (result.word != 0 || result.reason != StopReason::codeword) return 1;

    std::cout << "Redovi imaju 5, a kolone 3 jedinice.\n"
              << "Sindromski dekoder je vratio kodnu rec.\n"
              << "Gallager je sacuvao rec bez greske.\n";
}
