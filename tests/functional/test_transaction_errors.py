import pytest

from ragger.error import ExceptionRAPDU

from application_client.solar_command_sender import (
    CLA,
    MAX_APDU_LEN,
    P1,
    P2,
    Errors,
    InsType,
    SolarCommandSender,
    split_message,
)
from application_client.solar_response_unpacker import unpack_get_public_key_response
from application_client.solar_transaction import Transaction

from transactions.transfer import Transfer
from transactions.vote import Vote

from constants import PATH_MAINNET


# Verify the behaviour of SIGN_TX when a transfer uses an invalid typegroup.
def test_transaction_invalid_type_group(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    transaction = Transaction(
        typeGroup=3,
        type=0,
        nonce=6,
        senderPkey=public_key,
        fee=2000000,
        memo="",
    )

    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("058000002c80000d05800000000000000000000000"),
        )

        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START + 1,
            p2=P2.P2_LAST,
            data=transaction.serialise(),
        )

    assert error.value.status == Errors.SW_TX_PARSING_FAIL


# Verify the behaviour of SIGN_TX when a transfer uses an invalid type.
def test_transaction_invalid_type(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    transaction = Transaction(
        typeGroup=20,
        type=20,
        nonce=6,
        senderPkey=public_key,
        fee=2000000,
        memo="",
    )

    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("058000002c80000d05800000000000000000000000"),
        )

        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START + 1,
            p2=P2.P2_LAST,
            data=transaction.serialise(),
        )

    assert error.value.status == Errors.SW_TX_PARSING_FAIL


# Verify the behaviour of SIGN_TX when a transfer's' payment limit is exceeded.
def test_transaction_transfer_invalid_limit(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    payment_limit = 127

    transfer_transaction = Transfer(
        nonce=2,
        senderPkey=public_key,
        fee=5645365,
        memo="",
        addresses=[ "3fc91327b917bf2b464e9b8f1acf0588f4cb6e7bb3" ] * (payment_limit + 1),
        amounts=[ 1 ] * (payment_limit + 1),
    )

    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("058000002c80000d05800000000000000000000000"),
        )

        chunks = split_message(transfer_transaction.serialise(), MAX_APDU_LEN)

        for chunk in chunks:
            backend.exchange(
                cla=CLA,
                ins=InsType.SIGN_TX,
                p1=P1.P1_START + 1,
                p2=P2.P2_LAST,
                data=chunk,
            )

    assert error.value.status == Errors.SW_TX_PARSING_FAIL


# Verify the behaviour of SIGN_TX when a transfer uses an unsupported tx version.
def test_transaction_transfer_invalid_version(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    transfer_transaction = Transfer(
        version=2,
        nonce=2,
        senderPkey=public_key,
        fee=5645365,
        memo="",
        addresses=["3f091327b917bf2b464e9b8f1acf0588f4cb6e7bb3"],
        amounts=[123456],
    )

    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("058000002c80000d05800000000000000000000000"),
        )

        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START + 1,
            p2=P2.P2_LAST,
            data=transfer_transaction.serialise(),
        )

    assert error.value.status == Errors.SW_TX_PARSING_FAIL


# Verify the behaviour of SIGN_TX when a transfer uses an unsupported network.
def test_transaction_transfer_invalid_network(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    transfer_transaction = Transfer(
        network=62,
        nonce=2,
        senderPkey=public_key,
        fee=5645365,
        memo="",
        addresses=["3f091327b917bf2b464e9b8f1acf0588f4cb6e7bb3"],
        amounts=[123456],
    )

    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("058000002c80000d05800000000000000000000000"),
        )

        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START + 1,
            p2=P2.P2_LAST,
            data=transfer_transaction.serialise(),
        )

    assert error.value.status == Errors.SW_TX_PARSING_FAIL


# Verify the behaviour of SIGN_TX when a transfer uses an invalid starting byte.
def test_transaction_transfer_invalid_starting_byte(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    transfer_transaction = Transfer(
        startingByte=0xFE,
        nonce=2,
        senderPkey=public_key,
        fee=5645365,
        memo="",
        addresses=["3f091327b917bf2b464e9b8f1acf0588f4cb6e7bb3"],
        amounts=[123456],
    )

    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("058000002c80000d05800000000000000000000000"),
        )

        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START + 1,
            p2=P2.P2_LAST,
            data=transfer_transaction.serialise(),
        )

    assert error.value.status == Errors.SW_TX_PARSING_FAIL


# Verify the behaviour of SIGN_TX when a transfer memo uses invalid characters.
def test_transaction_transfer_invalid_memo(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    transfer_transaction = Transfer(
        nonce=2,
        senderPkey=public_key,
        fee=5645365,
        memo="this\nis\nnot\nvalid",
        addresses=["3f091327b917bf2b464e9b8f1acf0588f4cb6e7bb3"],
        amounts=[123456],
    )

    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("058000002c80000d05800000000000000000000000"),
        )

        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START + 1,
            p2=P2.P2_LAST,
            data=transfer_transaction.serialise(),
        )

    assert error.value.status == Errors.SW_TX_PARSING_FAIL


# Verify the behaviour of SIGN_TX when the data length is invalid.
def test_transaction_invalid_data_length(backend):
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
        )

        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START + 1,
            p2=P2.P2_LAST,
            data=bytes.fromhex(
                "ff033f010000000600010000000000000002937e1b9294d0"
                "7c91b46d511851d698ad606a64950dd3a332fcfac9c0a3fb"
                "0bad352456000000000005616161616161010040e2010000"
                "0000003fc91327b917bf2b464e9b8f1acf0588f4cb6e7bb3"
            ),
        )

    assert error.value.status == Errors.SW_WRONG_LENGTH


# Verify the behaviour of SIGN_TX when a vote has an invalid vote sum.
def test_transaction_transfer_invalid_vote_sum(backend):
    client = SolarCommandSender(backend)

    rapdu = client.get_public_key(path=PATH_MAINNET)
    _, public_key = unpack_get_public_key_response(rapdu.data)

    vote_transaction = Vote(
        nonce=6,
        senderPkey=public_key,
        fee=10000000,
        memo="",
        votes=[["gym", 2001], ["cactus1549", 4000], ["sl33p", 4000]],
    )

    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("058000002c80000d05800000000000000000000000"),
        )

        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START + 1,
            p2=P2.P2_LAST,
            data=vote_transaction.serialise(),
        )

    assert error.value.status == Errors.SW_TX_PARSING_FAIL
