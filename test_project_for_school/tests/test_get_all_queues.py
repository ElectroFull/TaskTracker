import pytest

# Start the tests via `make test-debug` or `make test-release`
@pytest.mark.pgsql('V001__create_tables', files=['V002__fill_tables.sql'])
async def test_get_all_queues(service_client, login_session_id):
    headers = {
        'X-Ya-User-Ticket': login_session_id
    }

    response = await service_client.get(
        '/v1/queues',
        headers=headers
    )
    assert response.status == 200

    response_data = response.json()
    assert isinstance(response_data['items'], list)
    assert len(response_data['items']) > 0

    item = response_data['items'][0]
    assert item is not None
    assert item['id'] is not None
    assert item['author_id'] is not None
    assert item['tickets_count'] is not None
    assert item['tickets_count'] >=0
    assert item['created_date'] is not None