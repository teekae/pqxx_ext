
#include "BaseFixture.h"
#include <pqxx_ext/Vector.h>

#include <gtest/gtest.h>
#include <pqxx/pqxx>

TEST(VectorTests, SizesCorrectly) {

  std::vector values = {1, 2, 3};

  const auto totalItems = values.size();

  size_t expectedSizeBuffer = 2; // 2 for brackets
  for (const auto &value : values) {
    expectedSizeBuffer += pqxx::size_buffer(value);
  }

  // a comma per value
  expectedSizeBuffer += totalItems - 1;

  // null terminator
  expectedSizeBuffer += 1;

  const auto sizeBuffer = pqxx::size_buffer(values);

  EXPECT_EQ(sizeBuffer, expectedSizeBuffer);
}

TEST(VectorTests, ConvertsFromString) {

  std::vector expectedValues = {1, 4, 9};

  const auto result =
      pqxx::string_traits<std::vector<int>>::from_string("{1,4,9}");

  EXPECT_EQ(result, expectedValues);
}

TEST(VectorTests, ConvertsToString) {
  std::vector values = {1, 2, 3};

  const std::string expectedValue = "{1,2,3}";

  const auto sizeBuffer = pqxx::size_buffer(values);

  std::string buf;
  buf.resize(sizeBuffer);

  const auto ptr = pqxx::string_traits<std::vector<int>>::into_buf(
      buf.data(), buf.data() + sizeBuffer, values);

  // Last character should be null
  EXPECT_EQ(*ptr, '\0');

  // Trim the null for comparison
  const auto result = std::string(buf.data(), ptr - 1);

  EXPECT_EQ(result, expectedValue);
}

class VectorDBTests : public BaseFixture {};

TEST_F(VectorDBTests, IntRoundTripsCorrectly) {
  const std::vector expectedVector = {1, 2, 4, 6};

  tx->exec("CREATE TABLE vector ("
           "id SERIAL PRIMARY KEY,"
           "values integer[]);");

  tx->exec("INSERT INTO vector (values) VALUES ($1);",
           pqxx::params{expectedVector});

  const auto row = tx->exec("SELECT values FROM vector;").one_row();

  std::tuple<std::vector<int>> result;

  row.to(result);

  EXPECT_EQ(std::get<0>(result), expectedVector);
}

TEST_F(VectorDBTests, StringRoundTripsCorrectly) {
  const std::vector<std::string> expectedVector = {"Hello", "PQXX", "Vector",
                                                   "Test"};

  tx->exec("CREATE TABLE vector ("
           "id SERIAL PRIMARY KEY,"
           "values text[]);");

  tx->exec("INSERT INTO vector (values) VALUES ($1);",
           pqxx::params{expectedVector});

  const auto row = tx->exec("SELECT values FROM vector;").one_row();

  std::tuple<std::vector<std::string>> result;

  row.to(result);

  EXPECT_EQ(std::get<0>(result), expectedVector);
}

TEST(VectorTests, FailToSee) { EXPECT_EQ("Hello", "World"); }

TEST(VectorTests, SkipTest) {
  GTEST_SKIP() << "Skipping this test for demonstration purposes.";
}