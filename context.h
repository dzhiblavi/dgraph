#ifndef DGRAPH_CONTEXT_H
#define DGRAPH_CONTEXT_H

#include <list>
#include <iostream>
#include <string>
#include <functional>
#include <stdexcept>
#include <cassert>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <condition_variable>

namespace dgl {
class context;

class window {
    friend class context;

public:
    typedef GLFWwindow  window_t;
    typedef GLFWwindow* window_ptr;

    struct attributes {
        int width = 0;
        int height = 0;
        std::string const& title{};
        GLFWmonitor* monitor = nullptr;
        GLFWwindow* share = nullptr;
    };

private:
    window_ptr pw_;
    std::function<void()> fproc_;

    struct render_thread_ {
        std::mutex m;
        std::condition_variable cv;
        bool work = false;
        bool quit = false;
        std::thread t;

        explicit render_thread_(std::function<void()>&& f);
        ~render_thread_();

        void stop();
    } wt_;

private:
    void process_();
    void close_();

public:
    window(context& ctx, attributes* attr);

    void swap_buffers();
    void set_default_viewport();
    void set_as_main_context();
    [[nodiscard]] bool should_close();
    void set_process(std::function<void()> fproc);
};

class context {
    friend class window;

private:
    std::list<window*> vw_;

private:
    void add_window_(window* wd);

public:
    context();
    ~context();

    void exec();
};
}

#endif //DGRAPH_CONTEXT_H
