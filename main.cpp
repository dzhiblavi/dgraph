#include <iostream>
#include <filesystem>

#include "dgl/Context.h"

#include "util/logger.h"
#include "util/cargs.h"

#include "app_window.h"

int main(int argc, char** argv) {
    tmnl::logger lout(std::cout);

    try {
        dgl::Context ctx(argc, argv);
        window w1({1600, 1200, "Window 1", nullptr, nullptr});
        ctx.exec();
    } catch (std::exception const& e) {
        lout << tmnl::fclr::RED << tmnl::fmt::BOLD << "main() failed with error: " 
             << tmnl::rst::ALL << e.what() << std::endl;
    } catch (...) {
        lout << tmnl::fclr::RED << tmnl::fmt::BOLD << "main() failed with unknown error."
             << tmnl::rst::ALL;
    }
    return 0;
}

