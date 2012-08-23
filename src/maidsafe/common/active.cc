/* Copyright (c) 2012 maidsafe.net limited
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

#include "maidsafe/common/active.h"


namespace maidsafe {

Active::Active() : stopping_(false),
                   functors_(),
                   mutex_(new std::mutex),
                   weak_ptr_to_mutex_(mutex_),
                   condition_(),
                   thread_([=] { Run(); }) {}

Active::~Active() {
  Stop();
}

void Active::Stop() {
  if (std::shared_ptr<std::mutex> mutex = weak_ptr_to_mutex_.lock()) {
    std::unique_lock<std::mutex> lock(*mutex);
    stopping_ = true;
    condition_.notify_one();
    condition_.wait_for(lock, std::chrono::seconds(10), [this] { return functors_.empty(); });
    thread_.join();
  }
}

bool Active::Send(Functor functor) {
  if (std::shared_ptr<std::mutex> mutex = weak_ptr_to_mutex_.lock()) {
    std::unique_lock<std::mutex> lock(*mutex);
    if (stopping_)
      return false;
    functors_.push(functor);
    condition_.notify_one();
    return true;
  } else {
    return false;
  }
}

void Active::Run() {
  bool stop(false);
  while (!stop) {
    Functor functor;
    {
      std::unique_lock<std::mutex> lock(*mutex_);
      condition_.wait(lock, [this] { return !functors_.empty(); });  // NOLINT (Fraser)
      functor = functors_.front();
      functors_.pop();
      stop = functors_.empty() && stopping_;
      condition_.notify_one();
    }
    functor();
  }
}

}  // namespace maidsafe
