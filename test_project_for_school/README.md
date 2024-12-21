# Task Tracker Bot

A **task tracker** with a simple interface in a **Telegram bot**. The service is built using the **Userver** asynchronous framework, ensuring high performance and scalability.

## Features

- **User Registration and Login**
- **Task Management**: Create, edit, and delete tasks
- **Queue Management**: Manage task queues and assign users
- **Task Updates**: Assign performer, change ticket and add comments
- **Simple Interface**: Interact through a Telegram bot

## Installation

Follow these steps to set up and run the project:

```bash
git clone https://github.com/stanislavmg/task-tracker.git
cd task-tracker
git submodule update --init --recursive
make install-release
# if you want to install service in docker
make docker-install-release
```

## Running the Service

To launch the task tracker service using Docker:

```bash
make docker-start-release
```

For a local launch:

```bash
make start-release
```

## Running the Telegram Bot

```bash
make docker-start-bot
```

## Testing the Service

```bash
make test-release
# same command using docker
make docker-test-release
```
