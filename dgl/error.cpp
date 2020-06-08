#include "error.h"

namespace {
std::mutex cerr_m;
} // namespace

namespace dgl {
void glCheckError_(char const *file, int line, char const* func) {
#ifdef CHECK_ERROR_VERBOSE
    std::cerr << "CheckError call: " << file << ' ' << line << std::endl;
#endif
    GLenum errorCode;
    std::string s;

    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        s += "{" + std::to_string(errorCode) + ":" + error + "}\n";
    }

    if (!s.empty())
        DGL_EXC_FULL(s, file, line, func);
}

#ifdef DEBUG
void errlog_impl_() {}
#endif

std::lock_guard<std::mutex> stderr_lock() {
    return std::lock_guard<std::mutex>(cerr_m);
}

void GLAPIENTRY glErrorCallback(GLenum source
        , GLenum type
        , GLuint id
        , GLenum severity
        , GLsizei length
        , GLchar const* message
        , void const* userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

void glfwErrorCallback(int code, char const* what) {
    DGL_EXC(std::to_string(code) + ": " + std::string(what));
}

} // namespace dgl
