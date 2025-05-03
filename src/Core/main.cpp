#include "MiniEngineApp.h"

int main() {
    MiniEngineApp engine;

    if (!engine.Init()) {
        std::cerr << "Engine initialization failed!" << std::endl;
        return -1;
    }

    engine.Run();
    return 0;
}