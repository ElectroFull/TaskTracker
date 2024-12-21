import pytest

async def test_session_id_fixture(service_client, session_id):
    assert session_id is not None
