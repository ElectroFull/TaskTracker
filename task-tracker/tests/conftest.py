import pathlib
import aiohttp
import pytest

from testsuite.databases.pgsql import discover
from testsuite.databases import pgsql

import sys
import os

sys.path.append(os.path.dirname(os.path.abspath(__file__)))


pytest_plugins = ['pytest_userver.plugins.postgresql']


@pytest.fixture(scope='session')
def service_source_dir():
    """Path to root directory service."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Path for find files with data"""
    return [
        service_source_dir / 'postgresql/data',
    ]

@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'task_tracker',  # service name that goes to the DB connection
        [service_source_dir.joinpath('postgresql/schema')],    
    )
    return pgsql_local_create(list(databases.values()))

#'V002_fill_tables.sql'

def create_multipart_data(login: str, password: str) -> aiohttp.MultipartWriter:
    with aiohttp.MultipartWriter('form-data') as data:
        if login is not None:
            payload = aiohttp.payload.StringPayload(login)
            payload.set_content_disposition('form-data', name='login')
            data.append_payload(payload)
        if password is not None:
            payload = aiohttp.payload.StringPayload(password)
            payload.set_content_disposition('form-data', name='password')
            data.append_payload(payload)
    return data

@pytest.fixture(scope='function')
async def session_id(service_client):
    login = 'non_existant_user12345@mail.dev'
    password = 'password'
    data = create_multipart_data(login, password)
    assert data is not None
    headers = {
        'Content-Type': 'multipart/form-data; boundary=' + data.boundary
    }
    response = await service_client.post(
        '/v1/register',
        data=data,
        headers=headers
    )
    
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 1
    assert response_json["id"] is not None


    response = await service_client.post(
        '/v1/login',
        data=data,
        headers=headers
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 1
    assert len(response_json["id"]) > 0
    return response_json["id"]



@pytest.fixture(scope='function')
async def login_session_id(service_client):
    login = 'one'
    password = '1'
    data = create_multipart_data(login, password)
    assert data is not None
    headers = {
        'Content-Type': 'multipart/form-data; boundary=' + data.boundary
    }

    response = await service_client.post(
        '/v1/login',
        data=data,
        headers=headers
    )
    assert response.status == 200
    response_json = response.json()
    assert len(response_json) == 1
    assert len(response_json["id"]) > 0
    return response_json["id"]