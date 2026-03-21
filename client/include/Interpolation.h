#ifndef MOTION_INTERPOLATION_INTERPOLATION_H
#define MOTION_INTERPOLATION_INTERPOLATION_H
#include <deque>
#include "SFML/System/Vector2.hpp"
using namespace sf;
class Interpolation
{
public:
    struct Sample {
        Vector2f position;
        float time;
    };
    Vector2f lerp(std::deque<Sample> &remote_buffer, float render_time);
    Vector2f lagrange(std::deque<Sample> &remote_buffer, float render_time, int n);
    Vector2f newton(std::deque<Sample> &remote_buffer, float render_time, int n);

private:
    float evaluateNewtonBasis(const std::deque<Sample>& remote_buffer,
                              float render_time,
                              int n,
                              bool use_x_axis);
};
#endif //MOTION_INTERPOLATION_INTERPOLATION_H
