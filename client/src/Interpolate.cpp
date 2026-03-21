#include <algorithm>
#include <vector>
#include "Interpolation.h"
#include <SFML/System/Vector2.hpp>
#include <iostream>
using namespace sf;
sf::Vector2f Interpolation::lerp(std::deque<Sample> &remote_buffer, float render_time)
{
    while (remote_buffer.size() >= 2 &&
               remote_buffer[1].time <= render_time)
    {
        remote_buffer.pop_front();
    }

    const Sample& a = remote_buffer[0];
    const Sample& b = remote_buffer[1];

    float t = (render_time - a.time) / (b.time - a.time);
    t = std::clamp(t, 0.f, 1.f);

    sf::Vector2f interpolated =
        a.position + t * (b.position - a.position);
    return interpolated;
}

Vector2f Interpolation::lagrange(std::deque<Sample>& remote_buffer, float render_time, int n)
{
    if (remote_buffer.size()>=n)
    {
        sf::Vector2f result(0.f, 0.f);

        for (size_t i = 0; i < n; ++i)
        {
            float Li = 1.f;

            float ti = remote_buffer[i].time;

            for (size_t j = 0; j < n; ++j)
            {
                if (i == j)
                    continue;

                float tj = remote_buffer[j].time;

                Li *= (render_time - tj) / (ti - tj);
            }

            result += remote_buffer[i].position * Li;
        }
        return result;
    }
    return remote_buffer.empty() ? sf::Vector2f(0.f, 0.f) : remote_buffer.back().position;
}

float Interpolation::evaluateNewtonBasis(const std::deque<Sample>& remote_buffer,
                                         float render_time,
                                         int n,
                                         bool use_x_axis)
{
    std::vector<float> coefficients;
    coefficients.reserve(static_cast<size_t>(n));

    for (int i = 0; i < n; ++i) {
        coefficients.push_back(use_x_axis
            ? remote_buffer[static_cast<size_t>(i)].position.x
            : remote_buffer[static_cast<size_t>(i)].position.y);
    }

    for (int order = 1; order < n; ++order) {
        for (int i = n - 1; i >= order; --i) {
            const float ti = remote_buffer[static_cast<size_t>(i)].time;
            const float tj = remote_buffer[static_cast<size_t>(i - order)].time;
            const float denominator = ti - tj;

            if (denominator == 0.f) {
                return coefficients.back();
            }

            coefficients[static_cast<size_t>(i)] =
                (coefficients[static_cast<size_t>(i)] - coefficients[static_cast<size_t>(i - 1)]) /
                denominator;
        }
    }

    float result = coefficients[static_cast<size_t>(n - 1)];
    for (int i = n - 2; i >= 0; --i) {
        result = result * (render_time - remote_buffer[static_cast<size_t>(i)].time) +
                 coefficients[static_cast<size_t>(i)];
    }

    return result;
}

Vector2f Interpolation::newton(std::deque<Sample>& remote_buffer, float render_time, int n)
{
    if (remote_buffer.size() < static_cast<size_t>(n) || n <= 0) {
        return remote_buffer.empty() ? sf::Vector2f(0.f, 0.f) : remote_buffer.back().position;
    }

    return {
        evaluateNewtonBasis(remote_buffer, render_time, n, true),
        evaluateNewtonBasis(remote_buffer, render_time, n, false)
    };
}
