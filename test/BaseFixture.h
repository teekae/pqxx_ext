#pragma once

#include <gtest/gtest.h>
#include <pqxx/pqxx>

class BaseFixture : public testing::Test {
protected:
  void SetUp() override;
  void TearDown() override;

  std::unique_ptr<pqxx::connection> conn;
  std::unique_ptr<pqxx::work> tx;
};
