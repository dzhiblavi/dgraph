#ifndef TMNL_LOGGER_H
#define TMNL_LOGGER_H

#include <iostream>

namespace tmnl {
enum class fmt {
    BOLD = 1,
    DIM = 2,
    UNDERLINED = 4,
    BLINK = 5,
    INVERT = 7,
    HIDDEN = 8,
};

enum class rst {
    ALL = 0,
    BOLD = 21,
    DIM = 22,
    UNDERLINED = 24,
    BLINK = 25,
    INVERT = 27,
    HIDDEN = 28,
};

enum class fclr {
    DEFAULT = 39,
    BLACK = 30,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    LIGHTGRAY = 37,
    DARKGRAY = 90,
    LIGHTRED = 91,
    LIGHTGREEN = 92,
    LIGHGYELLOW = 93,
    LIGHTBLUE = 94,
    LIGHTMAGENTA = 95,
    LIGHTCYAN = 96,
    WHITE = 97,
};  

enum class bclr {
    DEFAULT = 49,
    BLACK = 40,
    RED = 41,
    GREEN = 42,
    YELLOW = 43,
    BLUE = 44,
    MAGENTA = 45,
    CYAN = 46,
    LIGHTGRAY = 47,
    DARKGRAY = 100,
    LIGHTRED = 101,
    LIGHTGREEN = 102,
    LIGHGYELLOW = 103,
    LIGHTBLUE = 104,
    LIGHTMAGENTA = 105,
    LIGHTCYAN = 106,
    WHITE = 107,
};


class logger {
    std::ostream& os;

public:
    logger(std::ostream& os);

    logger& operator<<(fmt f);

    logger& operator<<(rst rs);

    logger& operator<<(bclr bg);

    logger& operator<<(fclr fg);

    logger& operator<<(std::ostream& (*f)(std::ostream&));

    template <typename T>
    logger& operator<<(T&& v) {
        os << v;
        return *this;
    }
};
} // namespace tmnl

#endif // TMNL_LOGGER_H
