INSERT INTO tracker.users (login, password, created_date)
VALUES
    ('one', '6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b', CURRENT_TIMESTAMP),
    ('two', '6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b', CURRENT_TIMESTAMP),
    ('three', '6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b', CURRENT_TIMESTAMP),
   	('four', '6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b', CURRENT_TIMESTAMP),
    ('five', '6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b', CURRENT_TIMESTAMP),
    ('six', '6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b', CURRENT_TIMESTAMP),
   	('seven', '6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b', CURRENT_TIMESTAMP),
  	('eight', '6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b', CURRENT_TIMESTAMP),
 	('nine', '6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b', CURRENT_TIMESTAMP);
 
   
INSERT INTO tracker.queues (name, author_id, tickets_count, created_date)
VALUES
    ('queue1', 1, 2, CURRENT_TIMESTAMP),
    ('queue2', 2, 2, CURRENT_TIMESTAMP),
    ('queue3', 3, 2, CURRENT_TIMESTAMP);

INSERT INTO tracker.users_queues (user_id, queue_id)
VALUES
    (1, 1),
    (2, 2),
    (3, 3),
    (2, 3),
    (2, 1);

INSERT INTO tracker.ticket_statuses (name, deleted_date)
VALUES
    ('Open', NULL),
    ('In Progress', NULL),
    ('Resolved', NULL);

   
INSERT INTO tracker.tickets (id, title, description, author_id, performer_id, priority, queue_id, status_id, start_date, finish_date, deadline_date, created_date)
VALUES
    ('queue1_1', 'Title 1 queue 1', 'Description 1 queue 1', 1, 2, 'HIGH', 1, 1, CURRENT_TIMESTAMP, NULL, '2024-12-15', CURRENT_TIMESTAMP),
    ('queue1_2', 'Title 2 queue 1', 'Description 2 queue 1', 2, 3, 'MEDIUM', 1, 1, CURRENT_TIMESTAMP, NULL, '2024-12-20', CURRENT_TIMESTAMP),
   	('queue2_1', 'Title 1 queue 2', 'Description 1 queue 2', 3, 2, 'HIGH', 1, 1, CURRENT_TIMESTAMP, NULL, '2024-12-15', CURRENT_TIMESTAMP),
    ('queue2_2', 'Title 2 queue 2', 'Description 2 queue 2', 4, 3, 'MEDIUM', 1, 1, CURRENT_TIMESTAMP, NULL, '2024-12-20', CURRENT_TIMESTAMP),
   	('queue3_1', 'Title 1 queue 3', 'Description 1 queue 3', 1, 2, 'HIGH', 1, 1, CURRENT_TIMESTAMP, NULL, '2024-12-15', CURRENT_TIMESTAMP),
    ('queue3_2', 'Title 2 queue 3', 'Description 2 queue 3', 2, 3, 'MEDIUM', 1, 1, CURRENT_TIMESTAMP, NULL, '2024-12-20', CURRENT_TIMESTAMP);
  
INSERT INTO tracker.watchers (user_id, ticket_id)
VALUES
    (5, 'queue1_1'),
    (5, 'queue2_1'),
    (6, 'queue1_1'),
    (6, 'queue2_1'),
    (7, 'queue1_2'),
   	(7, 'queue2_2');
  
INSERT INTO tracker.relations (from_id, to_id, type)
VALUES
    ('queue1_1', 'queue1_2', 'BLOCKED_BY'),
   	('queue2_1', 'queue2_2', 'LINKED_WITH');


INSERT INTO tracker.comments (message, ticket_id, created_date)
VALUES
    ('Comment 1', 'queue1_1', CURRENT_TIMESTAMP),
    ('Comment 2', 'queue1_1', CURRENT_TIMESTAMP),
   	('Comment 1', 'queue1_2', CURRENT_TIMESTAMP),
    ('Comment 2', 'queue1_2', CURRENT_TIMESTAMP),
   	('Comment 1', 'queue2_1', CURRENT_TIMESTAMP),
    ('Comment 2', 'queue2_1', CURRENT_TIMESTAMP),
   	('Comment 1', 'queue2_2', CURRENT_TIMESTAMP),
    ('Comment 2', 'queue2_2', CURRENT_TIMESTAMP);
   
  
INSERT INTO tracker.history (author_id, ticket_id, old_value, new_value, created_date)
VALUES
    (1, 'queue1_1', 'old_value', 'new_value', CURRENT_TIMESTAMP),
    (2, 'queue1_2', 'old_value', 'new_value', CURRENT_TIMESTAMP);


