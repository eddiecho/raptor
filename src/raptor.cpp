#include <iostream>
#include <string>

#define u32 uint32_t
#define s32 int32_t
#define u64 uint64_t
#define s64 int64_t
#define f32 float
#define f64 double

#define bool32 u32

#include "raptor_out.cpp"
#include "raptor_version.h"
#include "token.cpp"

void print_version() {
  printf("Raptor version %d.%d.%d\n", Raptor_VERSION_MAJOR,
         Raptor_VERSION_MINOR, Raptor_VERSION_PATCH);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    print_version();
    printf("  Usage: %s [script]\n", argv[0]);

    return 0;
  }

  if (std::string(argv[1]) == "--version") {
    print_version();
    return 0;
  }

  Scanner scan(argv[1]);
  scan.scanTokens();
  scan.print();

  return 0;
}
