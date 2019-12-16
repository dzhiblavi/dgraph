#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "context.h"

float genrand() {
    return 1.f * (rand() % 256) / 256.f;
}

int main() {
    try {
        dgl::context ctx;
        dgl::window::attributes attr{800, 600, "test window 1", nullptr, nullptr};

        dgl::window w1(ctx, &attr);
        dgl::window w2(ctx, &attr);
        dgl::window w3(ctx, &attr);

        w1.set_process([] {
            glClearColor(0.2f, genrand(), 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        });
        w2.set_process([] {
            glClearColor(genrand(), 0.1f, genrand(), 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        });
        w3.set_process([] {
            glClearColor(genrand(), 0.1f, 0.5f, genrand());
            glClear(GL_COLOR_BUFFER_BIT);
        });

        ctx.exec();
    } catch (std::exception const& e) {
        std::cerr << "main() failed: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "main() failed with unknown exception" << std::endl;
    }
    return 0;
}