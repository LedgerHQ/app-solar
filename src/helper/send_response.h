#pragma once

#include <stdint.h>  // uint*_t

/* -------------------------------------------------------------------------- */

#define CHAINCODE_LEN ((uint8_t)32)

typedef enum { ADDRESS_RESPONSE, PUBKEY_RESPONSE, SIGNATURE_RESPONSE } helper_response_t;

/* -------------------------------------------------------------------------- */

int helper_send(helper_response_t responseType);
