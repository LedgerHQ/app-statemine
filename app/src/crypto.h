/*******************************************************************************
*   (c) 2019 Zondax GmbH
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <zxmacros.h>
#include "coin.h"
#include <stdbool.h>
#include <sigutils.h>
#include "zxerror.h"

#ifdef SUPPORT_SR25519
// Flash
typedef struct {
    uint8_t sk[SK_LEN_25519];
    uint8_t pk[PK_LEN_25519];
    uint8_t signdata[MAX_SIGN_SIZE];
    uint8_t signature[SIG_PLUS_TYPE_LEN];
} sr25519_signdata_t;

extern uint16_t sr25519_signdataLen;

#if defined(TARGET_NANOS) || defined(TARGET_NANOX)
sr25519_signdata_t NV_CONST N_srdata_impl __attribute__ ((aligned(64)));
#define N_sr25519_signdata (*(NV_VOLATILE sr25519_signdata_t *)PIC(&N_srdata_impl))
#endif

void zeroize_sr25519_signdata(void) {
    _Static_assert(MAX_SIGN_SIZE >= SK_LEN_25519, "invalid SK_LEN_25519");
    _Static_assert(MAX_SIGN_SIZE >= PK_LEN_25519, "invalid PK_LEN_25519");
    _Static_assert(MAX_SIGN_SIZE >= SIG_PLUS_TYPE_LEN, "invalid SIG_PLUS_TYPE_LEN");

    uint8_t zero[MAX_SIGN_SIZE];
    explicit_bzero(zero, sizeof(zero));

    MEMCPY_NV((void *) &N_sr25519_signdata.sk, zero, SK_LEN_25519);
    MEMCPY_NV((void *) &N_sr25519_signdata.pk, zero, PK_LEN_25519);
    MEMCPY_NV((void *) &N_sr25519_signdata.signdata, zero, MAX_SIGN_SIZE);
    MEMCPY_NV((void *) &N_sr25519_signdata.signature, zero, SIG_PLUS_TYPE_LEN);
    sr25519_signdataLen = 0;
}

#else

void zeroize_sr25519_signdata(void) {
}

#endif

//#define SS58_BLAKE_PREFIX  (const unsigned char *) "SS58PRE"
//#define SS58_BLAKE_PREFIX_LEN 7
#define SS58_ADDRESS_MAX_LEN 60u

#define PREFIX_SIGNATURE_TYPE_ED25519  0
#define PREFIX_SIGNATURE_TYPE_SR25519  1
#define PREFIX_SIGNATURE_TYPE_EDCSA    2
#define MIN_BUFFER_LENGTH              235
#define START_BUFFER                   33

extern uint32_t hdPath[HDPATH_LEN_DEFAULT];

uint8_t crypto_SS58EncodePubkey(uint8_t *buffer, uint16_t buffer_len,
                                uint8_t addressType, const uint8_t *pubkey);

zxerr_t crypto_fillAddress(key_kind_e addressKind, uint8_t *buffer, uint16_t bufferLen, uint16_t *addrResponseLen);

zxerr_t crypto_sign_sr25519_prephase(const uint8_t *message, uint16_t messageLen);

zxerr_t crypto_sign_sr25519(uint8_t *signature, uint16_t signatureMaxlen);

zxerr_t crypto_sign_ed25519(uint8_t *signature, uint16_t signatureMaxlen,
                            const uint8_t *message, uint16_t messageLen);

#ifdef __cplusplus
}
#endif
