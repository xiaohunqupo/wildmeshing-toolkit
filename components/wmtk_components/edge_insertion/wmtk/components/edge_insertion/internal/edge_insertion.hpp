#pragma once

#include <wmtk/EdgeMesh.hpp>
#include <wmtk/TriMesh.hpp>

#include <wmtk/Types.hpp>
#include <wmtk/utils/Rational.hpp>


namespace wmtk::components::internal {

struct bbox
{
    Rational x_min, x_max, y_min, y_max;

    bbox(const Vector2r& p0, const Vector2r& p1)
    {
        if (p0[0] < p1[0]) {
            x_min = p0[0];
            x_max = p1[0];
        } else {
            x_min = p1[0];
            x_max = p0[0];
        }

        if (p0[1] < p1[1]) {
            y_min = p0[1];
            y_max = p1[1];
        } else {
            y_min = p1[1];
            y_max = p0[1];
        }
    }

    bbox(const Vector2r& p0, const Vector2r& p1, const Vector2r& p2)
    {
        if (p0[0] < p1[0]) {
            x_min = p0[0];
            x_max = p1[0];
        } else {
            x_min = p1[0];
            x_max = p0[0];
        }

        if (p0[1] < p1[1]) {
            y_min = p0[1];
            y_max = p1[1];
        } else {
            y_min = p1[1];
            y_max = p0[1];
        }

        x_min = (x_min > p2[0]) ? p2[0] : x_min;
        x_max = (x_max < p2[0]) ? p2[0] : x_max;
        y_min = (y_min > p2[1]) ? p2[1] : y_min;
        y_max = (y_max < p2[1]) ? p2[1] : y_max;
    }
};
class Segment
{
public:
    const Vector2r p0, p1;

    std::vector<std::pair<int64_t, Rational>> points_on_segments;

    Segment(const Vector2r& p0, const Vector2r& p1, const int64_t idx0, const int64_t idx1);
};

/**
 * @brief
 *
 * @param point
 * @param t1
 * @param t2
 * @param t3
 * @return int -1: outside 0: inside 1: on AB 2: on BC 3: on AC 4:on endpoint
 */
int is_point_inside_triangle(
    const wmtk::Vector2r& P,
    const wmtk::Vector2r& A,
    const wmtk::Vector2r& B,
    const wmtk::Vector2r& C);


/**
 * @brief include the intersection on edgepoints
 *
 */
bool segment_segment_inter(
    const Vector2r& s0,
    const Vector2r& e0,
    const Vector2r& s1,
    const Vector2r& e1,
    Vector2r& res);

} // namespace wmtk::components::internal