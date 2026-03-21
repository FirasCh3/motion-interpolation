#include <algorithm>
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

}
