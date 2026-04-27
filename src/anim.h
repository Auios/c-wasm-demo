#pragma once

/* requestAnimationFrame is ~60 Hz; we treat each frame as a fixed dt. */
#define ANIM_FPS 60.0f
#define ANIM_DT (1.0f / ANIM_FPS)
