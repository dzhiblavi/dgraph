#ifndef DGL_ERROR_H
#define DGL_ERROR_H

#include <stdexcept>
#include <mutex>
#include <iostream>
#include <string>

#include <GL/glew.h>

namespace dgl {
struct error : std::runtime_error {
    using std::runtime_error::runtime_error;
    using std::runtime_error::what;
};

//#define CHECK_ERROR_VERBOSE

#define EQSTR(X) #X + "=" + std::string(X)

#define DGL_ERROR_FULL(X, F, L, f) dgl::error(std::string(X) + ": " \
                        + F + "\n" \
                        + EQSTR(f) + "(...)\n" \
                        + "line=" + std::to_string(L) + "\n")


#define DGL_ERROR(X) DGL_ERROR_FULL(X, __FILE__, __LINE__, __func__)

#define DGL_EXC_FULL(X, F, L, f) throw DGL_ERROR_FULL(X, F, L, f)

#define DGL_EXC(X) DGL_EXC_FULL(X, __FILE__, __LINE__, __func__)

#ifdef DEBUG
#define glCheckError() glCheckError_(__FILE__, __LINE__, __func__)
#else
#define glCheckError()
#endif

void glCheckError_(char const* file, int line, char const* func);

void GLAPIENTRY glErrorCallback(GLenum source
        , GLenum type
        , GLuint id
        , GLenum severity
        , GLsizei length
        , GLchar const* message
        , void const* userParam);

void glfwErrorCallback(int code, char const* what);

std::lock_guard<std::mutex> stderr_lock();


#ifdef DEBUG
void errlog_impl_();

template <typename T, typename... Args>
void errlog_impl_(T&& t, Args&&... args) {
    std::cerr << t << ", ";
    errlog_impl_(std::forward<Args>(args)...);
}


template <typename... Args>
void errlog(Args&&... args) {
    auto lock = stderr_lock();
    std::cerr << "{";
    errlog_impl_(std::forward<Args>(args)...);
    std::cerr << "\b\b}" << std::endl;
}
#else
void errlog(...) {}
#endif
} // namespace dgl

#endif // DGL_ERROR_H

