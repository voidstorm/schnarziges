#pragma once
#include "./sm/api.h"
#include "./sm/functional.hpp"

namespace sm::gfx::rc {

struct RenderContextCreateInfo {

};

SCHNARZ_API std::function<bool(void)> make_render_context(const RenderContextCreateInfo& rci);
}