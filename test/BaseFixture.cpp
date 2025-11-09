//
// Created by Tom Kirk on 08/11/2025.
//

#include "BaseFixture.h"

namespace {
constexpr auto PGCONN_ENV_VAR = "POSTGRES_CONN";
}

void BaseFixture::SetUp() {
  const auto connString = std::getenv(PGCONN_ENV_VAR);
  if (connString == nullptr) {
    GTEST_SKIP()
        << "Skipping test since no database connection string is set in "
        << PGCONN_ENV_VAR << " environment variable.";
  }

  conn = std::make_unique<pqxx::connection>(connString);

  tx = std::make_unique<pqxx::work>(*conn);
}

void BaseFixture::TearDown() {
  if (tx) {
    // Don't commit to DB
    tx->abort();
  }
}
