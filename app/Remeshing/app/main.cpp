// Local library code
#include <remeshing/dummy.h>

#include <wmtk/utils/Logger.hpp>

// clang-format off
#include <wmtk/utils/DisableWarnings.hpp>
#include <CLI/CLI.hpp>
#include <wmtk/utils/EnableWarnings.hpp>
// clang-format on

// System include
#include <iostream>


int main(int argc, char const* argv[])
{
    struct
    {
        std::string input;
        double ratio = 1;
    } args;

    CLI::App app{argv[0]};
    // app.option_defaults()->always_capture_default();
    app.add_option("input", args.input, "Input mesh.");
    app.add_option("-r,--ratio", args.ratio, "Ratio.");
    CLI11_PARSE(app, argc, argv);

    wmtk::logger().info("Ratio: {}", args.ratio);

    return 0;
}
