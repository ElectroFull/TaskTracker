import pytest

@pytest.mark.pgsql('V001__create_tables', files=['V002__fill_tables.sql'])
@pytest.mark.parametrize("id, response_status", [
    (1, 200),
    (2, 200),
    (12, 200),
    (-1, 200),
    ("abcd", 400),
    ("", 400),
])
async def test_delete_comment(service_client, session_id, id, response_status):
    response = await service_client.delete(f'/v1/comments/{id}', headers = {'X-Ya-User-Ticket': session_id})

    assert response.status == response_status
    
    if response.status == 200:
        response_json = response.json()
        assert "id" in response_json
        response = await service_client.get(f'/v1/comments/{id}', headers = {'X-Ya-User-Ticket': session_id})
        assert response.status == 404
