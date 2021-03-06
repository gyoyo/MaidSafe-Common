/* Copyright 2008 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#ifndef MAIDSAFE_COMMON_NODE_ID_H_
#define MAIDSAFE_COMMON_NODE_ID_H_

#include <cstdint>
#include <string>
#include <vector>

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/error.h"


namespace maidsafe {

class NodeId {
 public:
  enum IdType { kMaxId, kRandomId };
  enum EncodingType { kBinary, kHex, kBase32, kBase64 };
  enum { kSize = crypto::SHA512::DIGESTSIZE };

  // Creates an ID equal to 0.
  NodeId();

  NodeId(const NodeId& other);
  NodeId(NodeId&& other);
  NodeId& operator=(NodeId other);

  // Creates an ID = (2 ^ kKeySizeBits) - 1 or a random ID in the interval [0, 2 ^ kKeySizeBits).
  explicit NodeId(IdType type);

  // Creates a NodeId from a raw (decoded) string.  Will throw if id is invalid.
  explicit NodeId(const std::string& id);

  // Creates a NodeId from a SHA512 hash.
  explicit NodeId(const crypto::SHA512Hash& id);

  // Creates a NodeId from an encoded string.  Will throw if id is invalid.
  NodeId(const std::string& id, EncodingType encoding_type);


  // Checks if id1 is closer in XOR distance to target_id than id2.
  static bool CloserToTarget(const NodeId& id1, const NodeId& id2, const NodeId& target_id);

  // Decoded representation of the ID.
  const std::string string() const;

  // Encoded representation of the ID.
  const std::string ToStringEncoded(const EncodingType& encoding_type) const;

  bool IsZero() const;

  // XOR distance between two IDs.  XOR bit to bit.
  NodeId& operator^=(const NodeId& rhs);

  friend bool operator==(const NodeId& lhs, const NodeId& rhs);
  friend bool operator<(const NodeId& lhs, const NodeId& rhs);
  friend std::string DebugId(const NodeId& node_id);

 private:
  std::string EncodeToBinary() const;
  void DecodeFromBinary(const std::string& binary_id);
  std::string raw_id_;
};

inline bool operator==(const NodeId& lhs, const NodeId& rhs) { return lhs.raw_id_ == rhs.raw_id_; }
inline bool operator!=(const NodeId& lhs, const NodeId& rhs) { return !operator==(lhs, rhs); }
inline bool operator<(const NodeId& lhs, const NodeId& rhs) { return lhs.raw_id_ < rhs.raw_id_; }
inline bool operator>(const NodeId& lhs, const NodeId& rhs) { return operator<(rhs, lhs); }
inline bool operator<=(const NodeId& lhs, const NodeId& rhs) { return !operator>(lhs, rhs); }
inline bool operator>=(const NodeId& lhs, const NodeId& rhs) { return !operator<(lhs, rhs); }
// XOR distance between two IDs.  XOR bit to bit.
inline NodeId operator^(NodeId lhs, const NodeId& rhs) {
  lhs ^= rhs;
  return lhs;
}

// Returns an abbreviated hex representation of node_id.
std::string DebugId(const NodeId& node_id);

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_NODE_ID_H_
