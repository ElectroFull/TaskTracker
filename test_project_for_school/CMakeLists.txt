cmake_minimum_required(VERSION 3.12)
project(task-tracker CXX)

# Adding userver dependency
find_package(userver COMPONENTS core postgresql QUIET)
if(NOT userver_FOUND)  # Fallback to subdirectory usage
	# Enable userver libraries that are needed in this project
	set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)

	# Compatibility mode: some systems don't support these features
	set(USERVER_FEATURE_CRYPTOPP_BLAKE2 OFF CACHE BOOL "" FORCE)
	set(USERVER_FEATURE_GRPC_CHANNELZ OFF CACHE BOOL "" FORCE)
	set(USERVER_FEATURE_REDIS_HI_MALLOC ON CACHE BOOL "" FORCE)
	
	if (EXISTS third_party/userver)
		message(STATUS "Using userver framework from third_party/userver")
		add_subdirectory(third_party/userver)
	else()
		message(FATAL_ERROR "Either install the userver or provide a path to it")
	endif()
endif()

userver_setup_environment()


# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
	src/models/session.hpp
	src/models/user.hpp
	src/models/user.ipp
	src/models/user.cpp
	src/models/queue.cpp
	src/models/queue.hpp
	src/models/queue.ipp
    src/helpers/helpers.hpp
    src/helpers/helpers.cpp
	src/handlers/lib/auth.hpp
	src/handlers/lib/auth.cpp
	src/models/ticket_from_user.hpp
	src/models/ticket_from_user.cpp
	src/models/ticket.hpp
	src/models/ticket.ipp
	src/models/ticket.cpp
	src/queries/ticket_queries.hpp
	src/queries/ticket_queries.cpp
	src/handlers/v1/tickets/add-ticket/view.hpp
	src/handlers/v1/tickets/add-ticket/view.cpp
	src/handlers/v1/tickets/get-ticket/view.cpp
	src/handlers/v1/tickets/get-ticket/view.hpp
	src/handlers/v1/tickets/update-ticket/view.cpp
	src/handlers/v1/tickets/update-ticket/view.hpp
	src/handlers/v1/tickets/get-tickets-from-queue/view.cpp
	src/handlers/v1/tickets/get-tickets-from-queue/view.hpp
	src/handlers/v1/register/view.hpp
	src/handlers/v1/register/view.cpp
	src/handlers/v1/login/view.hpp
	src/handlers/v1/login/view.cpp
	src/handlers/v1/unregister/view.hpp
	src/handlers/v1/unregister/view.cpp
    src/handlers/v1/queues/add-queue/view.hpp
	src/handlers/v1/queues/add-queue/view.cpp
	src/handlers/v1/queues/get-queues/view.hpp
	src/handlers/v1/queues/get-queues/view.cpp
	src/handlers/v1/queues/get-queue/view.hpp
	src/handlers/v1/queues/get-queue/view.cpp
	src/handlers/v1/queues/update-queue/view.hpp
	src/handlers/v1/queues/update-queue/view.cpp
	src/handlers/v1/queues/delete-queue/view.hpp
	src/handlers/v1/queues/delete-queue/view.cpp
	src/handlers/v1/users-queues/add-user-to-queue/view.hpp
	src/handlers/v1/users-queues/add-user-to-queue/view.cpp
	src/handlers/v1/users-queues/delete-user-from-queue/view.hpp
	src/handlers/v1/users-queues/delete-user-from-queue/view.cpp
    src/handlers/v1/users-queues/get-users-from-queue/view.hpp
	src/handlers/v1/users-queues/get-users-from-queue/view.cpp
	src/handlers/v1/comments/get-comment/view.hpp
	src/handlers/v1/comments/get-comment/view.cpp
	src/handlers/v1/comments/update-comment/view.hpp
	src/handlers/v1/comments/update-comment/view.cpp
	src/handlers/v1/comments/delete-comment/view.hpp
	src/handlers/v1/comments/delete-comment/view.cpp
	src/queries/queues_queries.hpp
	src/queries/queues_queries.cpp
    src/queries/user_queries.hpp
	src/queries/user_queries.cpp
	src/queries/comments_queries.hpp
	src/queries/comments_queries.cpp
	src/queries/statuses.hpp
	src/queries/statuses.cpp
	src/models/comment.ipp
	src/models/comment.hpp
	src/models/comment.cpp
	src/models/user_queue.hpp
	src/models/user_queue.cpp
	src/models/user_ticket.hpp
	src/models/user_ticket.cpp
	src/handlers/v1/comments/delete-comment/view.hpp
	src/handlers/v1/comments/delete-comment/view.cpp
	src/handlers/v1/comments/get-ticket-comments/view.hpp
	src/handlers/v1/comments/get-ticket-comments/view.cpp
	src/handlers/v1/comments/add-comment/view.hpp
	src/handlers/v1/comments/add-comment/view.cpp
	src/handlers/v1/watchers/get-users-by-ticket/view.hpp
	src/handlers/v1/watchers/get-users-by-ticket/view.cpp
	src/handlers/v1/watchers/add-user-to-ticket/view.hpp
	src/handlers/v1/watchers/add-user-to-ticket/view.cpp
	src/handlers/v1/watchers/delete-user-from-ticket/view.hpp
	src/handlers/v1/watchers/delete-user-from-ticket/view.cpp
	src/handlers/v1/users/get-users/view.hpp
	src/handlers/v1/users/get-users/view.cpp
	src/handlers/v1/users/get-user-by-login/view.hpp
	src/handlers/v1/users/get-user-by-login/view.cpp
)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Unit Tests
# add_executable(${PROJECT_NAME}_unittest
# 	unittests/basic.cpp
# )
# target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
# add_google_tests(${PROJECT_NAME}_unittest)


# # Benchmarks
# add_executable(${PROJECT_NAME}_benchmark
# )
# target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
# add_google_benchmark_tests(${PROJECT_NAME}_benchmark)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/third_party/userver/cmake")

# Functional Tests
include(UserverTestsuite)

userver_testsuite_add_simple()

# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
