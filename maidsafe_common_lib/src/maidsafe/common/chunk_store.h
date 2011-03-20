/* Copyright (c) 2011 maidsafe.net limited
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

/**
 * @file chunk_store.h
 * @brief Declaration of ChunkStore interface.
 */

#ifndef MAIDSAFE_COMMON_CHUNK_STORE_H_
#define MAIDSAFE_COMMON_CHUNK_STORE_H_

#include <cstdint>
#include <string>
#include "boost/filesystem.hpp"
#include "maidsafe/common/version.h"

namespace fs = boost::filesystem;

namespace maidsafe {

/**
 * Abstract class to manage storage and retrieval of named data items (chunks).
 *
 * A chunk is a small, content-adressable piece of data. The name has to match
 * the hash (SHA512) of the content, otherwise the chunk is deemed invalid.
 *
 * The storage capacity can be limited by setting Capacity to a value greater
 * than zero. If that limit is reached, further Store operations will fail. A
 * value of zero (the default) equals infinite storage capacity.
 */
class ChunkStore {
 public:
  ChunkStore()
      : capacity_(0), size_(0) {}
  virtual ~ChunkStore() {}

  /**
   * Retrieves a chunk's content as a string.
   * @param name Chunk name
   * @return Chunk content, or empty of non-existant
   */
  virtual std::string Get(const std::string &name) = 0;

  /**
   * Retrieves a chunk's content as a file, potentially overwriting an existing
   * file of the same name.
   * @param name Chunk name
   * @param sink_file_name Path to output file
   * @return True if chunk exists and could be written to file.
   */
  virtual bool Get(const std::string &name, const fs::path &sink_file_name) = 0;

  /**
   * Stores chunk content under the given name.
   * @param name Chunk name, i.e. hash of the chunk content
   * @param content The chunk's content
   * @return True if chunk could be stored or already existed
   */
  virtual bool Store(const std::string &name, const std::string &content) = 0;

  /**
   * Stores chunk content under the given name.
   * @param name Chunk name, i.e. hash of the chunk content
   * @param source_file_name Path to input file
   * @param delete_source_file True if file can be deleted after storing
   * @return True if chunk could be stored or already existed
   */
  virtual bool Store(const std::string &name,
                     const fs::path &source_file_name,
                     bool delete_source_file) = 0;

  /**
   * Deletes a stored chunk.
   * @param name Chunk name
   * @return True if chunk deleted or non-existant
   */
  virtual bool Delete(const std::string &name) = 0;

  /**
   * Efficiently adds a locally existing chunk to another ChunkStore and
   * removes it from this one.
   * @param name Chunk name
   * @param sink_chunk_store The receiving ChunkStore
   * @return True if operation successful
   */
  virtual bool MoveTo(const std::string &name,
                      ChunkStore *sink_chunk_store) = 0;

  /**
   * Checks if a chunk exists.
   * @param name Chunk name
   * @return True if chunk exists
   */
  virtual bool Has(const std::string &name) = 0;

  /**
   * Retrieves the size of a chunk.
   * @param name Chunk name
   * @return Size in bytes
   */
  virtual std::uintmax_t Size(const std::string &name) = 0;

  /**
   * Retrieves the total size of the stored chunks.
   * @return Size in bytes
   */
  std::uintmax_t Size() {
    return size_;
  }

  /**
   * Retrieves the maximum storage capacity available to this ChunkStore.
   *
   * A capacity of zero (0) equals infinite storage space.
   * @return Capacity in bytes
   */
  std::uintmax_t Capacity() {
    return capacity_;
  }

  /**
   * Sets the maximum storage capacity available to this ChunkStore.
   *
   * A capacity of zero (0) equals infinite storage space.
   * @param capacity Capacity in bytes
   */
  void SetCapacity(const std::uintmax_t &capacity) {
    capacity_ = capacity;
  }

  /**
   * Validates a chunk, i.e. confirms if the name matches the content's hash.
   *
   * In case a chunk turns out to be invalid, it's advisable to delete it.
   * @param name Chunk name
   * @return True if chunk valid
   */
  virtual bool Validate(const std::string &name) = 0;

  /**
   * Retrieves the number of chunks held by this ChunkStore.
   * @return Chunk count
   */
  virtual std::uintmax_t Count() = 0;

  /**
   * Checks if any chunks are held by this ChunkStore.
   * @return True if no chunks stored
   */
  virtual bool Empty() = 0;

  /**
   * Deletes all stored chunks.
   */
  virtual void Clear() = 0;

 protected:
  /**
   * Increases the total size of the stored chunks.
   *
   * To be called by derived class when storing non-existant chunk.
   * @param delta Size to add to total
   */
  void IncreaseSize(const std::uintmax_t &delta) {
    size_ += delta;
  }

  /**
   * Decreases the total size of the stored chunks.
   *
   * To be called by derived class when deleting existant chunk.
   * @param delta Size to subtract from total
   */
  void DecreaseSize(const std::uintmax_t &delta) {
    size_ -= delta;
  }

 private:
  ChunkStore(const ChunkStore&);
  ChunkStore& operator=(const ChunkStore&);
  std::uintmax_t capacity_, size_;
};

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_CHUNK_STORE_H_