/* Copyright (c) 2010 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MAIDSAFE_COMMON_LOG_H_
#define MAIDSAFE_COMMON_LOG_H_

#include <string>
#include <sstream>
#include <iostream>
#include <cstdarg>
#include <map>
#include <memory>

#include "boost/current_function.hpp"
#include "boost/filesystem/path.hpp"

#include "maidsafe/common/active.h"


namespace maidsafe {

namespace log {

typedef std::map<std::string, int> FilterMap;

enum class Colour { kDefaultColour, kRed, kGreen, kYellow, kCyan };
enum class ColourMode { kFullLine, kPartialLine, kNone };

#ifdef MAIDSAFE_WIN32
class NullStream {
 public:
  NullStream() {}
  template<typename T> NullStream& operator<<(T const&) { return *this; }
  operator bool() const { return false; }
};
#else
// compile away DLOG and LOG statements
class NullStream {
 public:
  NullStream() {}
  template<typename T> NullStream& operator<<(T const&) { return *this; }
  explicit operator bool() const { return false; }
};
#endif

struct Envoid {
 public:
  Envoid() {}
  // This has to be an operator with a precedence lower than << but higher than ?:
  void operator&(NullStream&) {}
};


const int kVerbose = -1, kInfo = 0, kSuccess = 1, kWarning = 2, kError = 3, kFatal = 4;

#ifdef NDEBUG
#  define LOG(_) maidsafe::log::Envoid() & maidsafe::log::NullStream()
#else
#  define LOG(level) maidsafe::log::LogMessage(__FILE__, \
                                               __LINE__, \
                                               BOOST_CURRENT_FUNCTION, \
                                               maidsafe::log::level).messageStream()
#endif
#define TLOG(colour) maidsafe::log::GtestLogMessage(maidsafe::log::Colour::colour).messageStream()

typedef const std::string& LogEntry;

class LogMessage {
 public:
  LogMessage(const std::string &file, int line, const std::string &function, int level);
  ~LogMessage();
  std::ostringstream& messageStream() { return stream_; }
 private:
  const boost::filesystem::path kFile_;
  const int kLine_;
  const std::string kFunction_;
  const int kLevel_;
  std::ostringstream stream_;
};

class GtestLogMessage {
 public:
  explicit GtestLogMessage(Colour colour);
  ~GtestLogMessage();
  std::ostringstream& messageStream() { return stream_; }
 private:
  const Colour kColour_;
  std::ostringstream stream_;
};

class Logging {
 public:
  typedef std::function<void()> functor;
  static Logging& instance() {
    static Logging logging;
    return logging;
  }
  void Stop();
  bool Send(functor function);
  void SetFilter(FilterMap filter) {
    if (!background_)
      background_.reset(new Active);
    filter_ = filter;
  }
  void AddFilter(std::string project, int level) {
    if (!background_)
      background_.reset(new Active);
    filter_[project] = level;
  }
  FilterMap Filter() const { return filter_; }
  void SetAsync(bool async) {
    if (!background_)
      background_.reset(new Active);
    async_ = async;
  }
  bool Async() const { return async_; }
  void SetColour(ColourMode colour_mode) {
    if (!background_)
      background_.reset(new Active);
    colour_mode_ = colour_mode;
  }
  ColourMode Colour() const { return colour_mode_; }

 private:
  Logging();
  std::unique_ptr<maidsafe::Active> background_;
  FilterMap filter_;
  bool async_;
  ColourMode colour_mode_;
};

}  // namespace log

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_LOG_H_
