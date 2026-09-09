#include "ldpc.h"

#include <iostream>
#include <limits>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try {
        if (argc != 2) {
            std::cout << "Pokretanje: ./ldpc BROJ_INDEKSA\n";
            return 1;
        }
        const std::string text = argv[1];
        if (text.empty() || text.find_first_not_of("0123456789") != std::string::npos) {
            throw std::runtime_error("Seme mora biti nenegativan ceo broj.");
        }
        const unsigned long long seed = std::stoull(text);
        if (seed > std::numeric_limits<std::uint32_t>::max()) {
            throw std::runtime_error("Seme je preveliko.");
        }
        ldpc::write_report("results", static_cast<std::uint32_t>(seed));
        std::cout << "Matrica, sindromi i izvestaj su u folderu results.\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
