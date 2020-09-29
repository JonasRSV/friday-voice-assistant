#include "../shared/aixlog.hpp"
#include "friday_options.hpp"
#include "setup_configs.hpp"
#include "setup_logging.hpp"
#include "shared/argparse.h"

namespace launch {

options *parse_options(int argc, const char *argv[]) {
  argparse::ArgumentParser parser("friday", "Friday argument parser");
  parser.add_argument()
      .names({"--configs"})
      .description("Config directory")
      .required(true);
  parser.add_argument()
      .names({"--logging"})
      .description("Logging level -- 0: fatal - 1: error - 2: warning - 3: "
                   "info - else debug");

  parser.enable_help();
  auto err = parser.parse(argc, argv);

  if (err) {
    std::cout << err << std::endl;
    exit(1);
  }

  if (parser.exists("help")) {
    parser.print_help();
    exit(0);
  }

  if (parser.exists("logging")) {
    set_logging(parser.get<int>("logging"));
  } else {
    // defaults to debug
    set_logging(4);
  }

  options *opt = new options();
  opt->configs = load_configs(parser.get<std::string>("configs"));

  return opt;
}

void free_options() {
  // TODO(jonasrsv)
  // Don't really need to free them since the OS does it.. on most systems
  // anyway But always nice to clean up after yourself.
}
} // namespace launch
