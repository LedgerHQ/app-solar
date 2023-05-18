import pytest

from ragger.error import ExceptionRAPDU
from application_client.solar_command_sender import CLA, InsType, P1, P2, Errors


# Ensure the app returns an error when a bad CLA is used
def test_bad_cla(backend):
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA + 1, ins=InsType.GET_VERSION)
    assert e.value.status == Errors.SW_CLA_NOT_SUPPORTED


# Ensure the app returns an error when a bad INS is used
def test_bad_ins(backend):
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=0xff)
    assert e.value.status == Errors.SW_INS_NOT_SUPPORTED


# Ensure the app returns an error when a bad P1 or P2 is used
def test_wrong_p1p2(backend):
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_VERSION, p1=P1.P1_START + 1, p2=P2.P2_LAST)
        print("ins=InsType.GET_VERSION, p1=P1.P1_START + 1, p2=P2.P2_LAST")
    assert e.value.status == Errors.SW_WRONG_P1P2
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_VERSION, p1=P1.P1_START, p2=P2.P2_MORE)
        print("ins=InsType.GET_VERSION, p1=P1.P1_START, p2=P2.P2_MORE")
    assert e.value.status == Errors.SW_WRONG_P1P2
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_APP_NAME, p1=P1.P1_START + 1, p2=P2.P2_LAST)
        print("ins=InsType.GET_APP_NAME, p1=P1.P1_START + 1, p2=P2.P2_LAST")
    assert e.value.status == Errors.SW_WRONG_P1P2
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_APP_NAME, p1=P1.P1_START, p2=P2.P2_MORE)
        print("ins=InsType.GET_APP_NAME, p1=P1.P1_START, p2=P2.P2_MORE")
    assert e.value.status == Errors.SW_WRONG_P1P2


# Ensure the app returns an error when a bad data length is used
def test_wrong_data_length(backend):
    # APDUs must be at least 5 bytes: CLA, INS, P1, P2, Lc.
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange_raw(b"E0030000")
    assert e.value.status == Errors.SW_WRONG_DATA_LENGTH
    # APDUs advertises a too long length
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange_raw(b"E003000005")
    assert e.value.status == Errors.SW_WRONG_DATA_LENGTH


# Ensure there is no state confusion when trying wrong APDU sequences
def test_invalid_state(backend):
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA,
                         ins=InsType.SIGN_TX,
                         p1=P1.P1_START + 1,  # Try to continue a flow instead of start a new one
                         p2=P2.P2_MORE,
                         data=b"abcde")  # data is not parsed in this case
    assert e.value.status == Errors.SW_BAD_STATE

# import pytest

# from client.exception import *
# from speculos.client import ApduException


# @pytest.mark.xfail(raises=ClaNotSupportedError, strict=True)
# def test_bad_cla(cmd):
#     try:
#         cmd.transport.apdu_exchange(
#             cla=0xA0, ins=0xB2, p1=0x00, p2=0x00, cdata=b""  # 0xa0 instead of 0xe0
#         )
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=0xB2)


# @pytest.mark.xfail(raises=InsNotSupportedError, strict=True)
# def test_bad_ins(cmd):
#     try:
#         cmd.transport.apdu_exchange(
#             cla=0xE0, ins=0xFF, p1=0x00, p2=0x00, cdata=b""  # bad INS
#         )
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=0xFF)


# @pytest.mark.xfail(raises=WrongP1P2Error, strict=True)
# def test_wrong_p1p2(cmd):
#     try:
#         cmd.transport.apdu_exchange(
#             cla=0xE0, ins=0xA1, p1=0x01, p2=0x00, cdata=b""  # 0x01 instead of 0x00
#         )
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=0xA1)


# @pytest.mark.xfail(raises=WrongDataLengthError, strict=True)
# def test_wrong_data_length(cmd):
#     try:
#         # APDUs must be at least 5 bytes: CLA, INS, P1, P2, Lc.
#         cmd.transport.apdu_exchange_raw("E000")
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=None)


# @pytest.mark.xfail(raises=OutOfOrderReqError, strict=True)
# def test_wrong_order(cmd):
#     try:
#         cmd.transport.apdu_exchange(
#             cla=0xE0,
#             ins=0xC1,
#             p1=0x00,
#             p2=0x80,
#             cdata=bytes.fromhex("058000002c80000d05800000000000000000000000"),
#         )

#         cmd.transport.apdu_exchange(
#             cla=0xE0, ins=0xC1, p1=0x01, p2=0x80, cdata=b"a" * 255
#         )

#         cmd.transport.apdu_exchange(
#             cla=0xE0, ins=0xC1, p1=0x03, p2=0x00, cdata=b"a"  # wrong p1
#         )
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=0xC1)


# @pytest.mark.xfail(raises=WrongTxLengthError, strict=True)
# def test_not_full_req(cmd):
#     try:
#         cmd.transport.apdu_exchange(
#             cla=0xE0,
#             ins=0xC1,
#             p1=0x00,
#             p2=0x80,
#             cdata=bytes.fromhex("058000002c80000d05800000000000000000000000"),
#         )

#         cmd.transport.apdu_exchange(
#             cla=0xE0, ins=0xC1, p1=0x01, p2=0x80, cdata=b"a" * 255
#         )

#         cmd.transport.apdu_exchange(
#             cla=0xE0,
#             ins=0xC1,
#             p1=0x02,
#             p2=0x80,
#             cdata=b"a" * 254,  # more but chunk not full
#         )
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=0xC1)


# @pytest.mark.xfail(raises=BadStateError, strict=True)
# def test_instruction_swap(cmd):
#     try:
#         cmd.transport.apdu_exchange(
#             cla=0xE0,
#             ins=0xC1,
#             p1=0x00,
#             p2=0x80,
#             cdata=bytes.fromhex("058000002c80000d05800000000000000000000000"),
#         )

#         cmd.transport.apdu_exchange(
#             cla=0xE0, ins=0xC1, p1=0x01, p2=0x80, cdata=b"a" * 255
#         )

#         cmd.transport.apdu_exchange(
#             cla=0xE0, ins=0xC2, p1=0x02, p2=0x80, cdata=b"b" * 255
#         )
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=0xC2)


# @pytest.mark.xfail(raises=BadStateError, strict=True)
# def test_state_cleared(cmd):
#     try:
#         cmd.transport.apdu_exchange(
#             cla=0xE0,
#             ins=0xC1,
#             p1=0x00,
#             p2=0x80,
#             cdata=bytes.fromhex("058000002c80000d05800000000000000000000000"),
#         )

#         cmd.transport.apdu_exchange_with_buttons(
#             cla=0xE0,
#             ins=0xC1,
#             p1=0x01,
#             p2=0x00,
#             cdata=bytes.fromhex("fd00" + "63" * 253),
#             n_screens=13,
#         )

#         cmd.transport.apdu_exchange(
#             cla=0xE0, ins=0xC1, p1=0x02, p2=0x00, cdata=b"b" * 255
#         )
#     except ApduException as error:
#         raise DeviceException(error_code=error.sw, ins=0xC1)
