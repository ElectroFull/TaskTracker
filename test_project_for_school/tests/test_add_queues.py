import pytest

# Start the tests via `make test-debug` or `make test-release`

@pytest.mark.parametrize("name, status", [
    ('new queue_1', 200),
    ('new queue_2', 200),
    (None, 400),
])
@pytest.mark.pgsql('V001__create_tables', files=['V002__fill_tables.sql'])
async def test_add_queue(service_client, session_id, name, status):
    data = {
        
        'name': name
    }
    headers = {
        'Content-Type': 'application/json',
        'X-Ya-User-Ticket': session_id 
    }
    response = await service_client.post(
        '/v1/queues',
        json=data,
        headers=headers
    )
    assert response.status == status
    if response.status != 200:
        return
    response_data = response.json()
    assert response_data['name'] == name
    assert response_data['author_id'] is not None


@pytest.mark.parametrize("name, status", [
    ('new queue_1', 401),
    ('new queue_2', 401),
])
@pytest.mark.pgsql('V001__create_tables', files=['V002__fill_tables.sql'])
async def test_add_queue_bad_session(service_client, name, status):
    data = {
        'name': name
    }
    headers = {
        'Content-Type': 'application/json',
        'X-Ya-User-Ticket': '' 
    }
    response = await service_client.post(
        '/v1/queues',
        json=data,
        headers=headers
    )
    assert response.status == status
    
@pytest.mark.pgsql('V001__create_tables', files=['V002__fill_tables.sql'])
async def test_add_queue_no_name(service_client, session_id):
    data = {}
    headers = {
        'Content-Type': 'application/json',
        'X-Ya-User-Ticket': session_id 
    }
    response = await service_client.post(
        '/v1/queues',
        json=data,
        headers=headers
    )
    assert response.status == 400