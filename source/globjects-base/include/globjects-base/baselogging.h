#pragma once

#include <globjects-base/globjects-base_api.h>

#include <globjects-base/LogMessage.h>
#include <globjects-base/LogMessageBuilder.h>

namespace glo
{

class AbstractLogHandler;
class LogMessageBuilder;

/**
  * Creates a stream like object (LogMessageBuilder) to create a LogMessage from the objects
  * passed to it and sends the LogMessage to the global LogMessageHandler when the builder goes out of scope.
  * Similar to `qDebug()` from Qt.
  *
  * \code{.cpp}
  * info() << "Message: " << 3.14;
  * \endcode
  */
GLOBJECTS_BASE_API LogMessageBuilder info(LogMessage::Level level = LogMessage::Info);
GLOBJECTS_BASE_API LogMessageBuilder debug();
GLOBJECTS_BASE_API LogMessageBuilder warning();
GLOBJECTS_BASE_API LogMessageBuilder critical();
GLOBJECTS_BASE_API LogMessageBuilder fatal();

GLOBJECTS_BASE_API void setLoggingHandler(AbstractLogHandler * handler);
GLOBJECTS_BASE_API AbstractLogHandler * loggingHandler();

GLOBJECTS_BASE_API void setVerbosityLevel(LogMessage::Level verbosity);
GLOBJECTS_BASE_API LogMessage::Level verbosityLevel();

/**
 * Uses formatString to write on the usual logging streams.
 *
 *  Sample usage:
 *  \code{.cpp}
 *      info("This is a test: %; pi = %+0E10.5;", 42, 3.141592653589793); // output: "This is a test: 42 pi = +3.14159E+00"
 *      info("%; - %X; - %rf?_10.2;", "a string", 255, 2.71828182846); // output: "a string - 255 - ______2.72"
 *  \endcode
 *
 *   \see formatString
 */
template <typename... Arguments>
void info(const char* format, Arguments... arguments);

/**
 *  \see info
 */
template <typename... Arguments>
void debug(const char* format, Arguments... arguments);

/**
 *  \see info
 */
template <typename... Arguments>
void warning(const char* format, Arguments... arguments);

/**
 *  \see info
 */
template <typename... Arguments>
void critical(const char* format, Arguments... arguments);

/**
 *  \see info
 */
template <typename... Arguments>
void fatal(const char* format, Arguments... arguments);

} // namespace glo

#include <globjects-base/baselogging.hpp>