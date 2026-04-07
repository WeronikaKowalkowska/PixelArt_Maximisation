#include <iostream>
#include "Enlarger.h"


int main() {
    std::string f = "C:/Users/wkow1/Documents/6sem/mmpx/test-data/stepin.png";
    Enlarger enl("C:/Users/wkow1/Documents/6sem/mmpx/test-data/stepin.png");
    enl.OperatePNG("C:/Users/wkow1/Documents/6sem/mmpx/test-data/stepin.png");
    //std::cout<<"hej";
    return 0;
}