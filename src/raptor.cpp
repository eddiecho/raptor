#include <iostream>

#include "raptor_version.h"
#include "token.cpp"

int main(int argc, char **argv) {
  if (argc < 2) {
    // report version
    std::cout << argv[0] << " Version " << Raptor_VERSION_MAJOR << "."
              << Raptor_VERSION_MINOR << std::endl;
  }
  return 0;
}
