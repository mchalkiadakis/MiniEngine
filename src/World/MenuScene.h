#pragma once
#include "Scene2D.h"

class MenuScene : public Scene2D {
protected:
    void OnRenderUI() override;
    virtual void OnMenuUI() = 0;
};