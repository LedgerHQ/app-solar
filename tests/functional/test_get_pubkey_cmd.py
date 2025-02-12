import pytest

from ragger.bip import CurveChoice, calculate_public_key_and_chaincode
from ragger.error import ExceptionRAPDU
from ragger.firmware import Firmware
from ragger.navigator import NavInsID

from application_client.solar_command_sender import Errors, SolarCommandSender
from application_client.solar_response_unpacker import (
    unpack_get_public_key_chaincode_response,
    unpack_get_public_key_response,
)


# Verify the behaviour of GET_PUBLIC_KEY in non-confirmation mode.
def test_get_public_key_nonconfirm(backend):
    client = SolarCommandSender(backend)
    path = "m/44'/3333'/0'/0/0"

    response = client.get_public_key(path=path).data
    _, public_key = unpack_get_public_key_response(response)

    ref_public_key, _ = calculate_public_key_and_chaincode(
        CurveChoice.Secp256k1, path=path, compress_public_key=True
    )

    assert public_key.hex() == ref_public_key


# Verify the behaviour of GET_PUBLIC_KEY (with chaincode) in non-confirmation mode.
def test_get_public_key_with_chaincode_nonconfirm(backend):
    client = SolarCommandSender(backend)
    path = "m/44'/3333'/0'/0/0"

    response = client.get_public_key(path=path, chaincode=1).data
    _, public_key, _, chain_code = unpack_get_public_key_chaincode_response(response)

    ref_public_key, ref_chain_code = calculate_public_key_and_chaincode(
        CurveChoice.Secp256k1, path=path, compress_public_key=True
    )

    assert public_key.hex() == ref_public_key
    assert chain_code.hex() == ref_chain_code


# Verify the behaviour of GET_PUBLIC_KEY in confirmation mode when confirmed.
def test_get_public_key_confirmed(
    backend,
    navigator,
    firmware,
    test_name,
    default_screenshot_path,
    scenario_navigator
):
    client = SolarCommandSender(backend)
    path = "m/44'/3333'/0'/0/0"

    instructions = []

    if firmware is Firmware.STAX or firmware is Firmware.FLEX:
        instructions = [
            NavInsID.SWIPE_CENTER_TO_LEFT,
            NavInsID.USE_CASE_REVIEW_TAP,
            NavInsID.USE_CASE_ADDRESS_CONFIRMATION_TAP,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]

    with client.get_public_key_with_confirmation(path=path):
        if firmware.device.startswith("nano"):
            scenario_navigator.review_approve()
        else:
            navigator.navigate_and_compare(
                default_screenshot_path,
                test_name,
                instructions,
            )

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    _, public_key = unpack_get_public_key_response(response.data)

    ref_public_key, _ = calculate_public_key_and_chaincode(
        CurveChoice.Secp256k1, path=path, compress_public_key=True
    )

    assert public_key.hex() == ref_public_key


# Verify the behaviour of GET_PUBLIC_KEY (with chaincode) in confirmation mode when confirmed.
def test_get_public_key_with_chaincode_confirmed(
    backend,
    navigator,
    firmware,
    test_name,
    default_screenshot_path,
    scenario_navigator
):
    client = SolarCommandSender(backend)
    path = "m/44'/3333'/0'/0/0"

    instructions = []

    if firmware is Firmware.STAX or firmware is Firmware.FLEX:
        instructions = [
            NavInsID.SWIPE_CENTER_TO_LEFT,
            NavInsID.USE_CASE_REVIEW_TAP,
            NavInsID.USE_CASE_ADDRESS_CONFIRMATION_TAP,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]

    with client.get_public_key_with_confirmation(path=path, chaincode=1):
        if firmware.device.startswith("nano"):
            scenario_navigator.review_approve()
        else:
            navigator.navigate_and_compare(
                default_screenshot_path,
                test_name,
                instructions,
            )

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    _, public_key, _, chain_code = unpack_get_public_key_chaincode_response(response.data)

    ref_public_key, ref_chain_code = calculate_public_key_and_chaincode(
        CurveChoice.Secp256k1, path=path, compress_public_key=True
    )

    assert public_key.hex() == ref_public_key
    assert chain_code.hex() == ref_chain_code


# Verify the behaviour of GET_PUBLIC_KEY in confirmation mode when rejected.
def test_get_public_key_rejected(backend, scenario_navigator):
    client = SolarCommandSender(backend)
    path = "m/44'/3333'/0'/0/0"

    # if firmware.is_nano:
    with pytest.raises(ExceptionRAPDU) as error:
        with client.get_public_key_with_confirmation(path=path):
            scenario_navigator.address_review_reject()

    assert error.value.status == Errors.SW_DENY
    assert len(error.value.data) == 0
