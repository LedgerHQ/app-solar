import pytest

from ragger.error import ExceptionRAPDU

from application_client.solar_command_sender import CLA, P1, P2, Errors, InsType


# Verify that the Solar app returns an error when a bad CLA is used.
def test_bad_cla(backend):
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(cla=CLA + 1, ins=InsType.GET_VERSION)
    assert error.value.status == Errors.SW_CLA_NOT_SUPPORTED


# Verify that the Solar app returns an error when a bad INS is used.
def test_bad_ins(backend):
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(cla=CLA, ins=0xFF)
    assert error.value.status == Errors.SW_INS_NOT_SUPPORTED


# Verify that the Solar app returns an error when a bad P1 or P2 is used.
def test_wrong_p1p2(backend):
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA, ins=InsType.GET_VERSION, p1=P1.P1_START + 1, p2=P2.P2_LAST
        )
    assert error.value.status == Errors.SW_WRONG_P1P2
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA, ins=InsType.GET_VERSION, p1=P1.P1_START, p2=P2.P2_MORE
        )
    assert error.value.status == Errors.SW_WRONG_P1P2
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA, ins=InsType.GET_APP_NAME, p1=P1.P1_START + 1, p2=P2.P2_LAST
        )
    assert error.value.status == Errors.SW_WRONG_P1P2
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA, ins=InsType.GET_APP_NAME, p1=P1.P1_START, p2=P2.P2_MORE
        )
    assert error.value.status == Errors.SW_WRONG_P1P2


# Verify that the Solar app returns an error when a bad data length is used.
def test_wrong_data_length(backend):
    # APDUs must be at least 5 bytes: CLA, INS, P1, P2, Lc.
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange_raw(b"E0030000")
    assert error.value.status == Errors.SW_WRONG_LENGTH
    # APDUs advertises a too long length
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange_raw(b"E003000005")
    assert error.value.status == Errors.SW_WRONG_LENGTH


# Verify that invalid APDU sequences are properly rejected.
def test_invalid_state(backend):
    with pytest.raises(ExceptionRAPDU) as error:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX,
            p1=P1.P1_START + 1,  # attempt to continue instead of starting a new flow
            p2=P2.P2_MORE,
            data=b"abcde",
        )  # data is not parsed in this case
    assert error.value.status == Errors.SW_BAD_STATE
