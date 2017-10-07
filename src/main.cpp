#include <fstream>


int main(int argc, char * argv[]) {

    std::ofstream outFile("results.csv");
    outFile << "rr";

    return 0;
}