#include "cargs.h"

namespace {
template <typename T>
void checkPresent(std::map<std::string, T> const& m, std::string const& v) {
    if (m.find(v) == m.end())
        throw std::runtime_error("Value is not present in the map: " + v);
}

std::string getStr(int& argc, char**& argv) {
    std::string r = std::string(argv[0]);
    --argc; ++argv;
    return r;
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
} // namespace

namespace tmnl {
cargs::cargs(int argc, char** argv) {
    mp["path"] = ::getStr(argc, argv);

    std::vector<std::string> v;
    while (argc > 0) {
        std::string key = ::getStr(argc, argv);

        if (key[0] != '-') {
            throw std::runtime_error("cargs: expected key, found: '" + key + "'");
        }

        v.clear();
        while (argc > 0) {
            std::string value = ::getStr(argc, argv);

            if (value[0] == '-') {
                if (value != "-") {
                    ++argc; --argv;
                }

                break;
            }

            v.push_back(value);
        }

        if (v.size() == 1) {
            mp[key] = v[0];
        } else if (v.size() > 1) {
            mp[key] = v;
        } else {
            throw std::runtime_error("cargs: expected key");
        }
    }
}

std::string cargs::getStr(std::string const& name) const {
    checkPresent(mp, name);
    return std::visit(overloaded {
            [] (std::string const& s) { return s; },
            [] (std::vector<std::string> const& v) { return v[0]; },
            }, mp[name]);
}

std::vector<std::string> cargs::getStrV(std::string const& name) const {
    checkPresent(mp, name);
    return std::visit(overloaded {
            [] (std::string const& s) { return std::vector{s}; },
            [] (std::vector<std::string> const& v) { return v; },
            }, mp[name]);
}

int cargs::getInt(std::string const& name) const {
    std::stringstream ss(getStr(name));
    int x;
    ss >> x;
    return x;
}

std::vector<int> cargs::getIntV(std::string const& name) const {
    std::vector<std::string> s = getStrV(name);
    std::vector<int> r;

    std::for_each(s.begin(), s.end(), [&r] (std::string const& sx) {
        std::stringstream ss(sx);
        int x;
        ss >> x;
        r.push_back(x);
    });

    return r;
}
} // namespace tmnl
