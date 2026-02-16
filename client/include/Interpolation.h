#ifndef CLIENT_INTERPOLATION_H
#define CLIENT_INTERPOLATION_H
#include "SFML/System/Vector2.hpp"
using namespace std;
using namespace sf;
class Interpolation {
    public:
    Vector2f lerp(Vector2f old_pos, Vector2f new_pos);
    private:
    float smoothing_factor = 1.f;
};
#endif //CLIENT_INTERPOLATION_H
