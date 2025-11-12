#include <gtest/gtest.h>
#include <pqxx_ext/SystemClock.h>

TEST(SystemClockTests, ConvertsFromString) {

  using namespace std::chrono;

  constexpr auto dayPoint = sys_days(year_month_day(year(2000), March, day(5)));
  const sys_time<microseconds> target =
      dayPoint + hours{10} + minutes{25} + seconds{10} + microseconds{123456};

  const auto text = std::format("2000-03-05 10:25:10.123456{:%z}", target);

  const auto result =
      pqxx::string_traits<system_clock::time_point>::from_string(text);

  EXPECT_EQ(result, target);
}

TEST(SystemClockTests, ConvertsToString) {

  using time_point = std::chrono::system_clock::time_point;
  constexpr time_point epoch;

  const std::string expectedValue = "1970-01-01 00:00:00.000000+0000";

  const auto sizeBuffer = pqxx::size_buffer(epoch);

  std::string buf;
  buf.resize(sizeBuffer);

  const auto ptr = pqxx::string_traits<time_point>::into_buf(
      buf.data(), buf.data() + sizeBuffer, epoch);

  // Last character should be null
  // EXPECT_EQ(*ptr, '\0');

  // Trim the null for comparison
  const auto result = std::string(buf.data(), ptr - 1);

  EXPECT_EQ(result, expectedValue);
}

TEST(SystemClockTests, FailTest) {
  EXPECT_TRUE(false);

  EXPECT_FALSE(true);
}
