/* Copyright (c) 2009 maidsafe.net limited
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

#include "maidsafe/common/log.h"

#ifdef MAIDSAFE_WIN32
#  include <Windows.h>
#else
#  include <unistd.h>
#endif
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>
#include <cstdio>

#include "maidsafe/common/utils.h"

namespace fs = boost::filesystem;


namespace maidsafe {

namespace log {

namespace {

std::mutex g_console_mutex;

#ifdef MAIDSAFE_WIN32

WORD GetColourAttribute(Colour colour) {
  switch (colour) {
    case Colour::kRed:
      return FOREGROUND_RED | FOREGROUND_INTENSITY;
    case Colour::kGreen:
      return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    case Colour::kYellow:
      return FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
    case Colour::kCyan:
      return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    default:
      return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
  }
}

void ColouredPrint(Colour colour, const std::string &text) {
  CONSOLE_SCREEN_BUFFER_INFO console_info_before;
  const HANDLE kConsoleHandle(GetStdHandle(STD_OUTPUT_HANDLE));
  std::lock_guard<std::mutex> lock(g_console_mutex);
  if (kConsoleHandle != INVALID_HANDLE_VALUE) {
    int got_console_info = GetConsoleScreenBufferInfo(kConsoleHandle, &console_info_before);
    fflush(stdout);
    SetConsoleTextAttribute(kConsoleHandle, GetColourAttribute(colour));
    printf("%s", text.c_str());
    fflush(stdout);
    if (got_console_info != 0)
      SetConsoleTextAttribute(kConsoleHandle, console_info_before.wAttributes);
  } else {
    printf("%s", text.c_str());
  }
  fflush(stdout);
}

#else

const char* GetAnsiColourCode(Colour colour) {
  switch (colour) {
    case Colour::kRed:
      return "1";
    case Colour::kGreen:
      return "2";
    case Colour::kYellow:
      return "3";
    case Colour::kCyan:
      return "6";
    default:
      return nullptr;
  }
}

void ColouredPrint(Colour colour, const std::string &text) {
  // On non-Windows platforms, we rely on the TERM variable.
  std::lock_guard<std::mutex> lock(g_console_mutex);
  auto env_ptr = getenv("TERM");
  const std::string kTerm(env_ptr ? env_ptr : "");
  const bool kTermSupportsColour(kTerm == "xterm" ||
                                 kTerm == "xterm-color" ||
                                 kTerm == "xterm-256color" ||
                                 kTerm == "screen" ||
                                 kTerm == "linux" ||
                                 kTerm == "cygwin");
  static const bool in_colour_mode = ((isatty(fileno(stdout)) != 0) && kTermSupportsColour);
  const bool kUseColour = in_colour_mode && (colour != Colour::kDefaultColour);
  if (kUseColour)
    printf("\033[0;3%sm", GetAnsiColourCode(colour));
  printf("%s", text.c_str());
  if (kUseColour)
    printf("\033[m");
  fflush(stdout);
}

#endif

}  // unnamed namespace

LogMessage::LogMessage(const std::string &file, int line, const std::string &function, int level)
    : kFile_(file),
      kLine_(line),
      kFunction_(function),
      kLevel_(level),
      stream_() {}

LogMessage::~LogMessage() {
  auto itr(kFile_.end()), begin_itr(kFile_.begin());
  std::string project;
  while (itr != begin_itr) {
    if (*(--itr) == "maidsafe") {
      project = (*(++itr)).string();
      break;
    }
  }

  FilterMap filter(Logging::instance().Filter());
  auto filter_itr(filter.find("*"));
  if (filter_itr == filter.end()) {
    filter_itr = filter.find(project);
  }
  if (filter_itr == filter.end())
    return;

  if ((*filter_itr).second > kLevel_)
    return;

  fs::path current_file;
  for (; itr != kFile_.end(); ++itr)
    current_file /= *itr;

  char log_level;
  Colour colour(Colour::kDefaultColour);
  switch (kLevel_) {
    case kVerbose:
      log_level = 'V';
      colour = Colour::kCyan;
      break;
    case kInfo:
      log_level = 'I';
      colour = Colour::kDefaultColour;
      break;
    case kSuccess:
      log_level = 'S';
      colour = Colour::kGreen;
      break;
    case kWarning:
      log_level = 'W';
      colour = Colour::kYellow;
      break;
    case kError:
      log_level = 'E';
      colour = Colour::kRed;
      break;
    case kFatal:
      log_level = 'F';
      colour = Colour::kRed;
      break;
    default:
      log_level = ' ';
  }

  std::ostringstream oss;
  oss << log_level << " " << std::this_thread::get_id();
#if defined(WIN32)
  oss << '\t';
#else
  oss << ' ';
#endif
  oss << boost::posix_time::microsec_clock().universal_time().time_of_day();
  std::string coloured_log_entry(oss.str());
  oss.str("");

  oss << " " << current_file.string() << ":" << kLine_ << "] ";
//  oss << " Function: " << function_ << "] ";

  oss << stream_.str() << '\n';
  std::string plain_log_entry(oss.str());
  ColourMode colour_mode(Logging::instance().Colour());
  auto print_functor([colour, coloured_log_entry, plain_log_entry, colour_mode] {
      if (colour_mode == ColourMode::kFullLine) {
        ColouredPrint(colour, coloured_log_entry + plain_log_entry);
      } else if (colour_mode == ColourMode::kPartialLine) {
        ColouredPrint(colour, coloured_log_entry);
        printf("%s", plain_log_entry.c_str());
      } else {
        printf("%s%s", coloured_log_entry.c_str(), plain_log_entry.c_str());
      }
      fflush(stdout);
  });

  if (Logging::instance().Async()) {
    if (!Logging::instance().Send(print_functor))
      ColouredPrint(Colour::kCyan, "*** After stop *** " + coloured_log_entry + plain_log_entry);
  } else {
    print_functor();
  }
}

GtestLogMessage::GtestLogMessage(Colour colour) : kColour_(colour), stream_() {}

GtestLogMessage::~GtestLogMessage() {
  if (Logging::instance().Async()) {
    std::string log_entry(stream_.str());
    Colour colour(kColour_);
    if (!Logging::instance().Send([colour, log_entry] { ColouredPrint(colour, log_entry); }))
      ColouredPrint(colour, log_entry);
  } else {
    ColouredPrint(kColour_, stream_.str());
  }
}

Logging::Logging()
    : background_(new Active),
      filter_(),
      async_(true),
      colour_mode_(ColourMode::kPartialLine) {}

void Logging::Stop() {
  background_.reset();
}

bool Logging::Send(functor voidfunction) {
  return background_ ? background_->Send(voidfunction) : false;
}


}  // namespace log
}  // namespace maidsafe
