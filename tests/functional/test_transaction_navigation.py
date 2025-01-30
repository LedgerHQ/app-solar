import hashlib

import pytest

from btclib.ecc import ssa

from ragger.firmware import Firmware
from ragger.navigator import NavIns, NavInsID

from application_client.solar_command_sender import SolarCommandSender
from application_client.solar_response_unpacker import unpack_get_public_key_response

from transactions.burn import Burn
from transactions.ipfs import Ipfs
from transactions.transfer import Transfer
from transactions.vote import Vote

from constants import PATH_MAINNET


################################################################################
# The Solar app implements dynamic / runtime navigation.
# The tests in this file are meant to help ensure that all screens are shown as expected.
################################################################################


################################################################################
# Test Variables                                                               #
################################################################################

TEST_NAME_EXTENSION = ["", "_with_short_memo", "_with_long_memo"]

NO_MEMO = ""
LONG_MEMO = (
    "All parts should go together without forcing. "
    "You must remember that the parts you are reassembling were disassembled by you. "
    "Therefore, if you can't get them together again, there must be a reason. "
    "By all means, do not use a hammer. ~ IBM Manual - (1975)"
)


################################################################################
# Screen Tap Helpers  (Flex, Stax)                                             #
################################################################################

# TxInfo Page Taps
TXINFO_TAP_FIELD_1_FLEX = NavIns(NavInsID.TOUCH, (430, 100))
TXINFO_TAP_FIELD_1_STAX = NavIns(NavInsID.TOUCH, (360, 105))

TXINFO_TAP_FIELD_2_FLEX = NavIns(NavInsID.TOUCH, (430, 202))
TXINFO_TAP_FIELD_2_STAX = NavIns(NavInsID.TOUCH, (360, 195))

TXINFO_TAP_FIELD_3_FLEX = NavIns(NavInsID.TOUCH, (430, 304))
TXINFO_TAP_FIELD_3_STAX = NavIns(NavInsID.TOUCH, (360, 285))


# Modal Taps
MODAL_TAP_FIELD_1_FLEX = NavIns(NavInsID.TOUCH, (240, 140))
MODAL_TAP_FIELD_1_STAX = NavIns(NavInsID.TOUCH, (360, 135))

MODAL_TAP_FIELD_2_FLEX = NavIns(NavInsID.TOUCH, (240, 230))
MODAL_TAP_FIELD_2_STAX = NavIns(NavInsID.TOUCH, (360, 230))

MODAL_TAP_FIELD_3_FLEX = NavIns(NavInsID.TOUCH, (240, 320))
MODAL_TAP_FIELD_3_STAX = NavIns(NavInsID.TOUCH, (360, 325))

MODAL_TAP_FIELD_4_FLEX = NavIns(NavInsID.TOUCH, (240, 410))
MODAL_TAP_FIELD_4_STAX = NavIns(NavInsID.TOUCH, (360, 420))

MODAL_TAP_FIELD_5_STAX = NavIns(NavInsID.TOUCH, (360, 515))


# Navigation Taps
# NAV_TAP_CLOSE_FLEX = NavIns(NavInsID.TOUCH, (90, 550))
NAV_TAP_CLOSE_FLEX = NavIns(NavInsID.TOUCH, (90, 550))
NAV_TAP_CLOSE_STAX = NavIns(NavInsID.TOUCH, (40, 625))

NAV_TAP_DISMISS_FLEX = NavIns(NavInsID.TOUCH, (240, 550))
NAV_TAP_DISMISS_STAX = NavIns(NavInsID.TOUCH, (200, 620))

NAV_TAP_NEXT_FLEX = NavIns(NavInsID.TOUCH, (430, 550))
NAV_TAP_NEXT_STAX = NavIns(NavInsID.TOUCH, (360, 625))


################################################################################
# IPFS Transaction                                                             #
# (TypeGroup 1, Type 5)                                                        #
################################################################################


# Verify the navigation behaviour of SIGN_TX while reviewing an ipfs transaction.
def test_transaction_navigation_ipfs(
    backend, default_screenshot_path, firmware, navigator, test_name
):
    if not firmware.is_nano:
        pytest.skip("Test only applicable to Nano (X,S+) devices")

    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    button_clicks = [4, 5, 9]

    for i, memo in enumerate([NO_MEMO, "This is an IPFS transaction.", LONG_MEMO]):
        clicks = button_clicks[i]

        ipfs_transaction = Ipfs(
            nonce=1,
            senderPkey=public_key,
            fee=645365,
            memo=memo,
            ipfs="122040e8c8cc86493d35f01b603190499ad4046cf2d097f5d34de034cebbba904804",
        )

        with client.sign_transaction(path=PATH_MAINNET, transaction=ipfs_transaction):
            instructions = [
                *([NavInsID.RIGHT_CLICK] * clicks),
                *([NavInsID.LEFT_CLICK] * clicks),
                *([NavInsID.RIGHT_CLICK] * (clicks - 1)),
                NavInsID.BOTH_CLICK,
            ]

            navigator.navigate_and_compare(
                path=default_screenshot_path,
                test_case_name=test_name + TEST_NAME_EXTENSION[i],
                instructions=instructions,
            )

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    assert ssa.verify(ipfs_transaction.serialise(),
                      public_key, response.data) is True


################################################################################
# Transfer Transaction                                                         #
# (TypeGroup 1, Type 6)                                                        #
################################################################################


# Transfer helper function to consistently generate valid unique addresses
def create_address(i):
    """Generate a single deterministic address using RIPEMD160"""
    # Start with '3f' network byte
    network = '3f'

    # Create a deterministic input for the hash
    # Convert i to bytes and add some salt to make it unique
    data = i.to_bytes(4, 'big') + b'solar_app_tests'

    # Generate RIPEMD160 hash
    h = hashlib.new('ripemd160')
    h.update(data)
    hash_bytes = h.digest()  # This gives us exactly 20 bytes

    # Combine network byte with the 20-byte hash
    return network + hash_bytes.hex()


# Verify the navigation behaviour of SIGN_TX while reviewing a transfer transaction.
def test_transaction_navigation_transfer(
    backend, firmware, navigator, default_screenshot_path, test_name
):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    payment_count = 14

    transfer_transaction = Transfer(
        nonce=2,
        senderPkey=public_key,
        fee=5645365,
        memo=LONG_MEMO,
        addresses=[create_address(i) for i in range(payment_count)],
        amounts=[ 1234567 ] * payment_count,
    )

    instructions = []

    if firmware.device.startswith("nano"):
        clicks = 36
        instructions = [
            *([NavInsID.RIGHT_CLICK] * clicks),
            *([NavInsID.LEFT_CLICK] * clicks),
            *([NavInsID.RIGHT_CLICK] * (clicks - 1)),
            NavInsID.BOTH_CLICK,
        ]
    elif firmware is Firmware.FLEX:
        instructions = [
            # Continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Open asset modal
            TXINFO_TAP_FIELD_1_FLEX,

            # Payments modal: 14 Payments
            * (
                # Pages: (1-4), (5-8), (9-12),
                [
                    MODAL_TAP_FIELD_1_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    MODAL_TAP_FIELD_2_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    MODAL_TAP_FIELD_3_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    MODAL_TAP_FIELD_4_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    NAV_TAP_NEXT_FLEX,
                ] * 3
            ),

            # Page: (13-14)
            MODAL_TAP_FIELD_1_FLEX,
            NAV_TAP_DISMISS_FLEX,

            MODAL_TAP_FIELD_2_FLEX,
            NAV_TAP_DISMISS_FLEX,

            # Close modal
            NAV_TAP_CLOSE_FLEX,

            # Memo details modal
            TXINFO_TAP_FIELD_3_FLEX,
            NAV_TAP_DISMISS_FLEX,

            NAV_TAP_NEXT_FLEX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]
    elif firmware is Firmware.STAX:
        instructions = [
            # Continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Open asset modal
            TXINFO_TAP_FIELD_1_STAX,

            # Payments modal: 14 Payments
            * (
                # Pages: (1-5), (6-10), (11-14)
                [
                    MODAL_TAP_FIELD_1_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_2_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_3_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_4_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_5_STAX,
                    NAV_TAP_DISMISS_STAX,

                    NAV_TAP_NEXT_STAX,
                ] * 2
            ),

            # Page: (11-14)
            MODAL_TAP_FIELD_1_STAX,
            NAV_TAP_DISMISS_STAX,

            MODAL_TAP_FIELD_2_STAX,
            NAV_TAP_DISMISS_STAX,

            MODAL_TAP_FIELD_3_STAX,
            NAV_TAP_DISMISS_STAX,

            MODAL_TAP_FIELD_4_STAX,
            NAV_TAP_DISMISS_STAX,

            # Close modal
            NAV_TAP_CLOSE_STAX,

            # Memo details modal
            TXINFO_TAP_FIELD_3_STAX,
            NAV_TAP_DISMISS_STAX,

            NAV_TAP_NEXT_STAX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]

    with client.sign_transaction(path=PATH_MAINNET, transaction=transfer_transaction):
        navigator.navigate_and_compare(
            path=default_screenshot_path,
            test_case_name=test_name,
            instructions=instructions,
        )

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    assert ssa.verify(transfer_transaction.serialise(),
                      public_key, response.data) is True


# Verify the navigation behaviour of SIGN_TX while reviewing a transfer with a single payment.
def test_transaction_navigation_transfer_single_payment(
    backend, firmware, navigator, default_screenshot_path, test_name
):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    payment_count = 1

    transfer_transaction = Transfer(
        nonce=2,
        senderPkey=public_key,
        fee=5645365,
        memo=LONG_MEMO,
        addresses=[create_address(i) for i in range(payment_count)],
        amounts=[ 1234567 ] * payment_count,
    )

    instructions = []

    if firmware.device.startswith("nano"):
        clicks = 10
        instructions = [
            *([NavInsID.RIGHT_CLICK] * clicks),
            *([NavInsID.LEFT_CLICK] * clicks),
            *([NavInsID.RIGHT_CLICK] * (clicks - 1)),
            NavInsID.BOTH_CLICK,
        ]
    elif firmware is Firmware.FLEX:
        instructions = [
            # continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Payment 1 of 1
            TXINFO_TAP_FIELD_1_FLEX,
            NAV_TAP_DISMISS_FLEX,

            # Memo details modal
            # tap extended memo field, then dismiss details modal
            TXINFO_TAP_FIELD_3_FLEX,
            NAV_TAP_DISMISS_FLEX,

            NAV_TAP_NEXT_FLEX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]
    elif firmware is Firmware.STAX:
        instructions = [
            # continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Payment 1 of 1
            TXINFO_TAP_FIELD_1_STAX,
            NAV_TAP_DISMISS_STAX,

            # Memo details modal
            TXINFO_TAP_FIELD_3_STAX,
            NAV_TAP_DISMISS_STAX,

            NAV_TAP_NEXT_STAX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]

    with client.sign_transaction(path=PATH_MAINNET, transaction=transfer_transaction):
        navigator.navigate_and_compare(
            path=default_screenshot_path,
            test_case_name=test_name,
            instructions=instructions,
        )

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    assert ssa.verify(transfer_transaction.serialise(),
                      public_key, response.data) is True


# Verify the navigation behaviour of SIGN_TX while reviewing a transfer with two payments.
def test_transaction_navigation_transfer_two_payments(
    backend, firmware, navigator, default_screenshot_path, test_name
):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    payment_count = 2

    transfer_transaction = Transfer(
        nonce=2,
        senderPkey=public_key,
        fee=5645365,
        memo=LONG_MEMO,
        addresses=[create_address(i) for i in range(payment_count)],
        amounts=[ 1234567 ] * payment_count,
    )

    instructions = []

    if firmware.device.startswith("nano"):
        clicks = 12
        instructions = [
            *([NavInsID.RIGHT_CLICK] * clicks),
            *([NavInsID.LEFT_CLICK] * clicks),
            *([NavInsID.RIGHT_CLICK] * (clicks - 1)),
            NavInsID.BOTH_CLICK,
        ]
    elif firmware is Firmware.FLEX:
        instructions = [
            # continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Open asset modal
            TXINFO_TAP_FIELD_1_FLEX,

            # Payments modal: 2 Payments
            # Page: (1-2)
            MODAL_TAP_FIELD_1_FLEX,
            NAV_TAP_DISMISS_FLEX,

            MODAL_TAP_FIELD_2_FLEX,
            NAV_TAP_DISMISS_FLEX,

            # Close modal
            NAV_TAP_CLOSE_FLEX,

            # Memo details modal
            TXINFO_TAP_FIELD_3_FLEX,
            NAV_TAP_DISMISS_FLEX,

            NAV_TAP_NEXT_FLEX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]
    elif firmware is Firmware.STAX:
        instructions = [
            # Continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Open asset modal
            TXINFO_TAP_FIELD_1_STAX,

            # Payments modal: 2 Payments
            # Page: (1-2)
            MODAL_TAP_FIELD_1_STAX,
            NAV_TAP_DISMISS_STAX,

            MODAL_TAP_FIELD_2_STAX,
            NAV_TAP_DISMISS_STAX,

            # Close modal
            NAV_TAP_CLOSE_STAX,

            # Memo details modal
            TXINFO_TAP_FIELD_3_STAX,
            NAV_TAP_DISMISS_STAX,

            NAV_TAP_NEXT_STAX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]

    with client.sign_transaction(path=PATH_MAINNET, transaction=transfer_transaction):
        navigator.navigate_and_compare(
            path=default_screenshot_path,
            test_case_name=test_name,
            instructions=instructions,
        )

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    assert ssa.verify(transfer_transaction.serialise(),
                      public_key, response.data) is True


# Verify the navigation behaviour of SIGN_TX while reviewing a transfer with 127 payments.
def test_transaction_navigation_transfer_max_payments(
    backend, firmware, navigator, default_screenshot_path, test_name
):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    payment_count = 127

    transfer_transaction = Transfer(
        nonce=2,
        senderPkey=public_key,
        fee=5645365,
        memo=LONG_MEMO,
        addresses=[create_address(i) for i in range(payment_count)],
        amounts=[ 1234567890 ] * payment_count,
    )

    instructions = []

    if firmware.device.startswith("nano"):
        clicks = 262
        instructions = [
            *([NavInsID.RIGHT_CLICK] * clicks),
            *([NavInsID.LEFT_CLICK] * clicks),
            *([NavInsID.RIGHT_CLICK] * (clicks - 1)),
            NavInsID.BOTH_CLICK,
        ]
    elif firmware is Firmware.FLEX:
        instructions = [
            # Continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Open asset modal
            TXINFO_TAP_FIELD_1_FLEX,

            # Payments modal: 127 Payments
            * (
                # Pages: (1-4), (5-8), ..., (121-124),
                [
                    MODAL_TAP_FIELD_1_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    MODAL_TAP_FIELD_2_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    MODAL_TAP_FIELD_3_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    MODAL_TAP_FIELD_4_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    NAV_TAP_NEXT_FLEX,
                ] * 31
            ),

            # Page: (125-127)
            MODAL_TAP_FIELD_1_FLEX,
            NAV_TAP_DISMISS_FLEX,

            MODAL_TAP_FIELD_2_FLEX,
            NAV_TAP_DISMISS_FLEX,

            MODAL_TAP_FIELD_3_FLEX,
            NAV_TAP_DISMISS_FLEX,

            # Close modal
            NAV_TAP_CLOSE_FLEX,

            # Memo details modal
            TXINFO_TAP_FIELD_3_FLEX,
            NAV_TAP_DISMISS_FLEX,

            NAV_TAP_NEXT_FLEX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]
    elif firmware is Firmware.STAX:
        instructions = [
            # Continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Open asset modal
            TXINFO_TAP_FIELD_1_STAX,

            # Payments modal: 14 Payments
            * (
                # Pages: (1-5), (6-10), (121-125)
                [
                    MODAL_TAP_FIELD_1_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_2_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_3_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_4_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_5_STAX,
                    NAV_TAP_DISMISS_STAX,

                    NAV_TAP_NEXT_STAX,
                ] * 25
            ),

            # Page: (126-127)
            MODAL_TAP_FIELD_1_STAX,
            NAV_TAP_DISMISS_STAX,

            MODAL_TAP_FIELD_2_STAX,
            NAV_TAP_DISMISS_STAX,

            # Close modal
            NAV_TAP_CLOSE_STAX,

            # Memo details modal
            TXINFO_TAP_FIELD_3_STAX,
            NAV_TAP_DISMISS_STAX,

            NAV_TAP_NEXT_STAX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]

    with client.sign_transaction(path=PATH_MAINNET, transaction=transfer_transaction):
        navigator.navigate_and_compare(
            path=default_screenshot_path,
            test_case_name=test_name,
            instructions=instructions,
        )

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    assert ssa.verify(transfer_transaction.serialise(),
                      public_key, response.data) is True


################################################################################
# Burn Transaction                                                             #
# (TypeGroup 2, Type 0)                                                        #
################################################################################


# Verify the navigation behaviour of SIGN_TX while reviewing a burn transaction.
def test_transaction_navigation_burn(
    backend, default_screenshot_path, firmware, navigator, test_name
):
    if not firmware.is_nano:
        pytest.skip("Test only applicable to Nano (X,S+) devices")

    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    nano_button_clicks = [4, 5, 9]

    for i, memo in enumerate([NO_MEMO, "This is a Burn transaction.", LONG_MEMO]):
        burn_transaction = Burn(
            nonce=3,
            senderPkey=public_key,
            fee=10000000,
            memo=memo,
            amount=250000000000,
        )
        with client.sign_transaction(path=PATH_MAINNET, transaction=burn_transaction):
            clicks = nano_button_clicks[i]
            instructions = [
                *([NavInsID.RIGHT_CLICK] * clicks),
                *([NavInsID.LEFT_CLICK] * clicks),
                *([NavInsID.RIGHT_CLICK] * (clicks - 1)),
                NavInsID.BOTH_CLICK,
            ]

            navigator.navigate_and_compare(
                path=default_screenshot_path,
                test_case_name=test_name + TEST_NAME_EXTENSION[i],
                instructions=instructions,
            )

        response = client.get_async_response()
        if response is None:
            raise ValueError("get_async_response returned None")

        assert ssa.verify(burn_transaction.serialise(),
                          public_key, response.data) is True


################################################################################
# Vote Transaction                                                             #
# (TypeGroup 2, Type 2)                                                        #
################################################################################


# Verify the navigation behaviour of SIGN_TX while reviewing a vote transaction.
def test_transaction_navigation_vote(
    backend, default_screenshot_path, firmware, navigator, test_name
):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    vote_transaction = Vote(
        nonce=5,
        senderPkey=public_key,
        fee=10000000,
        memo=LONG_MEMO,
        votes=[
            ["aurelion_sol", 625],
            ["deadlock", 625],
            ["bfx", 625],
            ["biz_classic", 625],
            ["cams_yellow_jacket", 625],
            ["crypticmaniac", 625],
            ["emsy", 625],
            ["fnoufnou", 625],
            ["gym", 625],
            ["goat", 625],
            ["kaos", 625],
            ["mtaylan", 625],
            ["nybl", 625],
            ["osrn", 625],
            ["pfeili", 625],
            ["sl33p", 625],
        ]
    )

    instructions = []

    if firmware.device.startswith("nano"):
        clicks = 40
        instructions = [
            *([NavInsID.RIGHT_CLICK] * clicks),
            *([NavInsID.LEFT_CLICK] * clicks),
            *([NavInsID.RIGHT_CLICK] * (clicks - 1)),
            NavInsID.BOTH_CLICK,
        ]
    elif firmware is Firmware.FLEX:
        instructions = [
            # Continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Open asset modal
            TXINFO_TAP_FIELD_1_FLEX,

            # Votes modal: 16 Votes
            * (
                # Pages: (1-4), (5-8), (9-12),
                [
                    MODAL_TAP_FIELD_1_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    MODAL_TAP_FIELD_2_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    MODAL_TAP_FIELD_3_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    MODAL_TAP_FIELD_4_FLEX,
                    NAV_TAP_DISMISS_FLEX,

                    NAV_TAP_NEXT_FLEX,
                ] * 3
            ),

            # Page: (13-16)
            MODAL_TAP_FIELD_1_FLEX,
            NAV_TAP_DISMISS_FLEX,

            MODAL_TAP_FIELD_2_FLEX,
            NAV_TAP_DISMISS_FLEX,

            MODAL_TAP_FIELD_3_FLEX,
            NAV_TAP_DISMISS_FLEX,

            MODAL_TAP_FIELD_4_FLEX,
            NAV_TAP_DISMISS_FLEX,

            # Close modal
            NAV_TAP_CLOSE_FLEX,

            # Memo details modal
            TXINFO_TAP_FIELD_3_FLEX,
            NAV_TAP_DISMISS_FLEX,

            NAV_TAP_NEXT_FLEX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]
    elif firmware is Firmware.STAX:
        instructions = [
            # Continue review (to txinfo page)
            NavInsID.SWIPE_CENTER_TO_LEFT,

            # Open asset modal
            TXINFO_TAP_FIELD_1_STAX,

            # Votes modal: 16 Payments
            * (
                # Pages: (1-5), (6-10), (11-15)
                [
                    MODAL_TAP_FIELD_1_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_2_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_3_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_4_STAX,
                    NAV_TAP_DISMISS_STAX,

                    MODAL_TAP_FIELD_5_STAX,
                    NAV_TAP_DISMISS_STAX,

                    NAV_TAP_NEXT_STAX,
                ] * 3
            ),

            # Page: (11-14)
            MODAL_TAP_FIELD_1_STAX,
            NAV_TAP_DISMISS_STAX,

            # Close modal
            NAV_TAP_CLOSE_STAX,

            # Memo details modal
            TXINFO_TAP_FIELD_3_STAX,
            NAV_TAP_DISMISS_STAX,

            NAV_TAP_NEXT_STAX,

            NavInsID.USE_CASE_REVIEW_CONFIRM,
            NavInsID.USE_CASE_STATUS_DISMISS,
        ]

    with client.sign_transaction(path=PATH_MAINNET, transaction=vote_transaction):
        navigator.navigate_and_compare(
            path=default_screenshot_path,
            test_case_name=test_name,
            instructions=instructions,
        )

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    assert ssa.verify(vote_transaction.serialise(),
                      public_key, response.data) is True


# Verify the navigation behaviour of SIGN_TX while reviewing a cancel vote transaction.
def test_transaction_navigation_vote_cancel(
    backend, default_screenshot_path, firmware, navigator, test_name
):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    nano_button_clicks = [3, 4, 8]

    for i, memo in enumerate([NO_MEMO, "This is a Cancel Vote transaction.", LONG_MEMO]):
        cancel_vote_transaction = Vote(
            nonce=4,
            senderPkey=public_key,
            fee=10000000,
            memo=memo,
            votes=[],
        )

        instructions = []

        if firmware.device.startswith("nano"):
            clicks = nano_button_clicks[i]
            instructions = [
                *([NavInsID.RIGHT_CLICK] * clicks),
                *([NavInsID.LEFT_CLICK] * clicks),
                *([NavInsID.RIGHT_CLICK] * (clicks - 1)),
                NavInsID.BOTH_CLICK,
            ]
        elif firmware is Firmware.FLEX:
            instructions = [
                # Continue review (to txinfo page)
                NavInsID.SWIPE_CENTER_TO_LEFT,

                * (  # Memo details modal (for long memos)
                    [
                        TXINFO_TAP_FIELD_2_FLEX,
                        NAV_TAP_DISMISS_FLEX,
                    ] if i == 2 else []
                ),

                NAV_TAP_NEXT_FLEX,

                NavInsID.USE_CASE_REVIEW_CONFIRM,
                NavInsID.USE_CASE_STATUS_DISMISS,
            ]
        elif firmware is Firmware.STAX:
            instructions = [
                # Continue review (to txinfo page)
                NavInsID.SWIPE_CENTER_TO_LEFT,

                * (  # Memo details modal (for long memos)
                    [
                        TXINFO_TAP_FIELD_2_STAX,
                        NAV_TAP_DISMISS_STAX,
                    ] if i == 2 else []
                ),

                NAV_TAP_NEXT_STAX,

                NavInsID.USE_CASE_REVIEW_CONFIRM,
                NavInsID.USE_CASE_STATUS_DISMISS,
            ]

        with client.sign_transaction(
            path=PATH_MAINNET, transaction=cancel_vote_transaction
        ):
            navigator.navigate_and_compare(
                path=default_screenshot_path,
                test_case_name=test_name + TEST_NAME_EXTENSION[i],
                instructions=instructions,
            )

        response = client.get_async_response()
        if response is None:
            raise ValueError("get_async_response returned None")

        assert (
            ssa.verify(cancel_vote_transaction.serialise(),
                       public_key, response.data)
            is True
        )
