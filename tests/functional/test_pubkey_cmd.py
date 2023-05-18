import pytest

from application_client.boilerplate_command_sender import SolarCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_get_public_key_response
from ragger.bip import calculate_public_key_and_chaincode, CurveChoice
from ragger.error import ExceptionRAPDU
from ragger.navigator import NavInsID, NavIns
from utils import ROOT_SCREENSHOT_PATH


# In this test we check that the GET_PUBLIC_KEY works in non-confirmation mode
def test_get_public_key_no_confirm(backend):
    for path in ["m/44'/1'/0'/0/0", "m/44'/1'/0/0/0", "m/44'/1'/911'/0/0", "m/44'/1'/255/255/255", "m/44'/1'/2147483647/0/0/0/0/0/0/0"]:
        client = SolarCommandSender(backend)
        response = client.get_public_key(path=path).data
        _, public_key, _, chain_code = unpack_get_public_key_response(response)

        ref_public_key, ref_chain_code = calculate_public_key_and_chaincode(CurveChoice.Secp256k1, path=path)
        assert public_key.hex() == ref_public_key
        assert chain_code.hex() == ref_chain_code


# In this test we check that the GET_PUBLIC_KEY works in confirmation mode
def test_get_public_key_confirm_accepted(firmware, backend, navigator, test_name):
    client = SolarCommandSender(backend)
    path = "m/44'/1'/0'/0/0"
    with client.get_public_key_with_confirmation(path=path):
        if firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                      [NavInsID.BOTH_CLICK],
                                                      "Approve",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)
        else:
            instructions = [
                NavInsID.USE_CASE_REVIEW_TAP,
                NavIns(NavInsID.TOUCH, (200, 335)),
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_EXIT_QR,
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_TAP,
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CONFIRM,
                NavInsID.USE_CASE_STATUS_DISMISS
            ]
            navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                           test_name,
                                           instructions)
    response = client.get_async_response().data
    _, public_key, _, chain_code = unpack_get_public_key_response(response)

    ref_public_key, ref_chain_code = calculate_public_key_and_chaincode(CurveChoice.Secp256k1, path=path)
    assert public_key.hex() == ref_public_key
    assert chain_code.hex() == ref_chain_code


# In this test we check that the GET_PUBLIC_KEY in confirmation mode replies an error if the user refuses
def test_get_public_key_confirm_refused(firmware, backend, navigator, test_name):
    client = SolarCommandSender(backend)
    path = "m/44'/1'/0'/0/0"

    if firmware.device.startswith("nano"):
        with pytest.raises(ExceptionRAPDU) as e:
            with client.get_public_key_with_confirmation(path=path):
                navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                          [NavInsID.BOTH_CLICK],
                                                          "Reject",
                                                          ROOT_SCREENSHOT_PATH,
                                                          test_name)
        # Assert that we have received a refusal
        assert e.value.status == Errors.SW_DENY
        assert len(e.value.data) == 0
    else:
        instructions_set = [
            [
                NavInsID.USE_CASE_REVIEW_REJECT,
                NavInsID.USE_CASE_STATUS_DISMISS
            ],
            [
                NavInsID.USE_CASE_REVIEW_TAP,
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CANCEL,
                NavInsID.USE_CASE_STATUS_DISMISS
            ]
        ]
        for i, instructions in enumerate(instructions_set):
            with pytest.raises(ExceptionRAPDU) as e:
                with client.get_public_key_with_confirmation(path=path):
                    navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                                   test_name + f"/part{i}",
                                                   instructions)
            # Assert that we have received a refusal
            assert e.value.status == Errors.SW_DENY
            assert len(e.value.data) == 0

# import pytest

# from client.exception import *
# from speculos.client import ApduException
# from client.cmd_builder import InsType


# def test_get_public_key(cmd):

#     # No Display, No Chain Code
#     pub_key, chain_code = cmd.get_public_key(
#         bip32_path="44'/3333'/0'/0/0", display=0, chaincode=0
#     )

#     assert len(pub_key) == 33
#     assert len(chain_code) == 0

#     # Display, No Chain Code
#     pub_key2, chain_code2 = cmd.get_public_key(
#         bip32_path="44'/3333'/0'/0/0",
#         display=1,
#         chaincode=0,
#         n_screens=5,
#     )

#     assert len(pub_key2) == 33
#     assert len(chain_code2) == 0

#     # Display, Chain Code
#     pub_key3, chain_code3 = cmd.get_public_key(
#         bip32_path="44'/3333'/0'/0/0",
#         display=1,
#         chaincode=1,
#         n_screens=5,
#     )

#     assert len(pub_key3) == 33
#     assert len(chain_code3) == 32

#     # No Display, Chain Code
#     pub_key4, chain_code4 = cmd.get_public_key(
#         bip32_path="44'/3333'/0'/0/0", display=0, chaincode=1
#     )

#     assert len(pub_key4) == 33
#     assert len(chain_code4) == 32


# @pytest.mark.xfail(raises=WrongP1P2Error, strict=True)
# def test_wrong_chaincode_option(cmd):
#     try:
#         cmd.get_public_key(bip32_path="44'/3333'/0'/0/0", display=0, chaincode=2)
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=InsType.INS_GET_PUBLIC_KEY)


# @pytest.mark.xfail(raises=WrongP1P2Error, strict=True)
# def test_wrong_display_option(cmd):
#     try:
#         cmd.get_public_key(bip32_path="44'/3333'/0'/0/0", display=2, chaincode=0)
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=InsType.INS_GET_PUBLIC_KEY)
