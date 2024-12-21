import pytest



@pytest.mark.parametrize("id, response_status", [
    (1, 200),
    (2, 200),
    (12, 404),
    (-1, 404),
    ("abcd", 400),
    ("", 400),
])

@pytest.mark.pgsql('V001__create_tables', files=['V002__fill_tables.sql'])
async def test_get_comment(service_client, session_id, id, response_status):
    response = await service_client.get(f'/v1/comments/{id}', headers = {'X-Ya-User-Ticket': session_id})

    assert response.status == response_status
    
    if response.status == 200:
        response_json = response.json()
        assert "message" in response_json
        assert "id" in response_json
        assert response_json["id"] == id
        assert "created_date" in response_json
