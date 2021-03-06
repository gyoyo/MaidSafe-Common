/* Copyright 2012 MaidSafe.net limited

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

#include "maidsafe/common/error_categories.h"

#include "maidsafe/common/error.h"


namespace maidsafe {

namespace detail {

const char* CommonCategory::name() const MAIDSAFE_NOEXCEPT {
  return "MaidSafe Common";
}

std::string CommonCategory::message(int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<CommonErrors>(error_value)) {
    case CommonErrors::success:
      return "Success";
    case CommonErrors::pending_result:
      return "Result still pending";
    case CommonErrors::null_pointer:
      return "nullptr passed";
    case CommonErrors::invalid_node_id:
      return "Invalid NodeID";
    case CommonErrors::invalid_key_size:
      return "Invalid key size";
    case CommonErrors::invalid_string_size:
      return "Invalid string size";
    case CommonErrors::invalid_parameter:
      return "One or more invalid parameters were passed";
    case CommonErrors::invalid_conversion:
      return "Invalid conversion between BoundedString types";
    case CommonErrors::file_too_large:
      return "File too large";
    case CommonErrors::uninitialised:
      return "Class is uninitialised.";
    case CommonErrors::hashing_error:
      return "Error during hashing";
    case CommonErrors::symmetric_encryption_error:
      return "Error during symmetric encryption";
    case CommonErrors::symmetric_decryption_error:
      return "Error during symmetric decryption";
    case CommonErrors::compression_error:
      return "Error during compression";
    case CommonErrors::uncompression_error:
      return "Error during uncompression";
    case CommonErrors::cannot_invoke_from_this_thread:
      return "This function cannot be invoked from this thread";
    case CommonErrors::cannot_exceed_limit:
      return "Cannot exceed specified limit";
    case CommonErrors::unable_to_handle_request:
      return "Unable to handle request";
    case CommonErrors::filesystem_io_error:
      return "Filesystem IO error";
    case CommonErrors::no_such_element:
      return "Element does not exist";
    case CommonErrors::serialisation_error:
      return "Error serialising to protocol buffer";
    case CommonErrors::parsing_error:
      return "Error parsing from protocol buffer";
    case CommonErrors::not_a_directory:
      return "Path is not a directory";
    case CommonErrors::unknown:
    default:
      return "Unknown error in Common";
  }
}

std::error_condition CommonCategory::default_error_condition(
    int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<CommonErrors>(error_value)) {
    case CommonErrors::null_pointer:
    case CommonErrors::invalid_node_id:
    case CommonErrors::invalid_key_size:
    case CommonErrors::invalid_string_size:
    case CommonErrors::invalid_parameter:
      return std::errc::invalid_argument;
    case CommonErrors::file_too_large:
      return std::errc::file_too_large;
    case CommonErrors::cannot_invoke_from_this_thread:
      return std::errc::operation_not_permitted;
    case CommonErrors::unable_to_handle_request:
      return std::errc::not_supported;
    case CommonErrors::filesystem_io_error:
      return std::errc::io_error;
    case CommonErrors::not_a_directory:
      return std::errc::not_a_directory;
    default:
      return std::error_condition(error_value, *this);
  }
}


const char* AsymmCategory::name() const MAIDSAFE_NOEXCEPT {
  return "MaidSafe Asymmetric Crypto";
}

std::string AsymmCategory::message(int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<AsymmErrors>(error_value)) {
    case AsymmErrors::keys_generation_error:
      return "Error generating key pair";
    case AsymmErrors::keys_serialisation_error:
      return "Error serialising key pair";
    case AsymmErrors::keys_parse_error:
      return "Error parsing key pair";
    case AsymmErrors::invalid_private_key:
      return "Invalid private key";
    case AsymmErrors::invalid_public_key:
      return "Invalid public key";
    case AsymmErrors::data_empty:
      return "Input data is empty";
    case AsymmErrors::invalid_file:
      return "Input file is missing or empty";
    case AsymmErrors::invalid_signature:
      return "Invalid signature";
    case AsymmErrors::signature_empty:
      return "Signature is empty";
    case AsymmErrors::encryption_error:
      return "Error during asymmetric encryption";
    case AsymmErrors::decryption_error:
      return "Error during asymmetric decryption";
    case AsymmErrors::signing_error:
      return "Error during asymmetric signing";
    default:
      return "Unknown error in Asymm";
  }
}

std::error_condition AsymmCategory::default_error_condition(
    int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<AsymmErrors>(error_value)) {
    case AsymmErrors::data_empty:
    case AsymmErrors::invalid_file:
    case AsymmErrors::invalid_signature:
    case AsymmErrors::signature_empty:
    case AsymmErrors::signing_error:
      return std::errc::invalid_argument;
    default:
      return std::error_condition(error_value, *this);
  }
}


const char* PassportCategory::name() const MAIDSAFE_NOEXCEPT {
  return "MaidSafe Identity Ring";
}

std::string PassportCategory::message(int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<PassportErrors>(error_value)) {
    case PassportErrors::fob_serialisation_error:
      return "Error serialising to protocol buffer representation";
    case PassportErrors::fob_parsing_error:
      return "Error parsing fob from protocol buffer representation";
    case PassportErrors::mid_parsing_error:
      return "Error parsing MID or SMID from protocol buffer representation";
    case PassportErrors::tmid_parsing_error:
      return "Error parsing TMID from protocol buffer representation";
    case PassportErrors::no_confirmed_fob:
      return "No confirmed fob of requested type available";
    case PassportErrors::no_pending_fob:
      return "No pending fob of requested type available";
    case PassportErrors::passport_parsing_error:
      return "Error parsing passport";
    case PassportErrors::public_id_already_exists:
      return "Public ID already exists in the passport";
    case PassportErrors::no_such_public_id:
      return "No such public ID exists in the passport";
    default:
      return "Unknown error in Passport";
  }
}

std::error_condition PassportCategory::default_error_condition(
    int error_value) const MAIDSAFE_NOEXCEPT {
//  switch (static_cast<PassportErrors>(error_value)) {
//    default:
      return std::error_condition(error_value, *this);
//  }
}


const char* NfsCategory::name() const MAIDSAFE_NOEXCEPT {
  return "MaidSafe Network Filesystem";
}

std::string NfsCategory::message(int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<NfsErrors>(error_value)) {
    case NfsErrors::failed_to_get_data:
      return "Routing failed to return requested data";
    default:
      return "Unknown error in NFS";
  }
}

std::error_condition NfsCategory::default_error_condition(
    int error_value) const MAIDSAFE_NOEXCEPT {
//  switch (static_cast<NfsErrors>(error_value)) {
//    default:
      return std::error_condition(error_value, *this);
//  }
}


const char* RoutingCategory::name() const MAIDSAFE_NOEXCEPT {
  return "MaidSafe Routing";
}

std::string RoutingCategory::message(int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<RoutingErrors>(error_value)) {
    case RoutingErrors::timed_out:
      return "Timed out";
    default:
      return "Unknown error in Routing";
  }
}

std::error_condition RoutingCategory::default_error_condition(
    int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<RoutingErrors>(error_value)) {
    case RoutingErrors::timed_out:
      return std::errc::timed_out;
    default:
      return std::error_condition(error_value, *this);
  }
}


const char* VaultCategory::name() const MAIDSAFE_NOEXCEPT {
  return "MaidSafe Vault";
}

std::string VaultCategory::message(int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<VaultErrors>(error_value)) {
    case VaultErrors::failed_to_join_network:
      return "Failed to join network";
    case VaultErrors::failed_to_handle_request:
      return "Failed to handle request";
    case VaultErrors::operation_not_supported:
      return "Requested operation not supported";
    case VaultErrors::permission_denied:
      return "Permission denied for request";
    case VaultErrors::no_such_account:
      return "Account not known on this vault";
    case VaultErrors::low_space:
      return "Running low on space in account";
    case VaultErrors::not_enough_space:
      return "Not enough space in account";
    case VaultErrors::unique_data_clash:
      return "Attempt to overwrite existing unique data";
    case VaultErrors::data_available_not_given:
      return "Data is held by the network, but was not provided";
    default:
      return "Unknown error in Vault";
  }
}

std::error_condition VaultCategory::default_error_condition(
    int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<VaultErrors>(error_value)) {
    case VaultErrors::operation_not_supported:
      return std::errc::operation_not_supported;
    case VaultErrors::permission_denied:
      return std::errc::permission_denied;
    default:
      return std::error_condition(error_value, *this);
  }
}


const char* LifeStuffCategory::name() const MAIDSAFE_NOEXCEPT {
  return "LifeStuff";
}

std::string LifeStuffCategory::message(int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<LifeStuffErrors>(error_value)) {
    case LifeStuffErrors::kAuthenticationError:
    default:
      return "Unknown error in LifeStuff";
  }
}

std::error_condition LifeStuffCategory::default_error_condition(
    int error_value) const MAIDSAFE_NOEXCEPT {
  switch (static_cast<LifeStuffErrors>(error_value)) {
    case LifeStuffErrors::kAuthenticationError:
    default:
      return std::error_condition(error_value, *this);
  }
}


}  // namespace detail

}  // namespace maidsafe
