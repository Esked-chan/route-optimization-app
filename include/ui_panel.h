#ifndef UI_PANEL_H
#define UI_PANEL_H

#include "ui.h"
#include "render.h"

void render_ui_panel(RenderContext* ctx, const UIState* ui_state);
void render_button(RenderContext* ctx, UIButton* button);

#endif // UI_PANEL_H