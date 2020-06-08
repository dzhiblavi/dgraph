#ifndef TMNL_CARGS_H
#define TMNL_CARGS_H

#include <map>
#include <exception>
#include <variant>
#include <string>
#include <vector>
#include <sstream>

namespace tmnl {
class cargs {
    mutable std::map<std::string, std::variant<std::string, std::vector<std::string>>> mp;

public:
    cargs(int argc, char** argv);

    std::string getStr(std::string const& name) const;

    std::vector<std::string> getStrV(std::string const& name) const;

    int getInt(std::string const& name) const;

    std::vector<int> getIntV(std::string const& name) const;

    auto begin() const {
        return mp.begin();
    }

    auto end() const {
        return mp.end();
    }
};
} // namespace tmnl

#endif // TMNL_CARGS_H
