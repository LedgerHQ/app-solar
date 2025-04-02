import pytest

from btclib.ecc import ssa

from ragger.error import ExceptionRAPDU
from ragger.firmware import Firmware
from ragger.navigator import NavIns, NavInsID

from application_client.solar_command_sender import Errors, SolarCommandSender
from application_client.solar_response_unpacker import unpack_get_public_key_response

from constants import PATH_MAINNET


MESSAGE_SHORT: str = "We are the music makers,\nthe dreamers of dreams...\n"
MESSAGE_SHORT_NBGL: str = "We are the music makers, the dreamers of dreams..."
MESSAGE_INVALID: str = "We are the music\rmakers, the dreamers of dreams..."
MESSAGE_LONG: str = (
    "A long, long time ago the World was in an age of Chaos. In the midst of the chaos, "
    "in a little kingdom in the land of Hyrule, a legend was being handed down from generation "
    "to generation, the legend of the 'Triforce': golden triangles possessing mystical powers. "
    "One day, an evil army led by Ganon, the powerful Prince of Darkness, attacked and stole "
    "the Triforce of Power. Fearing his wicked rule, princess Zelda split the Triforce of "
    "Wisdom into eight fragments and hid them throughout the realm. She commanded her most "
    "trustworthy nursemaid, Impa, to escape and search for someone courageous enough to "
    "destroy the evil Ganon. Upon hearing this, Ganon grew angry, imprisoned the princess, "
    "and sent out a party in search of Impa. Braving forests and mountains, Impa fled for "
    "her life from her pursuers. As she reached the very limit of her energy she found herself "
    "surrounded by Ganon's evil henchmen. Cornered! What could she do? But wait! All was not "
    "lost. A young lad appeared, driving off Ganon's henchmen and saving Impa from a fate worse "
    "than death. His name was Link. Impa told Link the story of Zelda and the evil Ganon. "
    "Burning with a sense of justice, Link resolved to save Zelda. But Ganon was a powerful "
    "opponent. He held the Triforce of Power. Link had to bring the scattered eight fragments "
    "of the Triforce of Wisdom together to rebuild the mystical triangle or there would be no "
    "chance he could fight his way into Death Mountain where Ganon lived. Can Link really "
    "destroy Ganon and save princess Zelda? Only your skill can answer that question. "
    "Good luck. Use the Triforce wisely."
)


# Verify the behaviour of SIGN_MESSAGE when asked to sign a short message.
def test_sign_message_short_signed(backend, firmware, scenario_navigator):
    client = SolarCommandSender(backend)

    message = (
        MESSAGE_SHORT if firmware.device.startswith("nano") else MESSAGE_SHORT_NBGL
    )

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    with client.sign_message(path=PATH_MAINNET, message=message):
        scenario_navigator.review_approve()

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    msg_short: bytes = b"".join(
        [
            len(message).to_bytes(2, byteorder="little"),
            bytes(message, "ascii"),
        ]
    )

    assert ssa.verify(msg_short, public_key, response.data) is True


# Verify the behaviour of SIGN_MESSAGE when asked to sign a long message.
def test_sign_message_long_signed(
    backend,
    navigator,
    firmware,
    test_name,
    default_screenshot_path,
    scenario_navigator
):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    instructions = []

    if firmware.device.startswith("nano"):
        with client.sign_message(path=PATH_MAINNET, message=MESSAGE_LONG):
            scenario_navigator.review_approve()
    else:
        if firmware is Firmware.FLEX:
            instructions = [
                NavInsID.USE_CASE_REVIEW_TAP,
                NavIns(NavInsID.TOUCH, (240, 380)),
                *([NavInsID.USE_CASE_VIEW_DETAILS_NEXT] * 5),
                NavInsID.USE_CASE_VIEW_DETAILS_EXIT,
                NavInsID.USE_CASE_REVIEW_TAP,
                NavInsID.USE_CASE_REVIEW_CONFIRM,
                NavInsID.USE_CASE_STATUS_DISMISS,
            ]
        elif firmware is Firmware.STAX:
            instructions = [
                NavInsID.USE_CASE_REVIEW_TAP,
                NavIns(NavInsID.TOUCH, (200, 420)),
                *([NavInsID.USE_CASE_VIEW_DETAILS_NEXT] * 4),
                NavInsID.USE_CASE_VIEW_DETAILS_EXIT,
                NavInsID.USE_CASE_REVIEW_TAP,
                NavInsID.USE_CASE_REVIEW_CONFIRM,
                NavInsID.USE_CASE_STATUS_DISMISS,
            ]

        with client.sign_message(path=PATH_MAINNET, message=MESSAGE_LONG):
            navigator.navigate_and_compare(
                default_screenshot_path, test_name, instructions
            )

    response = client.get_async_response()
    if response is None:
        raise ValueError("get_async_response returned None")

    msg_long: bytes = b"".join(
        [
            len(MESSAGE_LONG).to_bytes(2, byteorder="little"),
            bytes(MESSAGE_LONG, "ascii"),
        ]
    )

    assert ssa.verify(msg_long, public_key, response.data) is True


# Verify the behaviour of SIGN_MESSAGE when the user rejects signing.
def test_sign_message_rejected(backend, firmware, scenario_navigator):
    client = SolarCommandSender(backend)

    message = (
        MESSAGE_SHORT if firmware.device.startswith("nano") else MESSAGE_SHORT_NBGL
    )

    with pytest.raises(ExceptionRAPDU) as error:
        with client.sign_message(path=PATH_MAINNET, message=message):
            scenario_navigator.review_reject()

    assert error.value.status == Errors.SW_DENY
    assert len(error.value.data) == 0


# Verify the behaviour of SIGN_MESSAGE when the message contains invalid text.
def test_sign_message_invalid_ascii(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, _ = unpack_get_public_key_response(rapdu.data)

    with pytest.raises(ExceptionRAPDU) as error:
        with client.sign_message_sync(path=PATH_MAINNET, message=MESSAGE_INVALID):
            assert error.value.status == Errors.SW_TX_PARSING_FAIL
            assert len(error.value.data) == 0


# Verify the behaviour of SIGN_MESSAGE when the message is empty.
def test_sign_message_empty(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, _ = unpack_get_public_key_response(rapdu.data)

    with pytest.raises(ExceptionRAPDU) as error:
        with client.sign_message_sync(path=PATH_MAINNET, message=""):
            assert error.value.status == Errors.SW_TX_PARSING_FAIL
            assert len(error.value.data) == 0


# Verify the behaviour of SIGN_MESSAGE when the message is too long.
def test_sign_message_invalid_length(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, _ = unpack_get_public_key_response(rapdu.data)

    with pytest.raises(ExceptionRAPDU) as error:
        with client.sign_message_sync(
            path=PATH_MAINNET, message="X" * 4096
        ):

            assert error.value.status == Errors.SW_TX_PARSING_FAIL
            assert len(error.value.data) == 0
