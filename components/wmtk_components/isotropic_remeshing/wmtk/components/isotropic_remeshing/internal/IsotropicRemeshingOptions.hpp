#pragma once

#include <nlohmann/json.hpp>

namespace wmtk::components::internal {

struct IsotropicRemeshingAttributes
{
    nlohmann::json position;
    nlohmann::json inversion_position;
    nlohmann::json other_positions;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    IsotropicRemeshingAttributes,
    position,
    inversion_position,
    other_positions);

struct IsotropicRemeshingOptions
{
    std::string input;
    std::string output;
    IsotropicRemeshingAttributes attributes;
    nlohmann::json pass_through;
    int64_t iterations;
    double length_abs;
    double length_rel;
    bool lock_boundary;
    bool dont_lock_split;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    IsotropicRemeshingOptions,
    input,
    output,
    attributes,
    pass_through,
    length_abs,
    length_rel,
    iterations,
    lock_boundary,
    dont_lock_split);

} // namespace wmtk::components::internal
