#include <iostream>
#include <boost/program_options.hpp>
#include "utils.h"

int main(int argc, char *args[]) {
  Utils::init("config.json");

  return 0;
}
