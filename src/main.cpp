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
int main()
{
    // const char *json = R"(
    //     {
    //         "name": "neroll",
    //         "age": 12,
    //         "hobbies" : [ "play",  "study" ,  true, false,  null  ]
    //     }
    // )";

    // const char *js = R"(
    //     [ 1,  2,   true  ,  false,   null,  "hello" , {
    //         "name" : "neroll",
    //         "age": 12,
    //         "hobbies" : [ true,  false ]
    //     }   ]
    // )";

    // std::ifstream fin("bfotool-download.json");
    // std::ifstream fin("config.json");
    std::ifstream fin("test.json");
    // std::ifstream fin("D:\\Work\\Daily project\\vscode\\Daily\\out.txt");
    std::ostringstream out;
    out << fin.rdbuf();

    std::string json(out.str());

    cout << "length: " << json.size() << endl;

    try {
        Parser parser(Lexer{json});

        auto begin = clock();
        auto root = parser.parse();
        auto end = clock();

        auto object = std::static_pointer_cast<ObjectNode>(root);
        cout << "size: " <<  object->size() << endl;
        
        cout << "time: " << 1.0 * (end - begin) / CLOCKS_PER_SEC << endl;

        std::ofstream fout("index.html");
        Stringifier stringifier(root);
        fout << stringifier.to_html();


    } catch (std::runtime_error &e) {
        cout << e.what() << endl;
    }

    
    // auto value = array->value();
    // for (int i = 0; i < array->size(); i++) {
        
    // }
    
    return 0;
}
