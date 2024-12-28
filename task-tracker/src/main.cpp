#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "handlers/v1/register/view.hpp"
#include "handlers/v1/login/view.hpp"
#include "handlers/v1/unregister/view.hpp"
#include "handlers/v1/queues/add-queue/view.hpp"
#include "handlers/v1/queues/get-queues/view.hpp"
#include "handlers/v1/queues/get-queue/view.hpp"
#include "handlers/v1/queues/update-queue/view.hpp"
#include "handlers/v1/queues/delete-queue/view.hpp"
#include "handlers/v1/users-queues/add-user-to-queue/view.hpp"
#include "handlers/v1/users-queues/delete-user-from-queue/view.hpp"
#include "handlers/v1/users-queues/get-users-from-queue/view.hpp"
#include "handlers/v1/comments/get-comment/view.hpp"
#include "handlers/v1/comments/delete-comment/view.hpp"
#include "handlers/v1/comments/update-comment/view.hpp"
#include "handlers/v1/comments/get-ticket-comments/view.hpp"
#include "handlers/v1/comments/add-comment/view.hpp"
#include "handlers/v1/watchers/get-users-by-ticket/view.hpp"
#include "handlers/v1/watchers/add-user-to-ticket/view.hpp"
#include "handlers/v1/watchers/delete-user-from-ticket/view.hpp"
#include "handlers/v1/tickets/add-ticket/view.hpp"
#include "handlers/v1/tickets/get-ticket/view.hpp"
#include "handlers/v1/tickets/update-ticket/view.hpp"
#include "handlers/v1/tickets/get-tickets-from-queue/view.hpp"
#include "handlers/v1/users/get-users/view.hpp"
#include "handlers/v1/users/get-user-by-login/view.hpp"

int main(int argc, char* argv[]) {
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::components::HttpClient>()
          .Append<userver::server::handlers::TestsControl>()
          .Append<userver::components::Postgres>("postgres-db-1")
          .Append<userver::clients::dns::Component>();

  tracker::AppendRegisterUser(component_list);
  tracker::AppendLoginUser(component_list);  
  tracker::AppendUnregisterUser(component_list);
  tracker::AppendAddQueue(component_list);
  tracker::AppendGetQueues(component_list);
  tracker::AppendGetQueue(component_list);
  tracker::AppendUpdateQueue(component_list);
  tracker::AppendDeleteQueue(component_list);
  tracker::AppendAddUserToQueue(component_list);
  tracker::AppendDeleteUserFromQueue(component_list);
  tracker::AppendGetUsersFromQueue(component_list);
  tracker::AppendGetComment(component_list);
  tracker::AppendDeleteComment(component_list);
  tracker::AppendUpdateComment(component_list);
  tracker::AppendCreateTicket(component_list);  
  tracker::AppendGetTicketComments(component_list);
  tracker::AppendCreateComment(component_list);
  tracker::AppendGetUsersFromTicket(component_list);
  tracker::AppendAddUserToTicket(component_list);
  tracker::AppendDeleteUserFromTicket(component_list);
  tracker::AppendGetTicket(component_list);
  tracker::AppendUpdateTicket(component_list);
  tracker::AppendGetTicketsFromQueue(component_list);
  tracker::AppendGetUsers(component_list);
  tracker::AppendGetUserByLogin(component_list);
  return userver::utils::DaemonMain(argc, argv, component_list);
}
