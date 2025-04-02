from application_client.solar_command_sender import SolarCommandSender
from application_client.solar_response_unpacker import unpack_get_version_response


# Taken from the Makefile, to update every time the Makefile version is bumped
MAJOR = 1
MINOR = 2
PATCH = 0


# Verify the behaviour when getting the Solar app's version.
def test_get_app_version(backend):
    # Use the app interface instead of raw interface
    client = SolarCommandSender(backend)
    # Send the GET_VERSION instruction
    rapdu = client.get_version()
    # Use an helper to parse the response, assert the values
    assert unpack_get_version_response(rapdu.data) == (MAJOR, MINOR, PATCH)
