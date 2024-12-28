DROP SCHEMA IF EXISTS tracker CASCADE;

CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE SCHEMA IF NOT EXISTS tracker;

CREATE TABLE IF NOT EXISTS tracker.users (
    id SERIAL PRIMARY KEY,
    login text not NULL,
    password TEXT NOT NULL,
    created_date TIMESTAMP NOT NULL,
    deleted_date TIMESTAMP
);

CREATE TABLE IF NOT EXISTS tracker.queues (
    id SERIAL PRIMARY KEY,
    name TEXT NOT NULL UNIQUE,
    author_id integer NOT NULL REFERENCES tracker.users (id),
    tickets_count integer NOT NULL,
    created_date TIMESTAMP NOT NULL,
    deleted_date TIMESTAMP
);

CREATE TABLE IF NOT EXISTS tracker.users_queues (
    user_id integer NOT NULL REFERENCES tracker.users (id),
    queue_id integer NOT NULL REFERENCES tracker.queues (id),
    PRIMARY KEY (user_id, queue_id)
);

CREATE TABLE IF NOT EXISTS tracker.ticket_statuses (
    id SERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    deleted_date TIMESTAMP
);

DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'priority_type') THEN
        CREATE TYPE tracker.priority_type AS ENUM ('LOW', 'MEDIUM', 'HIGH');
    END IF;
END $$;

CREATE TABLE IF NOT EXISTS tracker.tickets (
    id TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    description TEXT,
    author_id integer NOT NULL REFERENCES tracker.users (id),
    performer_id integer REFERENCES tracker.users (id),
    priority tracker.priority_type NOT NULL ,
    queue_id integer NOT NULL REFERENCES tracker.queues (id),
    status_id INTEGER NOT NULL REFERENCES tracker.ticket_statuses (id),
    start_date TIMESTAMP,
    finish_date TIMESTAMP,
    deadline_date TIMESTAMP,
    created_date TIMESTAMP NOT NULL,
    deleted_date TIMESTAMP
    
);

CREATE table if not exists tracker.watchers (
    user_id integer NOT NULL REFERENCES tracker.users (id),
    ticket_id TEXT NOT NULL REFERENCES tracker.tickets (id),
    PRIMARY KEY (user_id, ticket_id)
);

DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'relation_type') THEN
        CREATE TYPE tracker.relation_type AS ENUM ('PARENT', 'CHILD', 'BLOCKED_BY', 'LINKED_WITH');
    END IF;
END $$;

CREATE table if not exists tracker.relations (
    from_id TEXT NOT NULL REFERENCES tracker.tickets (id),
    to_id TEXT NOT NULL REFERENCES tracker.tickets (id),
    type tracker.relation_type,
    PRIMARY KEY (from_id, to_id)
);

CREATE table if not EXISTS tracker.comments (
    id SERIAL PRIMARY KEY,
    message TEXT NOT NULL,
    ticket_id TEXT NOT NULL REFERENCES tracker.tickets (id),
    created_date TIMESTAMP NOT NULL,
    deleted_date TIMESTAMP
);


CREATE table if not exists tracker.history (
    id SERIAL PRIMARY KEY,
    author_id integer NOT NULL REFERENCES tracker.users (id),
    ticket_id TEXT NOT NULL REFERENCES tracker.tickets (id),
    old_value TEXT,
    new_value TEXT,
    created_date TIMESTAMP NOT NULL,
    deleted_date TIMESTAMP
);

CREATE TABLE IF NOT EXISTS tracker.auth_sessions (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id integer NOT NULL,
    foreign key(user_id) REFERENCES tracker.users(id)
);


