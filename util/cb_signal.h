#ifndef CB_SIGNAL_H
#define CB_SIGNAL_H

#include <list>

template <typename... Args>
class cb_signal {
public:
    typedef std::function<void(Args...)> cb_t;
    typedef std::list<cb_t> cbs_t;

    class connection {
        typename cbs_t::iterator it;
        cb_signal* sig;

    public:
        connection(typename cbs_t::iterator it, cb_signal* sig)
            : it(it), sig(sig) {}

        void disconnect() {
            if (0 < sig->emitting) {
                *it = cb_t();
            } else {
                sig->cbs.erase(it);
            }
        }
    };

private:
    cbs_t cbs;
    bool* destroyed = nullptr;
    size_t emitting = 0;

public:
    cb_signal() = default;

    ~cb_signal() {
        if (nullptr != destroyed) {
            *destroyed = true;
        }
    }

    connection connect(cb_t const& f) {
        cbs.push_front(f);
        return connection(cbs.begin(), this);
    }

    void operator()(Args... args) {
        bool cur_destroyed = false;
        bool* old_destroyed = destroyed;

        destroyed = &cur_destroyed;
        ++emitting;

        try {
            for (cb_t& f : cbs) {
                if (f) {
                    try {
                        f(args...);
                    } catch (...) {
                        // ignore
                    }
                }
            }
        } catch (...) {
            // ignore
        }

        --emitting;

        if (0 == emitting) {
            for (auto it = cbs.begin(); it != cbs.end(); ) {
                if (*it) {
                    ++it;
                } else {
                    it = cbs.erase(it);
                }
            }
        }

        destroyed = old_destroyed;
    }
};

#endif // CB_SIGNAL_H
