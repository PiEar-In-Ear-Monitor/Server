//******************************************************************************
// Copyright © 2022  Alexander O'Connor
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//******************************************************************************

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

#ifndef PIEAR_SERVER_LOGGER_H
#define PIEAR_SERVER_LOGGER_H

#ifndef PIEAR_LOG_WITH_FILE_LOCATION
    #define PIEAR_LOG_WITH_FILE_LOCATION(lvl) BOOST_LOG_SEV(PiEar::Logger::lg, (lvl)) << __FILE__ << ":" << __LINE__ << ": "
#endif

#ifndef PIEAR_LOG_WITHOUT_FILE_LOCATION
    #define PIEAR_LOG_WITHOUT_FILE_LOCATION(lvl) BOOST_LOG_SEV(PiEar::Logger::lg, (lvl))
#endif

namespace PiEar::Logger {
    [[maybe_unused]] extern boost::log::sources::severity_logger< boost::log::trivial::severity_level > lg; //!< The logger
    /**
     * @brief Initialise the logger
     * @param log_level defaults to info
     */
    void init( int log_level = boost::log::trivial::info );
}

#endif //PIEAR_SERVER_LOGGER_H
