#ifndef DGL_GPPROG_FRAME_H
#define DGL_GPPROG_FRAME_H

#define DECL_FRAME(name) struct name { \
private: \
    dgl::GpProg& prog; \
\
public: \
    name (dgl::GpProg& prog) : prog(prog) {} \
    dgl::GpProg& get_prog() noexcept {return prog;}


#define DECL_VAR(name) template <typename T> \
void name (T const& t) { \
    prog[#name] = t; \
}

#define DECL_ARR(name) template <typename T> \
void name ## _ ## field (size_t ind, T const& t) { \
    prog[#name "[" + std::to_string(ind) + "]"] = t; \
}

#define DECL_ARR_S(name, field) template <typename T> \
void name ## _ ## field (size_t ind, T const& t) { \
    prog[#name "[" + std::to_string(ind) + "]." #field] = t; \
}

#define DECL_END };


#endif // DGL_GPPROG_FRAME_H
