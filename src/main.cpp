#include <iostream>
#include <format>
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>

#include "njson.h"

using std::cout, std::endl;
using std::format;
using namespace neroll;

int main() {
    std::ifstream fin("test.json");
    std::ostringstream out;
    out << fin.rdbuf();

    std::string json(out.str());

    try {
        Parser parser(Lexer{json});

        auto root = parser.parse();

        auto object = std::static_pointer_cast<ObjectNode>(root);

        std::ofstream fout("index.html");
        Stringifier stringifier(root);
        fout << stringifier.to_html();

    } catch (std::runtime_error &e) {
        cout << e.what() << endl;
    }
}
