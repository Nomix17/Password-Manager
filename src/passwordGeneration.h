#include <iostream>
#include <sstream>

#ifndef RANDOMGENERATION 
#define RANDOMGENERATION

inline std::string GeneratePassword(){
  std::stringstream sstr;
  for(int i=0;i<16;i++){
    char CharacterCase = rand() % 2 ? 'A':'a';
    char Character = rand()%2? (char)(rand() % 26 + CharacterCase):(char)(rand() % 10 + '0');
    sstr << Character;
  }
  return sstr.str();
}

#endif
