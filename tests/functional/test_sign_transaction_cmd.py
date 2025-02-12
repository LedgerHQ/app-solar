import pytest

from btclib.ecc import ssa

from ragger.error import ExceptionRAPDU

from application_client.solar_command_sender import (
    Errors,
    SolarCommandSender,
)
from application_client.solar_response_unpacker import unpack_get_public_key_response

from transactions.burn import Burn
from transactions.ipfs import Ipfs
from transactions.transfer import Transfer
from transactions.vote import Vote

from constants import PATH_MAINNET


################################################################################
# Test Variables                                                               #
################################################################################


TEST_NAME_EXTENSION = ["", "_with_short_memo", "_with_long_memo", ""]

NO_MEMO = ""
LONG_MEMO = (
    "All parts should go together without forcing. "
    "You must remember that the parts you are reassembling were disassembled by you. "
    "Therefore, if you can't get them together again, there must be a reason. "
    "By all means, do not use a hammer. ~ IBM Manual - (1975)"
)
REJECT_MEMO = "This transaction should be rejected."


################################################################################
# Test Helpers                                                                 #
################################################################################


def approve_tx(client, scenario_navigator, case_name, transaction, public_key):
    with client.sign_transaction(path=PATH_MAINNET, transaction=transaction):
        scenario_navigator.review_approve(
            test_name=case_name + "_signed"
        )

    response = client.get_async_response().data
    assert ssa.verify(transaction.serialise(),
                      public_key, response) is True


def reject_tx(client, scenario_navigator, case_name, transaction):
    with pytest.raises(ExceptionRAPDU) as error:
        with client.sign_transaction(path=PATH_MAINNET, transaction=transaction):
            scenario_navigator.review_reject(
                test_name=case_name + "_rejected"
            )

    assert error.value.status == Errors.SW_DENY
    assert len(error.value.data) == 0


################################################################################
# IPFS Transaction                                                             #
# (TypeGroup 1, Type 5)                                                        #
################################################################################


# Verify the behaviour of SIGN_TX when asked to sign an ipfs transaction.
def test_sign_transaction_ipfs(backend, scenario_navigator, test_name):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    for i, memo in enumerate([NO_MEMO, "This is an IPFS transaction.", LONG_MEMO, REJECT_MEMO]):
        ipfs_transaction = Ipfs(
            nonce=1,
            senderPkey=public_key,
            fee=645365,
            memo=memo,
            ipfs="122040e8c8cc86493d35f01b603190499ad4046cf2d097f5d34de034cebbba904804",
        )

        if memo == REJECT_MEMO:
            reject_tx(client,
                      scenario_navigator,
                      test_name + TEST_NAME_EXTENSION[i],
                      ipfs_transaction)
        else:
            approve_tx(client,
                       scenario_navigator,
                       test_name + TEST_NAME_EXTENSION[i],
                       ipfs_transaction,
                       public_key)


################################################################################
# Transfer Transaction                                                         #
# (TypeGroup 1, Type 6)                                                        #
################################################################################


# Verify the behaviour of SIGN_TX when asked to sign a transfer transaction.
def test_sign_transaction_transfer(backend, scenario_navigator, test_name):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    for i, memo in enumerate([NO_MEMO, "This is a Transfer transaction.", LONG_MEMO, REJECT_MEMO]):
        transfer_transaction = Transfer(
            nonce=2,
            senderPkey=public_key,
            fee=5645365,
            memo=memo,
            addresses=[
                # SN7z4VXicX4YXBsiAGRcYtXP9eqzgZCex6
                "3f091327b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # SPav3G7t4MUq747c6yJbYfJrmszCHx2S9Q
                "3f192327b917bf2b464e9b8f1acf1588f4cb6e7bb3",
                # SR3r22i3WBu7gwDW3gBScC69XsrzVtifYU
                "3f293327b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # SSWmzoJCx2KQGotPzP4MdqsXizNVNwxyon
                "3f394327b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # STyhyZtNPrjgrgZHw5wGfVeuv6szJiBcpc
                "3f495327b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # SVSdxLUXqh9ySZEBsnpBh9SJ7DPVDM7usB
                "3f596327b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # SWuZw74hHXaG2Ru5pVh6ioDgJKtz991yrw
                "3f697327b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # SYNVuserjMzYcJZymCalkT14VSQV9Jk5U3
                "3f798327b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # SZqRteF2BCQqCBEshuSvn6nSgYuz4TMrmP
                "3f899327b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # SbF4CR8MrvS4CTfcrhWGppHcj785vWJFR9
                "3f990327b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # ScilNfdLjKWunuYCjLj4Su4Nk82tiyuaAa
                "3fa91427b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # SeAdPDZEphxfECBmizTta617YgWm418kYB
                "3fb91527b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # SfdFPmV8v6QQfUqLieCihGwrMEzdMi4HAL
                "3fc91627b917bf2b464e9b8f1acf0588f4cb6e7bb3",
                # Sh5sQKR31UrA6mUuiHwYpTtb9oUVi9BpMF
                "3fd91727b917bf2b464e9b8f1acf0588f4cb6e7bb3",
            ],
            amounts=[
                123456,
                1234567,
                12345678,
                123456789,
                1234567890,
                12345678900,
                123456789000,
                1234567890000,
                12345678900000,
                123456789000000,
                1234567890000000,
                12345678900000000,
                123456789000000000,
                18446744073709551615,
            ],
        )

        if memo == REJECT_MEMO:
            reject_tx(client,
                      scenario_navigator,
                      test_name + TEST_NAME_EXTENSION[i],
                      transfer_transaction)
        else:
            approve_tx(client,
                       scenario_navigator,
                       test_name + TEST_NAME_EXTENSION[i],
                       transfer_transaction,
                       public_key)


################################################################################
# Burn Transaction                                                             #
# (TypeGroup 2, Type 0)                                                        #
################################################################################


# Verify the behaviour of SIGN_TX when asked to sign a burn transaction.
def test_sign_transaction_burn(backend, scenario_navigator, test_name):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    for i, memo in enumerate([NO_MEMO, "This is a Burn transaction.", LONG_MEMO, REJECT_MEMO]):
        burn_transaction = Burn(
            nonce=3,
            senderPkey=public_key,
            fee=10000000,
            memo=memo,
            amount=250000000000,
        )

        if memo == REJECT_MEMO:
            reject_tx(client,
                      scenario_navigator,
                      test_name + TEST_NAME_EXTENSION[i],
                      burn_transaction)
        else:
            approve_tx(client,
                       scenario_navigator,
                       test_name + TEST_NAME_EXTENSION[i],
                       burn_transaction,
                       public_key)


################################################################################
# Vote Transaction                                                             #
# (TypeGroup 2, Type 2)                                                        #
################################################################################


# Verify the behaviour of SIGN_TX when asked to sign a vote transaction.
def test_sign_transaction_vote(backend, scenario_navigator, test_name):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    for i, memo in enumerate([NO_MEMO, "This is a Vote transaction.", LONG_MEMO, REJECT_MEMO]):
        vote_transaction = Vote(
            nonce=4,
            senderPkey=public_key,
            fee=10000000,
            memo=memo,
            votes=[["gym", 2000], ["bfx", 4000], ["sl33p", 4000]],
        )

        if memo == REJECT_MEMO:
            reject_tx(client,
                      scenario_navigator,
                      test_name + TEST_NAME_EXTENSION[i],
                      vote_transaction)
        else:
            approve_tx(client,
                       scenario_navigator,
                       test_name + TEST_NAME_EXTENSION[i],
                       vote_transaction,
                       public_key)


# Verify the behaviour of SIGN_TX when asked to sign a cancel vote transaction.
def test_sign_transaction_vote_cancel(backend, scenario_navigator, test_name):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    for i, memo in enumerate(
        [NO_MEMO, "This is a Cancel Vote transaction.", LONG_MEMO, REJECT_MEMO]
    ):
        cancel_vote_transaction = Vote(
            nonce=5,
            senderPkey=public_key,
            fee=10000000,
            memo=memo,
            votes=[],
        )

        if memo == REJECT_MEMO:
            reject_tx(client,
                      scenario_navigator,
                      test_name + TEST_NAME_EXTENSION[i],
                      cancel_vote_transaction)
        else:
            approve_tx(client,
                       scenario_navigator,
                       test_name + TEST_NAME_EXTENSION[i],
                       cancel_vote_transaction,
                       public_key)
