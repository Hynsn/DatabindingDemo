//
// Created by housaibang on 2021-01-28.
//

#ifndef TESTDEMO_MYAES_H
#define TESTDEMO_MYAES_H
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "mbedtls/include/base64.h"
#include "mbedtls/include/aes.h"
#include "mbedtls/include/platform.h"

#include <android/log.h>

const char * LOG_TGA = "LOG_TGA";

static const unsigned char base64_test_dec[64] =
        {
                0x24, 0x48, 0x6E, 0x56, 0x87, 0x62, 0x5A, 0xBD,
                0xBF, 0x17, 0xD9, 0xA2, 0xC4, 0x17, 0x1A, 0x01,
                0x94, 0xED, 0x8F, 0x1E, 0x11, 0xB3, 0xD7, 0x09,
                0x0C, 0xB6, 0xE9, 0x10, 0x6F, 0x22, 0xEE, 0x13,
                0xCA, 0xB3, 0x07, 0x05, 0x76, 0xC9, 0xFA, 0x31,
                0x6C, 0x08, 0x34, 0xFF, 0x8D, 0xC2, 0x6C, 0x38,
                0x00, 0x43, 0xE9, 0x54, 0x97, 0xAF, 0x50, 0x4B,
                0xD1, 0x41, 0xBA, 0x95, 0x31, 0x5A, 0x0B, 0x97
        };

static const unsigned char base64_test_enc[] =
        "JEhuVodiWr2/F9mixBcaAZTtjx4Rs9cJDLbpEG8i7hPK"
        "swcFdsn6MWwINP+Nwmw4AEPpVJevUEvRQbqVMVoLlw==";

/*
 * Checkup routine
 */
int mbedtls_base64_self_test( int verbose )
{
    size_t len;
    const unsigned char *src;
    unsigned char buffer[128];

    if( verbose != 0 )
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "  Base64 encoding test: ");

    src = base64_test_dec;

    if( mbedtls_base64_encode( buffer, sizeof( buffer ), &len, src, 64 ) != 0 ||
        memcmp( base64_test_enc, buffer, 88 ) != 0 )
    {
        if( verbose != 0 )
            __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "failed");

        return( 1 );
    }

    if( verbose != 0 )
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "passed %s \n  Base64 decoding test: ",buffer);

    src = base64_test_enc;

    if( mbedtls_base64_decode( buffer, sizeof( buffer ), &len, src, 88 ) != 0 ||
        memcmp( base64_test_dec, buffer, 64 ) != 0 )
    {
        if( verbose != 0 )
            __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "failed\n");

        return( 1 );
    }

    if( verbose != 0 )
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "passed %d \n",sizeof( buffer ));

    return( 0 );
}


static const unsigned char aes_test_ecb_dec[3][16] =
        {
                { 0x44, 0x41, 0x6A, 0xC2, 0xD1, 0xF5, 0x3C, 0x58,
                        0x33, 0x03, 0x91, 0x7E, 0x6B, 0xE9, 0xEB, 0xE0 },
                { 0x48, 0xE3, 0x1E, 0x9E, 0x25, 0x67, 0x18, 0xF2,
                        0x92, 0x29, 0x31, 0x9C, 0x19, 0xF1, 0x5B, 0xA4 },
                { 0x05, 0x8C, 0xCF, 0xFD, 0xBB, 0xCB, 0x38, 0x2D,
                        0x1F, 0x6F, 0x56, 0x58, 0x5D, 0x8A, 0x4A, 0xDE }
        };

static const unsigned char aes_test_ecb_enc[3][16] =
        {
                { 0xC3, 0x4C, 0x05, 0x2C, 0xC0, 0xDA, 0x8D, 0x73,
                        0x45, 0x1A, 0xFE, 0x5F, 0x03, 0xBE, 0x29, 0x7F },
                { 0xF3, 0xF6, 0x75, 0x2A, 0xE8, 0xD7, 0x83, 0x11,
                        0x38, 0xF0, 0x41, 0x56, 0x06, 0x31, 0xB1, 0x14 },
                { 0x8B, 0x79, 0xEE, 0xCC, 0x93, 0xA0, 0xEE, 0x5D,
                        0xFF, 0x30, 0xB4, 0xEA, 0x21, 0x63, 0x6D, 0xA4 }
        };

int hex2_string(unsigned char *src,int srclen,char *des,int deslen) {
    unsigned char Hb;
    unsigned char Lb;

    if (deslen < srclen * 2) return 0;
    memset(des, 0, deslen);

    for (int i = 0; i < srclen;i++) {
        Hb = (src[i] & 0xF0) >> 4; // 高位
        if (Hb <= 9) Hb += 0x30; // 0~9
        else if (Hb >= 10 && Hb <= 15) Hb = Hb - 10 + 'a'; // 10~15
        else return 0;

        Lb = src[i] & 0x0F; // 低位
        if (Lb <= 9) Lb += 0x30;
        else if (Lb >= 10 && Lb <= 15) Lb = Lb - 10 + 'a';
        else return 0;

        des[i * 2] = Hb;
        des[i * 2 + 1] = Lb;
    }
    return 1;
}
int mbedtls_aes_ecb_self_test( int verbose )
{
    int ret = 0, i, j, u, mode;
    unsigned int keybits;
    unsigned char key[32];
    unsigned char buf[16];
    char des[33];

    const unsigned char *aes_tests;
    mbedtls_aes_context ctx;

    memset(des,0, 33);
    memset(key, 0, 32 );
    mbedtls_aes_init( &ctx );

    /*
     * ECB mode
     */
    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        keybits = 128 + u * 64;
        mode = i & 1;

        if( verbose != 0 )
            __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "  AES-ECB-%3u (%s): ",keybits,( mode == MBEDTLS_AES_DECRYPT ) ? "dec" : "enc");

        memset( buf, 0, 16 );

        if( mode == MBEDTLS_AES_DECRYPT )
        {
            ret = mbedtls_aes_setkey_dec( &ctx, key, keybits );
            aes_tests = aes_test_ecb_dec[u];
        }
        else
        {
            ret = mbedtls_aes_setkey_enc( &ctx, key, keybits );
            aes_tests = aes_test_ecb_enc[u];
        }

        /*
          AES-192 is an optional feature that may be unavailable when
          there is an alternative underlying implementation i.e. when
          MBEDTLS_AES_ALT is defined.
         */
        if( ret == MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED && keybits == 192 )
        {
            __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "skipped\n");
            continue;
        }
        else if( ret != 0 )
        {
            goto exit;
        }
        memset(des,0, 33);
        hex2_string(const_cast<unsigned char *>(buf), 16, des, sizeof(des));
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "aes_tests: %s ",des);

        for( j = 0; j < 10000; j++ )
        {
            ret = mbedtls_aes_crypt_ecb( &ctx, mode, buf, buf );
            if( ret != 0 )
                goto exit;
        }
        memset(des,0, 33);
        hex2_string(const_cast<unsigned char *>(buf), 16, des, sizeof(des));
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "aes_tests: %s ",des);
        if( memcmp( buf, aes_tests, 16 ) != 0 )
        {
            ret = 1;
            goto exit;
        }

        if( verbose != 0 ){
            /*memset(des,0, 33);
            hex2_string(const_cast<unsigned char *>(aes_tests), 16, des, sizeof(des));
            __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "aes_tests: %s ",des);
            memset(des,0, 33);
            hex2_string(const_cast<unsigned char *>(buf), 16, des, sizeof(des));
            __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "buf: %s ",des);*/
            __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "passed\n");
        }
    }

    ret = 0;
exit:
    if( ret != 0 && verbose != 0 )
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "failed\n");

    mbedtls_aes_free( &ctx );

    return( ret );
}


#define ECBBLOCK_SIZE 16
char ecb_key[20] = {0};

int aes_ecb_encryption(unsigned char** dst,const char *key, const char *src,size_t slen) {
    char des[33];
    int ret = -1;

    size_t ecb_time, pt_remain,dlen;
    u_char pt_cover;

    u_char * tmp_buf;
    mbedtls_aes_context ctx;

    size_t block_len;
    memset(ecb_key, 0,sizeof(ecb_key));

    mbedtls_aes_init( &ctx );

    strcpy(ecb_key, key);

    ecb_time = slen / ECBBLOCK_SIZE;
    pt_remain = slen % ECBBLOCK_SIZE;
    pt_cover = ECBBLOCK_SIZE - pt_remain;
    ret = mbedtls_aes_setkey_enc(&ctx, reinterpret_cast<const unsigned char *>(ecb_key), 128 );

    if (ret != 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "aes_ecb init failed.");
        return ret;
    }

    if(pt_remain != 0){
        ecb_time++;
    }
    block_len = ecb_time * ECBBLOCK_SIZE;
    tmp_buf = (u_char *)malloc(sizeof(u_char) * (block_len));
    memset(tmp_buf,0,block_len);
    for (size_t i = 0; i < block_len; i++) {
        if(i>=slen) tmp_buf[i] = pt_cover;
        else tmp_buf[i] = src[i];
    }
    for(int j = 0; j < ecb_time; j++){
        ret = mbedtls_aes_crypt_ecb(&ctx, 1, &tmp_buf[16 * j], &tmp_buf[16 * j]);
    }
    tmp_buf[block_len] = '\0';

    int base_len = ceil((float)block_len / 3) * 4 +1;
    u_char *base64_buf = (u_char *)malloc(sizeof(u_char) * (base_len));
    memset(base64_buf, 0, base_len);

    ret = mbedtls_base64_encode(base64_buf, base_len, &dlen, tmp_buf, block_len);
    *dst = base64_buf;
    if(tmp_buf != nullptr){
        free(tmp_buf);
    }
    if(ret == 0){
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "encode success base64_len: %d",base_len);
    }
    mbedtls_aes_free( &ctx );
    /*memset(des,0, 33);
    hex2_string(const_cast<unsigned char *>(ecb_outbuf), 16, des, sizeof(des));
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "buf: %s ",des);*/
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "buf: %skey: %s etlen: %d ptlen: %d slen: %d", base64_buf, ecb_key, dlen, block_len, slen);
    return ret==0 ? dlen : -1;
}

int aes_ecb_decryption(unsigned char** dst, const char *key, const unsigned char *src, size_t slen) {
    unsigned char * tmp_buf;
    mbedtls_aes_context ctx;
    int ecb_time;
    size_t len;
    int ret = -1;
    memset(ecb_key, 0,sizeof(ecb_key));

    strcpy(ecb_key, key);
    int buf_len = ((slen / 4) * 3) + 1;
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "buf_len: %d slen: %d",buf_len,slen);
    ecb_time = slen / ECBBLOCK_SIZE;
    ret = mbedtls_aes_setkey_dec(&ctx, reinterpret_cast<const unsigned char *>(ecb_key), 128);
    tmp_buf = (unsigned char *)malloc(sizeof(char) * buf_len);
    memset(tmp_buf,0,buf_len);
    mbedtls_base64_decode(tmp_buf, buf_len, &len, src, slen );

    if (ret != 0) {
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "aes_ecb init failed.");
        return ret;
    }

    if((slen < ECBBLOCK_SIZE) && (slen != 0)) {
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "[decrypt] pt_len <16 and >0 return -1");
        return ret;
    }
    else{
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "start-->");
        for(int j = 0; j < ecb_time && (j*16 < len); j++){
            ret = mbedtls_aes_crypt_ecb(&ctx, 0, &tmp_buf[16 * j], &tmp_buf[16 * j]);
        }
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "end-->");
        *dst = tmp_buf;
    }
    mbedtls_aes_free( &ctx );
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TGA, "etlen: %d buf: %s \n et %s",len, src, tmp_buf);

    return ret==0 ? len : -1;
}

#endif //TESTDEMO_MYAES_H