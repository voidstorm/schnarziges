#pragma once
#include "./sm/api.h"
#include "./sm/functional.hpp"
#include "sm/system_logger.hpp"
#include "sm/gfx/render_context.h"
#include "sm/gfx/app.h"

struct GameCreateInfo {

};


auto make_game(const GameCreateInfo& gci) {
   return [=](const sm::app::AppCreateInfo &app_info, sm::app::AppCallbackInfo callbacks, sm::app::AppMainCallbacks appmain) {
      return [=](const sm::gfx::rc::RenderContextCreateInfo &rci) {
         auto rc = sm::gfx::rc::make_render_context(rci);
         rc();
         return sm::app::make_app(app_info, callbacks, appmain);
      };
   };
};
