import pytest

from ragger.error import ExceptionRAPDU
from ragger.firmware import Firmware
from ragger.navigator import NavIns, NavInsID

from application_client.solar_command_sender import (
    CLA,
    Errors,
    InsType,
    SolarCommandSender,
)
from application_client.solar_response_unpacker import unpack_get_address_response

from constants import (
    NETWORK_MAINNET,
    NETWORK_TESTNET,
    NETWORKS,
    PATH_MAINNET,
    PATH_TESTNET,
)


# Verify the behaviour of GET_ADDRESS in non-confirmation mode.
def test_get_address_nonconfirm(backend):
    client = SolarCommandSender(backend)

    for network in NETWORKS:
        response = client.get_address(
            path=PATH_TESTNET if network is NETWORK_TESTNET else PATH_MAINNET,
            network=network,
        ).data

        address_len, address = unpack_get_address_response(response)

        assert address_len == 34
        assert address[0] == (ord("D") if network is NETWORK_TESTNET else ord("S"))


# Verify the behaviour of GET_ADDRESS in confirmation mode when confirmed.
def test_get_address_confirmed(backend, scenario_navigator):
    client = SolarCommandSender(backend)

    with client.get_address_with_confirmation(
        path=PATH_MAINNET, network=NETWORK_MAINNET
    ):
        scenario_navigator.address_review_approve()

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    address_len, address = unpack_get_address_response(response.data)

    assert address_len == 34
    assert address[0] == ord("S")


# Verify that GET_ADDRESS (with a QR) works as expected in confirmation mode.
def test_get_address_with_qr_confirmed(
    backend, firmware, navigator, default_screenshot_path, test_name
):
    if firmware.is_nano:
        pytest.skip("Test only applicable to e-ink (Flex,Stax) devices")
    else:
        client = SolarCommandSender(backend)

        with client.get_address_with_confirmation(
            path=PATH_MAINNET, network=NETWORK_MAINNET
        ):
            qr_tap = (
                NavIns(NavInsID.TOUCH, (64, 520))
                if firmware is Firmware.STAX
                else NavIns(NavInsID.TOUCH, (77, 466))
            )

            instructions = [
                NavInsID.SWIPE_CENTER_TO_LEFT,
                qr_tap,
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_EXIT_QR,
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CONFIRM,
                NavInsID.USE_CASE_STATUS_DISMISS,
            ]

            navigator.navigate_and_compare(
                default_screenshot_path, test_name, instructions
            )

        response = client.get_async_response()
        if response is None:
            raise ValueError("get_async_response returned None")

        address_len, address = unpack_get_address_response(response.data)

        assert address_len == 34
        assert address[0] == ord("S")


# Verify the behaviour of GET_ADDRESS in confirmation mode when rejected.
def test_get_address_rejected(backend, scenario_navigator):
    client = SolarCommandSender(backend)

    with pytest.raises(ExceptionRAPDU) as error:
        with client.get_address_with_confirmation(
            path=PATH_MAINNET, network=NETWORK_MAINNET
        ):
            scenario_navigator.address_review_reject()

    assert error.value.status == Errors.SW_DENY
    assert len(error.value.data) == 0


# Verify the behaviour of GET_ADDRESS when the provided network is not supported.
def test_get_address_unsupported_network(backend):
    # client = SolarCommandSender(backend)
    no_confirm = 0x00
    unsupported_network = 0x3E

    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA, ins=InsType.GET_ADDRESS, p1=no_confirm, p2=unsupported_network
        )

    assert error.value.status == Errors.SW_WRONG_P1P2
