#include "./sm/gfx/render_context.h"
#include "GLFW/glfw3.h"

SCHNARZ_API std::function<bool(void)> sm::gfx::rc::make_render_context(const RenderContextCreateInfo & rci) {
   return []()->bool {
      return true;
   };
}
