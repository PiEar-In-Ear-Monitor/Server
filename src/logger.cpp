#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include "logger.h"

namespace PiEar::Logger {
    [[maybe_unused]] boost::log::sources::severity_logger< boost::log::trivial::severity_level > lg;
    void init( int log_level ) {
        boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%TimeStamp%]: %Message%");
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= log_level);
        boost::log::add_common_attributes();
    }
}