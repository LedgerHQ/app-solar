import pytest

from application_client.solar_command_sender import SolarCommandSender, Errors
from application_client.solar_response_unpacker import unpack_get_address_response
from ragger.error import ExceptionRAPDU
# from ragger.navigator import NavInsID, NavIns
# from utils import ROOT_SCREENSHOT_PATH


TESTNET: int = 0xE0
MAINNET: int = 0x3F

def test_get_address_no_confirm(backend):
    client = SolarCommandSender(backend)
    path = "m/44'/3333'/0'/0/0"
    networks = [TESTNET, MAINNET]

    for network in networks:
        response = client.get_address(path=path, network=network).data

        address_len, address = unpack_get_address_response(response)

        assert address_len == 34
        assert address[0] == (ord("D") if network == TESTNET else ord("S"))

def test_get_address_unsupported_network(backend):
    client = SolarCommandSender(backend)
    path = "m/44'/3333'/0'/0/0"
    unsupported_network = 0x3E

    with pytest.raises(ExceptionRAPDU) as e:
        client.get_address(path=path, network=unsupported_network).data
    assert e.value.status == Errors.SW_INS_NOT_SUPPORTED

# def test_get_address_confirm(backend):
#     client = SolarCommandSender(backend)
#     path = "m/44'/3333'/0'/0/0"
#     networks = [TESTNET, MAINNET]
#
#     for network in networks:
#         with client.get_address(path=path, display=1, network=network):
#             navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
#                                                     [NavInsID.BOTH_CLICK],
#                                                     "Approve",
#                                                     ROOT_SCREENSHOT_PATH,
#                                                     test_name)
#         response = client.get_async_response().data
#         address_len, address = unpack_get_address_response(response)
#
#         assert address_len == 34
#         assert address[0] == (ord("D") if network == TESTNET else ord("S"))
