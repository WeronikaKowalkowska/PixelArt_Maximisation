//
// Created by wkow1 on 6.04.2026.
//
#include <iostream>
#include "Enlarger.h"
#include "thirdparty/SDL/include/SDL3/SDL_surface.h"


//Enlarger::Enlarger(std::ifstream &file):file(file) {};
Enlarger::Enlarger(const char* str){
  // implementation, e.g., open file or store the string
};
//std::ofstream Enlarger::GenerateResults(std::ifstream &file){
//  return file;
//};

void Enlarger::OperatePNG(const char* fileName){
  SDL_Surface *surf =  SDL_LoadPNG(fileName);
  if (surf == NULL) {
    std::cerr << "Error loading image: " << fileName << std::endl;

  }
  if(!SDL_SavePNG(surf,"C:/Users/wkow1/Documents/6sem/mmpx/Enlarger.png")){
    std::cerr << "Error saving image: " << fileName << std::endl;
  }
}

void Enlarger::Enlarge(std::ifstream &file){

};
