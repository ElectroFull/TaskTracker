import aiohttp

import pytest


valid_email = 'test@yandex.ru'
valid_password = 'pass123'


async def auth_user_action(service_client, action, login, password, response_status):
    with aiohttp.MultipartWriter('form-data') as data:
        if login is not None:
            login_payload = aiohttp.payload.StringPayload(login)
            login_payload.set_content_disposition('form-data', name='login')
            data.append_payload(login_payload)

        if password is not None:
            password_payload = aiohttp.payload.StringPayload(password)
            password_payload.set_content_disposition('form-data', name='password')
            data.append_payload(password_payload)

    headers = {
        'Content-Type': 'multipart/form-data; boundary=' + data.boundary
    }

    response = await service_client.post(
        action,
        data=data,
        headers=headers
    )

    assert response.status == response_status

    if response.status == 200:
        response_json = response.json()

        assert len(response_json) == 1
        assert response_json["id"] is not None

    return response


@pytest.mark.parametrize("login, password, response_status", [
    (valid_email, valid_password, 200),
    (None, valid_password, 400),
    (valid_email, None, 400),
    ('test', valid_password, 400),
    ('test@mail', valid_password, 400),
    ('testmail.ru', valid_password, 400),
    ('test@mail.ru-', valid_password, 400),
    ('te..st@mail.ru', valid_password, 400),
    ('te@st@mail.ru', valid_password, 400),
])
async def test_register(service_client, login, password, response_status):
    await auth_user_action(service_client, '/v1/register', login, password, response_status)


async def test_register_with_existing_user(service_client):
    await auth_user_action(service_client, '/v1/register', valid_email, valid_password, 200)
    await auth_user_action(service_client, '/v1/register', valid_email, valid_password, 409)

