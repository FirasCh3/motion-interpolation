#include "Interpolation.h"
#include "Player.h"
Vector2f Interpolation::lerp(Vector2f old_pos, Vector2f new_pos) {
    return old_pos + (old_pos - new_pos) * smoothing_factor;
}
