#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <openssl/evp.h>
#include "sm4_ossl.h"
#include "crypto_types.h"
#include "err.h"			/* for srtp_debug */
#include "alloc.h"
#include "cipher_types.h"

srtp_debug_module_t srtp_mod_sm4 = {
    0,				/* debugging is off by default */
    "sm4 ossl"		/* printable module name       */
};


static const uint8_t srtp_sm4_test_case_0_key[30] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0xf0, 0xf1, 0xf2, 0xf3,
    0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd
};

static srtp_err_status_t srtp_sm4_openssl_alloc(srtp_cipher_t **c,
                                                    int key_len,
                                                    int tlen)
{
	srtp_sm4_ctx_t* icm;
    *c = (srtp_cipher_t *)srtp_crypto_alloc(sizeof(srtp_cipher_t));
    if (*c == NULL) {
        return srtp_err_status_alloc_fail;
    }

    icm = (srtp_sm4_ctx_t*)srtp_crypto_alloc(sizeof(srtp_sm4_ctx_t));
    if (icm == NULL) {
        srtp_crypto_free(*c);
        *c = NULL;
        return srtp_err_status_alloc_fail;
    }

    icm->ctx = EVP_CIPHER_CTX_new();
    if (icm->ctx == NULL) {
        srtp_crypto_free(icm);
        srtp_crypto_free(*c);
        *c = NULL;
        return srtp_err_status_alloc_fail;
    }

    (*c)->state = icm;

    (*c)->algorithm = SRTP_SM4;
    (*c)->type = &srtp_sm4;
    icm->key_size = 16;
    (*c)->key_len = key_len;

    return srtp_err_status_ok;
}


static srtp_err_status_t srtp_sm4_openssl_dealloc(srtp_cipher_t *c)
{
  srtp_sm4_ctx_t* ctx;
    if (c == NULL) {
        return srtp_err_status_bad_param;
    }
    ctx = (srtp_sm4_ctx_t*)c->state;
    if (ctx != NULL) {
        EVP_CIPHER_CTX_free(ctx->ctx);
		octet_string_set_to_zero(ctx, sizeof(srtp_sm4_ctx_t));
        srtp_crypto_free(ctx);
    }
    srtp_crypto_free(c);
    return srtp_err_status_ok;
}


static srtp_err_status_t srtp_sm4_openssl_context_init(void *cv,
                                                           const uint8_t *key)
{
	srtp_sm4_ctx_t* c = (srtp_sm4_ctx_t*)cv;
// 	sms4_set_encrypt_key(&c->en_key, srtp_sm4_test_case_0_key);
//     sms4_set_decrypt_key(&c->de_key, srtp_sm4_test_case_0_key);

	const EVP_CIPHER* evp;
    v128_set_to_zero(&c->counter);
    v128_set_to_zero(&c->offset);
    memcpy(&c->counter, key + c->key_size, SRTP_SALT_LEN);
    memcpy(&c->offset, key + c->key_size, SRTP_SALT_LEN);

    c->offset.v8[SRTP_SALT_LEN] = c->offset.v8[SRTP_SALT_LEN + 1] = 0;
    c->counter.v8[SRTP_SALT_LEN] = c->counter.v8[SRTP_SALT_LEN + 1] = 0;

	evp = EVP_sms4_ctr();
	if (!EVP_EncryptInit_ex(c->ctx, evp, NULL, key, NULL)) {
		return srtp_err_status_fail;
	}
	return srtp_err_status_ok;
}


static srtp_err_status_t srtp_sm4_openssl_set_iv(
    void *cv,
    uint8_t *iv,
    srtp_cipher_direction_t dir)
{
	srtp_sm4_ctx_t* c = (srtp_sm4_ctx_t*)cv;
	v128_t nonce;

	/* set nonce (for alignment) */
	v128_copy_octet_string(&nonce, iv);

	v128_xor(&c->counter, &c->offset, &nonce);

	if (!EVP_EncryptInit_ex(c->ctx, NULL, NULL, NULL, c->counter.v8)) {
		return srtp_err_status_fail;
	} else {
		return srtp_err_status_ok;
	}
    return srtp_err_status_ok;
}

static srtp_err_status_t srtp_sm4_openssl_encrypt(void *cv,
                                                      unsigned char *buf,
                                                      unsigned int *enc_len)
{
// 	srtp_sm4_ctx_t* c = (srtp_sm4_ctx_t*)cv;
// 	sms4_encrypt(buf, buf, &c->en_key);
	srtp_sm4_ctx_t* c = (srtp_sm4_ctx_t*)cv;
	int len = 0;


	if (!EVP_EncryptUpdate(c->ctx, buf, &len, buf, *enc_len)) {
		return srtp_err_status_cipher_fail;
	}
	*enc_len = len;

	if (!EVP_EncryptFinal_ex(c->ctx, buf, &len)) {
		return srtp_err_status_cipher_fail;
	}
	*enc_len += len;
    return srtp_err_status_ok;
}

static srtp_err_status_t srtp_sm4_openssl_decrypt(void* cv,
                                                  unsigned char* buf,
                                                  unsigned int* enc_len) {
//   srtp_sm4_ctx_t* c = (srtp_sm4_ctx_t*)cv;
//   sms4_encrypt(buf, buf, &c->de_key);
	srtp_sm4_ctx_t* c = (srtp_sm4_ctx_t*)cv;
	int len = 0;

	if (!EVP_EncryptUpdate(c->ctx, buf, &len, buf, *enc_len)) {
		return srtp_err_status_cipher_fail;
	}
	*enc_len = len;

	if (!EVP_EncryptFinal_ex(c->ctx, buf, &len)) {
		return srtp_err_status_cipher_fail;
	}
	*enc_len += len;
	return srtp_err_status_ok;
}

static const char srtp_sm4_openssl_description[] =
    "SM4 mode using openssl";

uint32_t rk[32] = {
    0xf12186f9, 0x41662b61, 0x5a6ab19a, 0x7ba92077, 0x367360f4, 0x776a0c61,
    0xb6bb89b3, 0x24763151, 0xa520307c, 0xb7584dbd, 0xc30753ed, 0x7ee55b57,
    0x6988608c, 0x30d895b7, 0x44ba14af, 0x104495a1, 0xd120b428, 0x73b55fa3,
    0xcc874966, 0x92244439, 0xe89e641f, 0x98ca015a, 0xc7159060, 0x99e1fd2e,
    0xb79bd80c, 0x1d2115b0, 0x0e228aeb, 0xf1780c81, 0x428d3654, 0x62293496,
    0x01cf72e5, 0x9124a012,
};



static uint8_t srtp_sm4_test_case_0_nonce[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t srtp_sm4_test_case_0_plaintext[16] =  {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};

static const uint8_t srtp_sm4_test_case_0_ciphertext[16] = {
//     0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e,
//     0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46
	0x78,0xf8,0xa8,0xe5,0xee,0x84,0x87,0xcf,0xb0,0xf5,0xbd,
	0x44,0x8f,0xe9,0x75,0x64
};


static const srtp_cipher_test_case_t srtp_sm4_test_case_0 = {
    30,											/* octets in key            */
    srtp_sm4_test_case_0_key,					/* key                      */
    srtp_sm4_test_case_0_nonce,					/* packet index             */
    16,											/* octets in plaintext      */
    srtp_sm4_test_case_0_plaintext,				/* plaintext                */
    16,											/* octets in ciphertext     */
    srtp_sm4_test_case_0_ciphertext,			/* ciphertext               */
    0,											/* */
    NULL,										/* */
    0,											/* */
    NULL										/* pointer to next testcase */
};

const srtp_cipher_type_t srtp_sm4 = {
    srtp_sm4_openssl_alloc,					/* */
    srtp_sm4_openssl_dealloc,				/* */
    srtp_sm4_openssl_context_init,			/* */
    0,										/* set_aad */
    srtp_sm4_openssl_encrypt,				/* */
    srtp_sm4_openssl_decrypt,				/* */
    srtp_sm4_openssl_set_iv,				/* */
    0,										/* get_tag */
    srtp_sm4_openssl_description,			/* */
    &srtp_sm4_test_case_0,					/* */
    SRTP_SM4								/* */
};


