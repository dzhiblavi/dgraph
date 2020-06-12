#ifndef DGL_GPPROG_FRAME_H
#define DGL_GPPROG_FRAME_H

#define DECL_FRAME(name) struct name { \
private: \
    dgl::GpProg& prog; \
\
public: \
    name (dgl::GpProg& prog) : prog(prog) {} \
    dgl::GpProg& get_prog() noexcept {return prog;}


#define DECL_VAR(name) void name () { \
    return prog[#name]; \
}

#define DECL_END };


#endif // DGL_GPPROG_FRAME_H
