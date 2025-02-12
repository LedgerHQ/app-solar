#pragma once

// Standard status words (ISO 7816-4)
#define SW_OK                ((uint16_t)0x9000)
#define SW_DENY              ((uint16_t)0x6985)
#define SW_WRONG_LENGTH      ((uint16_t)0x6700)
#define SW_WRONG_P1P2        ((uint16_t)0x6B00)
#define SW_INS_NOT_SUPPORTED ((uint16_t)0x6D00)
#define SW_CLA_NOT_SUPPORTED ((uint16_t)0x6E00)

#define SW_DISPLAY_BIP32_PATH_FAIL ((uint16_t)0xB001)
#define SW_DISPLAY_ADDRESS_FAIL    ((uint16_t)0xB002)
#define SW_WRONG_TX_LENGTH         ((uint16_t)0xB004)
#define SW_TX_PARSING_FAIL         ((uint16_t)0xB005)
#define SW_TX_HASH_FAIL            ((uint16_t)0xB006)
#define SW_BAD_STATE               ((uint16_t)0xB007)
#define SW_SIGNATURE_FAIL          ((uint16_t)0xB008)

#define SW_REQ_ORDER_FAIL           ((uint16_t)0xB009)
#define SW_DISPLAY_FAIL             ((uint16_t)0xB00A)
#define SW_PUBKEY_PARSING_FAIL      ((uint16_t)0xB00B)
#define SW_VALIDATION_HASH_MISMATCH ((uint16_t)0xB00C)
