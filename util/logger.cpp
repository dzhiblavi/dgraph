#include "logger.h"

namespace {
void log_modifier(std::ostream& os, int mod) {
    os << "\e[" << mod << "m";
}
} // namespace


namespace tmnl {
logger::logger(std::ostream& os)
    : os(os) {}

logger& logger::operator<<(fmt f) {
    log_modifier(os, (int)f);
    return *this;
}

logger& logger::operator<<(rst rs) {
    log_modifier(os, (int)rs);
    return *this;
}

logger& logger::operator<<(bclr bg) {
    log_modifier(os, (int)bg);
    return *this;
}

logger& logger::operator<<(fclr fg) {
    log_modifier(os, (int)fg);
    return *this;
}

logger& logger::operator<<(std::ostream& (*f)(std::ostream&)) {
    os << f;
    return *this;
}
} // namespace tmnl
