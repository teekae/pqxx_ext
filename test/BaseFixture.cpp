//
// Created by Tom Kirk on 08/11/2025.
//

#include "BaseFixture.h"

pqxx::connection BaseFixture::CreateConnection() {
  const auto connString = std::getenv("POSTGRES_CONN");

  return pqxx::connection{connString};
}
