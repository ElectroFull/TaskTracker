#pragma once
#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace tracker 
{

userver::storages::postgres::ResultSet CreateStatus(userver::storages::postgres::ClusterPtr _pg_cluster, int name);
userver::storages::postgres::ResultSet GetDefaultStatus(userver::storages::postgres::ClusterPtr _pg_cluster);
userver::storages::postgres::ResultSet GetStatus(userver::storages::postgres::ClusterPtr _pg_cluster, int id);
userver::storages::postgres::ResultSet DeleteStatus(userver::storages::postgres::ClusterPtr _pg_cluster, int id);

} // namespace tracker