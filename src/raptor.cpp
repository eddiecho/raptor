#include <iostream>
#include <string.h>

#include "raptor.h"
#include "result.h"

#include "raptor_out.cpp"
#include "raptor_version.h"
#include "token.cpp"

internal inline void print_version() {
  printf("Raptor version %d.%d.%d\n", Raptor_VERSION_MAJOR,
         Raptor_VERSION_MINOR, Raptor_VERSION_PATCH);
}

int main(int argc, char **argv) {
  // TODO - repl
  if (argc == 1) {
    print_version();
    printf("  Usage: %s [script]\n", argv[0]);

    return 0;
  }

  if (strcmp(argv[1], "--version") == 0) {
    print_version();
    return 0;
  }

  Scanner scan(argv[1]);
  scan.scanTokens();
  scan.print();

  return 0;
}
