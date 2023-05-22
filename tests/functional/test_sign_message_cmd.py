import pytest

from btclib.ecc import ssa

from application_client.solar_command_sender import (
    CLA,
    InsType,
    SolarCommandSender,
    Errors,
)
from application_client.solar_response_unpacker import (
    unpack_get_public_key_response,
    unpack_sign_tx_response,
)

from ragger.bip import calculate_public_key_and_chaincode, CurveChoice

from ragger.error import ExceptionRAPDU
from ragger.navigator import NavInsID, NavIns
from utils import ROOT_SCREENSHOT_PATH


PATH_MAINNET: str = "m/44'/3333'/0'/0/0"

MESSAGE_SHORT: str = "We are the music makers,\n the dreamers of dreams...\n"
MESSAGE_LONG: str = "A long, long time ago the World was in an age of Chaos. In the midst of the chaos, in a little kingdom in the land of Hyrule, a legend was being handed down from generation to generation, the legend of the 'Triforce': golden triangles possessing mystical powers. One day, an evil army led by Ganon, the powerful Prince of Darkness, attacked and stole the Triforce of Power. Fearing his wicked rule, princess Zelda split the Triforce of Wisdom into eight fragments and hid them throughout the realm. She commanded her most trustworthy nursemaid, Impa, to escape and search for someone courageous enough to destroy the evil Ganon. Upon hearing this, Ganon grew angry, imprisoned the princess, and sent out a party in search of Impa. Braving forests and mountains, Impa fled for her life from her pursuers. As she reached the very limit of her energy she found herself surrounded by Ganon's evil henchmen. Cornered! What could she do? But wait! All was not lost. A young lad appeared, driving off Ganon's henchmen and saving Impa from a fate worse than death. His name was Link. Impa told Link the story of Zelda and the evil Ganon. Burning with a sense of justice, Link resolved to save Zelda. But Ganon was a powerful opponent. He held the Triforce of Power. Link had to bring the scattered eight fragments of the Triforce of Wisdom together to rebuild the mystical triangle or there would be no chance he could fight his way into Death Mountain where Ganon lived. Can Link really destroy Ganon and save princess Zelda? Only your skill can answer that question. Good luck. Use the Triforce wisely."

# In this tests we check the behavior of the device when asked to sign a short message
def test_sign_message_short(firmware, backend, navigator, test_name):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    with client.sign_message(path=PATH_MAINNET, message=MESSAGE_SHORT):
        # if firmware.device.startswith("nano"):
        navigator.navigate_until_text_and_compare(
            NavInsID.RIGHT_CLICK,
            [NavInsID.BOTH_CLICK],
            "Approve",
            ROOT_SCREENSHOT_PATH,
            test_name,
        )
        # else:
        #     navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
        #                                               [NavInsID.USE_CASE_REVIEW_CONFIRM,
        #                                                NavInsID.USE_CASE_STATUS_DISMISS],
        #                                               "Hold to sign",
        #                                               ROOT_SCREENSHOT_PATH,
        #                                               test_name)
    response = client.get_async_response().data

    assert ssa.verify(MESSAGE_SHORT.encode("ascii"), public_key, response) is True


# In this tests we check the behavior of the device when asked to sign a long message
def test_sign_message_long(firmware, backend, navigator, test_name):
    client = SolarCommandSender(backend)

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    with client.sign_message(path=PATH_MAINNET, message=MESSAGE_LONG):
        # if firmware.device.startswith("nano"):
        navigator.navigate_until_text_and_compare(
            NavInsID.RIGHT_CLICK,
            [NavInsID.BOTH_CLICK],
            "Approve",
            ROOT_SCREENSHOT_PATH,
            test_name,
        )
        # else:
        #     navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
        #                                               [NavInsID.USE_CASE_REVIEW_CONFIRM,
        #                                                NavInsID.USE_CASE_STATUS_DISMISS],
        #                                               "Hold to sign",
        #                                               ROOT_SCREENSHOT_PATH,
        #                                               test_name)
    response = client.get_async_response().data

    assert ssa.verify(MESSAGE_LONG.encode("ascii"), public_key, response) is True


# Message signature refused test
# The test will ask for a message signature that will be refused on screen
def test_sign_message_refused(firmware, backend, navigator, test_name):
    client = SolarCommandSender(backend)

    # if firmware.device.startswith("nano"):
    with pytest.raises(ExceptionRAPDU) as e:
        with client.sign_message(path=PATH_MAINNET, message=MESSAGE_SHORT):
            # if firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(
                NavInsID.RIGHT_CLICK,
                [NavInsID.BOTH_CLICK],
                "Reject",
                ROOT_SCREENSHOT_PATH,
                test_name,
            )
    assert e.value.status == Errors.SW_DENY
    assert len(e.value.data) == 0
    # else:
    #     for i in range(3):
    #         instructions = [NavInsID.USE_CASE_REVIEW_TAP] * i
    #         instructions += [NavInsID.USE_CASE_REVIEW_REJECT,
    #                         NavInsID.USE_CASE_CHOICE_CONFIRM,
    #                         NavInsID.USE_CASE_STATUS_DISMISS]
    #         with pytest.raises(ExceptionRAPDU) as e:
    #             with client.sign_tx(path=PATH_MAINNET, transaction=transaction):
    #                 navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
    #                                             test_name + f"/part{i}",
    #                                             instructions)
    #         # Assert that we have received a refusal
    #         assert e.value.status == Errors.SW_DENY
    #         assert len(e.value.data) == 0
