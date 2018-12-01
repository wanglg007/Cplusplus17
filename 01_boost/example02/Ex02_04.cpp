#include "Logger.h"

//void doLogging(const std::string &msg, ...) {
//    std::string logfile = "/var/MyApp/log/app.log";
//    std::unique_ptr<Logger> logger = make_logger(logfile);
//    logger->log(msg, "Hello", "world");
//}

void doLogging(const std::string &msg, ...) {
    std::string filename = "/var/MyApp/log/app.log";
    std::unique_ptr<Logger> logger =
            std::make_unique<Logger>(filename);
    logger->log(msg, "hello", "world");
}

int main() {}
