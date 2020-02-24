#include <stdlib.h>
#include <iostream>

#include "blifparse.hpp"

#include "c_make_header.h"
#include "blif_ingest.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << argv[0] << " Version " <<  blifdot_VERSION_MAJOR << "."
              << blifdot_VERSION_MINOR << std::endl;
  }

  blifdot::BlifIngest ingest;
  blifparse::blif_parse_filename(argv[1], ingest);

  return EXIT_SUCCESS;
}
