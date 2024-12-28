import pytest

from test_register import auth_user_action, valid_email, valid_password

# Start the tests via `make test-debug` or `make test-release`



@pytest.mark.parametrize("login, password, response_status", [
    (valid_email, valid_password, 200),
    ('abc' + valid_email, valid_password, 404),
    (None, valid_password, 400),
    (valid_email, None, 400),
])
async def test_login(service_client, login, password, response_status):
    if response_status != 404 and login == valid_email and password == valid_password:
        await auth_user_action(service_client, '/v1/register', valid_email, valid_password, 200)
    
    await auth_user_action(service_client, '/v1/login', login, password, response_status)
