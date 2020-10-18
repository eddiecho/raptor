#include <iostream>
#include <string>

#define u32 uint32_t
#define s32 int32_t
#define u64 uint64_t
#define s64 int64_t
#define f34 float
#define f64 double

#include "raptor_out.cpp"
#include "raptor_version.h"
#include "token.cpp"

int main(int argc, char **argv) {
  if (argc == 1) {
    std::cout << "Raptor Version " << Raptor_VERSION_MAJOR << "."
              << Raptor_VERSION_MINOR << std::endl;
    std::cout << " Usage: " << argv[0] << " [script]" << std::endl;
  } else if (argc == 2) {
    if (std::string(argv[1]) == "--version") {
      std::cout << "Raptor Version " << Raptor_VERSION_MAJOR << "."
                << Raptor_VERSION_MINOR << std::endl;

      return 0;
    }

    Scanner scan(argv[1]);
    scan.print();
  }
  return 0;
}
