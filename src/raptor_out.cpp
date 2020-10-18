#include "raptor_out.h"

void report(u32 line, char *message) {
  std::cerr << "Error: " << message << std::endl;
  std::cerr << "  In line: " << line << std::endl;
}
