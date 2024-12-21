import pytest
import json


@pytest.mark.parametrize("title, description, performer_id, priority, queue_id, status_id, deadline_date, response_status", [
    ("New task", "Urgent work to do", 2, "HIGH", 1, 1, "2024-12-08T13:43:09.264373+00:00", 200),
    ("New task", None, 2, "HIGH", 1, 1, "2024-12-08T13:43:09.264373+00:00", 200),
    ("New task", "Urgent work to do", None , "HIGH", 1, 1, "2024-12-08T13:43:09.264373+00:00", 200),
    ("New task", "Urgent work to do", 2, "HIGH", 1, 1, None, 200),
    ("New task", "Urgent work to do", None, "HIGH", 1, 1, None, 200),
    ("New task", None, 2, "HIGH", 1, 1, None, 200),
    ("New task", None, None, "HIGH", 1, 1, "2024-12-08T13:43:09.264373+00:00", 200),
    ("New task", None, None, "HIGH", 1, 1, None, 200),
    (None, None, None, None, None, None, None, 400),
    (None, "Urgent work to do", 2, "HIGH", 1, 1, "2024-12-08T13:43:09.264373+00:00", 400),
    ("New task", "Urgent work to do", 2, "HIGH", None, 1, "2024-12-08T13:43:09.264373+00:00", 400),
    ("New task", "Urgent work to do", 2, "HIGH", 1, None, "2024-12-08T13:43:09.264373+00:00", 400),
    ("New task", "Urgent work to do", 2, "HIGH", 2, None, "2024-12-08T13:43:09.264373+00:00", 400),
    ("New task", "Urgent work to do", 2, "HIGH", 2, 5, "2024-12-08T13:43:09.264373+00:00", 403),
    ("New task", "Urgent work to do", 2, "HIGH", 4, 1, "2024-12-08T13:43:09.264373+00:00", 404),
    ("New task", "Urgent work to do", 2, "HIGH", 4, 1, "2024-12-08T13:43:09.264373+00:00", 404),
])

@pytest.mark.pgsql('V001__create_tables', files=['V002__fill_tables.sql'])
async def test_get_comment(service_client, login_session_id, title, description, performer_id, priority, 
                           queue_id, status_id, deadline_date, response_status):
    response = await service_client.post(f'/v1/tickets/', headers = {'X-Ya-User-Ticket': login_session_id},
            data = json.dumps({
                'title': title,
                'description': description,
                'performer_id': performer_id,
                'priority': priority, 
                'queue_id' : queue_id,
                'status_id': status_id,
                'deadline_date': deadline_date,
            }))
    assert response.status == response_status
    if response_status == 200:
        response_json = response.json()
        assert "id" in response_json
        assert response_json["id"] is not None