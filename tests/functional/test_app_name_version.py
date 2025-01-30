from application_client.solar_command_sender import SolarCommandSender
from application_client.solar_response_unpacker import (
    unpack_get_app_and_version_response,
)


# Verify that BOLOS (not the app) replies the name and version of the Solar app.
def test_get_app_name_and_version(backend):
    # Use the app interface instead of raw interface
    client = SolarCommandSender(backend)
    # Send the special instruction to BOLOS
    response = client.get_app_and_version()
    # Use an helper to parse the response, assert the values
    app_name, version = unpack_get_app_and_version_response(response.data)

    assert app_name == "Solar"
    assert version == "1.1.3"
