import pytest
import json



@pytest.mark.parametrize("id, response_status, message", [
    (1, 200, "privet1"),
    (2, 200, "provet2"),
    (3, 200, "spookyghost"),
    (12, 404, "damn"),
    (-1, 404, "damn"),
    ("abcd", 400, "damn"),
    ("", 400, "damn"),
])

@pytest.mark.pgsql('V001__create_tables', files=['V002__fill_tables.sql'])
async def test_update_comment(service_client, session_id, id, response_status, message):
    response = await service_client.put(f'/v1/comments/{id}',
    headers={
        'X-Ya-User-Ticket': session_id,
        'Content-Type': 'application/json'
    },
    data = json.dumps({'message': message}))

    assert response.status == response_status
    if response.status == 200:
        response_json = response.json()
        assert "message" in response_json
        assert response_json["message"] == message
        assert "id" in response_json
        assert response_json["id"] == id
        assert "created_date" in response_json
