################################################################################
# Module Description                                                           #
################################################################################

project(Dispatcher
  VERSION 1.2.0
  DESCRIPTION "Apdu dispatcher of the Solar app"
  LANGUAGES C)

################################################################################
# Module Definitions                                                           #
################################################################################

add_definitions(
  -DAPPNAME=\"Solar\"
  -DAPPVERSION=\"1.2.0\"
  -DMAJOR_VERSION=1
  -DMINOR_VERSION=2
  -DPATCH_VERSION=0
  -DBAGL_HEIGHT=64
  -DBAGL_WIDTH=128
  -DHAVE_BAGL
  -DDEBUG=1
  -DHAVE_ECC
  -DHAVE_ECC_WEIERSTRASS
  -DHAVE_HASH
  -DHAVE_IO_USB
  -DHAVE_L4_USBLIB
  -DHAVE_PRINTF
  -DHAVE_SECP256K1_CURVE
  -DHAVE_SHA256
  -DHAVE_SPRINTF
  -DHAVE_USB_APDU
  -DHAVE_UX_FLOW
  -DIO_HID_EP_LENGTH=64
  -DIO_SEPROXYHAL_BUFFER_SIZE_B=300
  -DIO_USB_MAX_ENDPOINTS=4
  -DOS_IO_SEPROXYHAL
  -DPRINTF=mcu_usb_printf
  -DSCREEN_SIZE_NANO
  -DUSB_SEGMENT_SIZE=64)

################################################################################
# Module Source Files                                                          #
################################################################################

set(DISPATCHER_LEDGER_MOCKS
  "${CMAKE_SOURCE_DIR}/src/buffer_mock.c"
  "${CMAKE_SOURCE_DIR}/src/io_mock.c"
  "${CMAKE_SOURCE_DIR}/src/pic_mock.c")

set(DISPATCHER_LEDGER_SRC "${DISPATCHER_LEDGER_MOCKS}"
  "${BOLOS_SDK}/lib_standard_app/bip32.c"
  "${BOLOS_SDK}/lib_standard_app/buffer.c"
  "${BOLOS_SDK}/lib_standard_app/format.c"
  "${BOLOS_SDK}/lib_standard_app/parser.c"
  "${BOLOS_SDK}/lib_standard_app/read.c"
  "${BOLOS_SDK}/lib_standard_app/varint.c"
  "${BOLOS_SDK}/lib_standard_app/write.c")

set(DISPATCHER_SOLAR_MOCKS
  "${CMAKE_SOURCE_DIR}/src/crypto_mock.c"
  "${CMAKE_SOURCE_DIR}/src/display_address_mock.c"
  "${CMAKE_SOURCE_DIR}/src/display_message_mock.c"
  "${CMAKE_SOURCE_DIR}/src/display_public_key_mock.c"
  "${CMAKE_SOURCE_DIR}/src/display_transaction_mock.c"
  "${CMAKE_SOURCE_DIR}/src/send_response_mock.c")

set(DISPATCHER_SOLAR_SRC "${DISPATCHER_SOLAR_MOCKS}"
  "${SOLAR_SOURCE_DIR}/apdu/dispatcher.c"
  "${SOLAR_SOURCE_DIR}/context.c"
  "${SOLAR_SOURCE_DIR}/handler/get_address.c"
  "${SOLAR_SOURCE_DIR}/handler/get_app_name.c"
  "${SOLAR_SOURCE_DIR}/handler/get_public_key.c"
  "${SOLAR_SOURCE_DIR}/handler/get_version.c"
  "${SOLAR_SOURCE_DIR}/handler/sign_tx.c"
  "${SOLAR_SOURCE_DIR}/transaction/deserialise_message.c"
  "${SOLAR_SOURCE_DIR}/transaction/deserialise_transaction.c"
  "${SOLAR_SOURCE_DIR}/transaction/types/burn.c"
  "${SOLAR_SOURCE_DIR}/transaction/types/ipfs.c"
  "${SOLAR_SOURCE_DIR}/transaction/types/transfer.c"
  "${SOLAR_SOURCE_DIR}/transaction/types/vote.c"
  "${SOLAR_SOURCE_DIR}/utils/check_ascii.c")

add_library(dispatcher
  "${DISPATCHER_LEDGER_SRC}"
  "${DISPATCHER_SOLAR_SRC}")

################################################################################
# Module Properties                                                            #
################################################################################

set_target_properties(dispatcher PROPERTIES SOVERSION 1)

################################################################################
# Module Include Directories                                                   #
################################################################################

target_include_directories(dispatcher PUBLIC
  "${SOLAR_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/include"
  "${BOLOS_SDK}/include"
  "${BOLOS_SDK}/lib_bagl/include"
  "${BOLOS_SDK}/lib_standard_app"
  "${BOLOS_SDK}/lib_cxng/include"
  "${BOLOS_SDK}/lib_ux/include"
  "${BOLOS_SDK}/lib_ux/src"
  "${BOLOS_SDK}/target/nanox/include")
