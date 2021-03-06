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

#include "maidsafe/common/node_id.h"

#include <bitset>

#include "maidsafe/common/error_categories.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/utils.h"


namespace maidsafe {

NodeId::NodeId() : raw_id_(kSize, 0) {}

NodeId::NodeId(const NodeId& other) : raw_id_(other.raw_id_) {}

NodeId::NodeId(NodeId&& other) : raw_id_(std::move(other.raw_id_)) {}

NodeId& NodeId::operator=(NodeId other) {
  using std::swap;
  swap(raw_id_, other.raw_id_);
  return *this;
}

NodeId::NodeId(IdType type) : raw_id_(
    [type]()->std::string {
        switch (type) {
          case kMaxId:
            return std::string(kSize, -1);
          case kRandomId:
            return RandomString(kSize);
          default:
            ThrowError(CommonErrors::invalid_parameter);
        }
        return "";
    }()) {}

NodeId::NodeId(const std::string& id) : raw_id_(id) {
  if (raw_id_.size() != kSize) {
    raw_id_.clear();
    ThrowError(CommonErrors::invalid_node_id);
  }
}

NodeId::NodeId(const crypto::SHA512Hash& id) : raw_id_(id.string()) {}

NodeId::NodeId(const std::string& id, EncodingType encoding_type) : raw_id_() {
  try {
    switch (encoding_type) {
      case kBinary: DecodeFromBinary(id);
        break;
      case kHex: raw_id_ = DecodeFromHex(id);
        break;
      case kBase32: raw_id_ = DecodeFromBase32(id);
        break;
      case kBase64: raw_id_ = DecodeFromBase64(id);
        break;
      default: raw_id_ = id;
    }
  }
  catch(const std::exception& e) {
    LOG(kError) << "NodeId Ctor: " << e.what();
    raw_id_.clear();
  }
  if (raw_id_.size() != kSize) {
    raw_id_.clear();
    ThrowError(CommonErrors::invalid_node_id);
  }
}

std::string NodeId::EncodeToBinary() const {
  std::string binary;
  binary.reserve(kSize);
  for (size_t i = 0; i < kSize; ++i) {
    std::bitset<8> temp(static_cast<int>(raw_id_[i]));
    binary += temp.to_string();
  }
  return binary;
}

void NodeId::DecodeFromBinary(const std::string& binary_id) {
  std::bitset<8 * kSize> binary_bitset(binary_id);
  if (raw_id_.size() != kSize)
    raw_id_.assign(kSize, 0);
  for (size_t i = 0; i < kSize; ++i) {
    std::bitset<8> temp(binary_id.substr(i * 8, 8));
    raw_id_[i] = static_cast<char>(temp.to_ulong());
  }
}

bool NodeId::CloserToTarget(const NodeId& id1, const NodeId& id2, const NodeId& target_id) {
  for (uint16_t i = 0; i < kSize; ++i) {
    unsigned char result1 = id1.raw_id_[i] ^ target_id.raw_id_[i];
    unsigned char result2 = id2.raw_id_[i] ^ target_id.raw_id_[i];
    if (result1 != result2)
      return result1 < result2;
  }
  return false;
}

const std::string NodeId::string() const {
  return raw_id_;
}

const std::string NodeId::ToStringEncoded(const EncodingType& encoding_type) const {
  switch (encoding_type) {
    case kBinary:
      return EncodeToBinary();
    case kHex:
      return EncodeToHex(raw_id_);
    case kBase32:
      return EncodeToBase32(raw_id_);
    case kBase64:
      return EncodeToBase64(raw_id_);
    default:
      return raw_id_;
  }
}

bool NodeId::IsZero() const {
  for (auto i : raw_id_) {
    if (i != 0)
      return false;
  }
  return true;
}

NodeId& NodeId::operator^=(const NodeId& rhs) {
  for (uint16_t i(0); i != kSize; ++i)
    raw_id_[i] ^= rhs.raw_id_[i];
  return *this;
}

std::string DebugId(const NodeId& node_id) {
  return HexSubstr(node_id.raw_id_);
}

}  // namespace maidsafe
