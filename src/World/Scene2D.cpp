#include "Scene2D.h"

void Scene2D::Render(const RenderContext& ctx) const {
    const_cast<Scene2D*>(this)->OnRenderUI();
}