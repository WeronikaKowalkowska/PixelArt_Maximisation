//
// Created by wkow1 on 6.04.2026.
//

#ifndef ENLARGER_H
#define ENLARGER_H

#include <fstream>



class Enlarger {
public:
  //Enlarger(std::ifstream &file);

   Enlarger( const char * str);

  //std::ofstream GenerateResult();

    //void OperatePNG(std::string &fileName);

  void OperatePNG(const char * str);

private:
  //std::ifstream &file;
    void Enlarge(std::ifstream &file);

};



#endif //ENLARGER_H
