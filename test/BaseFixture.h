#pragma once

#include <gtest/gtest.h>
#include <pqxx/pqxx>

class BaseFixture : public testing::Test {
protected:
  pqxx::connection CreateConnection();
};
