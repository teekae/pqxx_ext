
#pragma once

#include <pqxx/pqxx>
#include <string>

namespace pqxx {
template <typename T> struct string_traits<std::vector<T>> {
  [[nodiscard]] static zview to_buf(char *begin, char *end,
                                    const std::vector<T> &value) {
    return generic_to_buf(begin, end, value);
  }

  static char *into_buf(char *begin, char *end, const std::vector<T> &value) {
    const auto sizeDiff = end - begin;

    *begin++ = '{';
    if (!value.empty()) {
      for (const auto &v : value) {
        begin = string_traits<T>::into_buf(begin, end, v);

        if (*begin == '\0') {
          // Remove null terminators
          --begin;
        }

        *begin++ = ',';
      }
      // Remove the last comma
      if (begin > begin - 1 && *(begin - 1) == ',') {
        --begin;
      }
    }

    *begin++ = '}';
    *begin++ = '\0';
    return begin;
  }

  [[nodiscard]] static std::vector<T> from_string(std::string_view text) {
    if (text.empty()) {
      return {};
    }

    std::vector<T> result;

    if (text.front() != '{' || text.back() != '}') {
      throw conversion_error("Invalid format for string array: " +
                             std::string(text));
    }

    // Trim off the brackets
    auto content = text.substr(1, text.size() - 2);
    size_t pos = 0;
    while ((pos = content.find(',')) != std::string_view::npos) {
      result.emplace_back(
          string_traits<T>::from_string(content.substr(0, pos)));
      content.remove_prefix(pos + 1);
    }
    if (!content.empty()) {
      result.emplace_back(string_traits<T>::from_string(content));
    }

    return result;
  }

  [[nodiscard]] static std::size_t
  size_buffer(const std::vector<T> &value) noexcept {
    std::size_t size = 2; // for the curly braces
    for (const auto &v : value) {
      size += string_traits<T>::size_buffer(v);
    }

    size += value.size() - 1; // for the commas

    ++size; // for the null terminator

    return size;
  }
};
} // namespace pqxx
