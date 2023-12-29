#pragma once

#include <nlohmann/json.hpp>

namespace wmtk {
namespace components {
namespace internal {

struct DelaunayOptions
{
    std::string type;
    std::string input;
    std::string output;
    long cell_dimension = -1;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DelaunayOptions, type, input, output, cell_dimension);

} // namespace internal
} // namespace components
} // namespace wmtk