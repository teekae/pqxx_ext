
#pragma once

#include <pqxx/pqxx>

#include <chrono>

namespace internal {

template <typename Duration, typename TValue = int>
Duration parseDurationToOffset(std::string_view &text, const size_t offset) {
  const auto begin = text.begin();

  auto value = TValue{0};

  auto res = std::from_chars(begin, begin + offset, value);

  if (res.ec != std::errc{}) {

    throw std::runtime_error("parse int failed");
  }

  // Consume what we have read plus the character to search for
  text = text.substr(offset + 1);

  return Duration(value);
}

template <typename Duration, typename TValue = int>
Duration parseDurationUntil(std::string_view &text, const char target) {

  const auto offset = text.find_first_of(target);

  return parseDurationToOffset<Duration, TValue>(text, offset);
}
} // namespace internal

namespace pqxx {
template <> struct string_traits<std::chrono::system_clock::time_point> {
  [[nodiscard]] static zview
  to_buf(char *begin, char *end,
         const std::chrono::system_clock::time_point &value) {
    return generic_to_buf(begin, end, value);
  }

  static char *into_buf(char *begin, char *end,
                        const std::chrono::system_clock::time_point &value) {

    const auto sizeDiff = end - begin;

    // Some platforms have higher precision than microseconds
    // so we need to convert to microseconds to get the expected format
    using sys_time = std::chrono::time_point<std::chrono::system_clock,
                                             std::chrono::microseconds>;

    const sys_time timePointInMicroseconds =
        std::chrono::time_point_cast<std::chrono::microseconds>(value);

    const auto result = std::format_to_n(begin, sizeDiff, "{:%F %T%z}",
                                         timePointInMicroseconds);

    begin = result.out;

    *begin++ = '\0';

    return begin;
  }

  [[nodiscard]] static std::chrono::system_clock::time_point
  from_string(std::string_view text) {
    using namespace std::chrono;

    const auto year =
        ::internal::parseDurationUntil<std::chrono::year>(text, '-');
    const auto month =
        ::internal::parseDurationUntil<std::chrono::month, unsigned>(text, '-');
    const auto day =
        ::internal::parseDurationUntil<std::chrono::day>(text, ' ');

    const auto ymd = year_month_day(year, month, day);

    const sys_days dayPoint = sys_days(ymd);

    const auto hours =
        ::internal::parseDurationUntil<std::chrono::hours>(text, ':');
    const auto minutes =
        ::internal::parseDurationUntil<std::chrono::minutes>(text, ':');
    const auto seconds =
        ::internal::parseDurationUntil<std::chrono::seconds>(text, '.');

    // Find where we add the timezone
    const auto offset = text.find_first_of("+-");

    const auto tzSign = text[offset];

    const auto micro =
        ::internal::parseDurationToOffset<microseconds>(text, offset);

    // now `text` should start at the four timezone digits (e.g. "0000")
    int tz_h = 0;
    int tz_m = 0;
    if (text.size() >= 4) {
      auto r1 = std::from_chars(text.data(), text.data() + 2, tz_h);
      auto r2 = std::from_chars(text.data() + 2, text.data() + 4, tz_m);
      if (r1.ec != std::errc{} || r2.ec != std::errc{}) {
        throw std::runtime_error("parse timezone failed");
      }
    } else {
      // no explicit timezone -> assume +0000
      tz_h = 0;
      tz_m = 0;
    }

    int tz_total_minutes = tz_h * 60 + tz_m;
    if (tzSign == '-') {
      tz_total_minutes = -tz_total_minutes;
    }
    const auto tz_offset = std::chrono::minutes{tz_total_minutes};

    const sys_time<microseconds> local_tp_us =
        dayPoint + hours + minutes + seconds + micro;

    // convert to UTC by subtracting the timezone offset (local = UTC + offset)
    const sys_time<microseconds> utc_tp_us =
        local_tp_us - duration_cast<microseconds>(tz_offset);

    // convert to system_clock::time_point (may have different tick precision)
    const auto as_system_dur =
        duration_cast<system_clock::duration>(utc_tp_us.time_since_epoch());

    return system_clock::time_point{as_system_dur};
  }

  [[nodiscard]] static std::size_t
  size_buffer(const std::chrono::system_clock::time_point &value) noexcept {

    // return size of the format +1 for null terminator
    return sizeof("0000-00-00 00:00:00.000000+0000") + 1;
  }
};
} // namespace pqxx
