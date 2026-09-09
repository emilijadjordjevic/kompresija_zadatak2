#include "ldpc.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace ldpc {

void write_report(const std::string& directory, std::uint32_t seed)
{
    Matrix matrix = generate_matrix({}, seed);
    CodeInfo info = analyze_code(matrix);
    FailureSearch search = find_minimum_failure(matrix);
    unsigned checks = static_cast<unsigned>(matrix.rows.size());

    std::filesystem::create_directories(directory);
    write_matrix(directory + "/matrica.txt", matrix);
    std::ofstream table(directory + "/sindromi.txt");
    std::ofstream report(directory + "/izvestaj.txt");
    if (!table || !report) throw std::runtime_error("Cannot open report files.");

    unsigned reachable = 0;
    table << "Sindrom    Korektor         Tezina\n";

    for (Word s = 0; s < info.table.size(); ++s) {
        table << word_string(s, checks) << "  ";
        const SyndromeEntry& entry = info.table[s];
        if (entry.reachable) {
            ++reachable;
            table << word_string(entry.correction, matrix.length) << "  " << entry.weight << '\n';
        } else {
            table << "nedostizan\n";
        }
    }
    report << "ZADATAK 2\nSeme: " << seed
           << "\nSeme mora odgovarati broju indeksa.\n"
           << "n=15, redova=9, wr=5, wc=3\n\n"
           << "Rang H: " << info.rank
           << "\nStvarna dimenzija: " << info.dimension
           << "\nDostizni sindromi: " << reachable << "/512"
           << "\nKodno rastojanje d: " << info.distance
           << "\nNajkraca nenulta kodna rec: "
           << word_string(info.minimum_word, matrix.length)
           << "\nGarantovana ispravka optimalnim dekoderom: "
           << (info.distance - 1) / 2 << " gresaka.\n\n"
           << "Devet redova nisu nezavisni: zbir redova svake grupe je isti.\n"
           << "Korektori su nadjeni pregledom svih 32768 reci.\n"
           << "Svaki korektor ima najmanju tezinu za svoj sindrom.\n\n"
           << "Gallager B: th0=th1=0.5, postupak iz skripte 8.8.2.\n"
           << "Poslata rec je nulta. Uspeh je povratak bas na nulu.\n"
           << "Tezina  Provereno  Ispravljeno  Pogresna rec  Ciklus\n";

    for (const WeightResult& row : search.results) {
        report << row.weight << "       " << row.tested << "          "
               << row.corrected << "            " << row.wrong_codeword
               << "             " << row.cycles << '\n';
    }

    if (search.found) {
        report << "\nNajmanja neuspesna tezina: " << weight(search.error)
               << "\nPrimer greske: " << word_string(search.error, matrix.length)
               << "\nGarantovana ispravka Gallagerom: " << weight(search.error) - 1
               << " gresaka.\nSindromski dekoder za isti primer: "
               << word_string(decode_syndrome(matrix, info.table, search.error), matrix.length) << "\n\nGallagerove iteracije:\n";

        for (Word word : search.decoding.trace) {
            report << word_string(word, matrix.length) << '\n';
        }

        report << "Kraj: " << (search.decoding.reason == StopReason::cycle ? "ciklus" : "pogresna kodna rec") << '\n';
    }

    report << "\nSve manje tezine su potpuno proverene. Za ove pragove i wc=3,\n"
           << "linearnost i simetrija dozvoljavaju proveru nulte poslate reci.\n"
           << "Nulti sindrom ne znaci uvek da je vracena poslata rec.\n";
    table.close();
    report.close();
    if (!table || !report) throw std::runtime_error("Cannot finish writing reports.");
}

} // namespace ldpc
