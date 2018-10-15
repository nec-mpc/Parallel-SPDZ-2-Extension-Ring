//
// Created by liork on 30/05/16.
//

#ifndef SCAPIPRG_PRG_HPP
#define SCAPIPRG_PRG_HPP

#include <cstdint>
#include <string.h>
#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include "aes_locl.h"

extern unsigned int OPENSSL_ia32cap_P[];
# define AESNI_CAPABLE (OPENSSL_ia32cap_P[1]&(1<<(57-32)))

using namespace std;

typedef unsigned char byte;

//#define DEFAULT_CACHE_SIZE 400000 //11*8
#define DEFAULT_CACHE_SIZE 10000

// #define USECXXPROF
// #include <cxxprof_static/CxxProf.h>

class PRG
{

public:
    PRG();
    PRG(byte *key);
    PRG(byte *key, byte *iv,int cacheSize=DEFAULT_CACHE_SIZE);
    ~PRG();
   
    uint32_t getRandom()
    {
    	switch (m_idx)
    	{
        case 0:
            {
            	m_pIdx = (uint32_t*) getRandomBytes();
            	m_u1 = *m_pIdx;
            	m_pIdx++;
            	m_idx++;
            	return m_u1;
            }
            case 1:
            {
            	m_u2 = *m_pIdx;
            	m_pIdx++;
            	m_idx++;
            	return m_u2;
            }

            case 2:
            {
            	m_u3 = *m_pIdx;
            	m_pIdx++;
            	m_idx++;
            	return m_u3;
            }

            case 3:
            {
            	m_u4 = *m_pIdx;
            	m_idx = 0;
            	return m_u4;
            }
        }
        return 0;
    }

    uint8_t getRandomByte()
    {
    	switch (m_idx)
    	{
        case 0:
            {
            	m_bIdx = (uint8_t*) getRandomBytes();
            	m_b1 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b1;
            }
            case 1:
            {
            	m_b2 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b2;
            }

            case 2:
            {
            	m_b3 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b3;
            }
            case 3:
            {
            	m_b4 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b4;
            }
            case 4:
            {
            	m_b5 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b5;
            }
            case 5:
            {
            	m_b6 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b6;
            }
            case 6:
            {
            	m_b7 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b7;
            }
            case 7:
            {
            	m_b8 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b8;
            }
            case 8:
            {
            	m_b9 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b9;
            }
            case 9:
            {
            	m_b10 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b10;
            }
            case 10:
            {
            	m_b11 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b11;
            }
            case 11:
            {
            	m_b12 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b12;
            }
            case 12:
            {
            	m_b13 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b13;
            }
            case 13:
            {
            	m_b14 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b14;
            }
            case 14:
            {
            	m_b15 = *m_bIdx;
            	m_bIdx++;
            	m_idx++;
            	return m_b15;
            }
            case 15:
            {
            	m_b16 = *m_bIdx;
            	m_idx = 0;
            	return m_b16;
            }
        }
        return 0;
    }


    uint64_t getRandomLong()
    {
    	switch (m_idx)
    	{
        case 0:
            {
            	m_lIdx = (uint64_t*) getRandomBytes();
            	m_l1 = *m_lIdx;
            	m_lIdx++;
            	m_idx++;
            	return m_l1;
            }
            case 1:
            {
            	m_l2 = *m_lIdx;
            	m_idx = 0;
            	return m_l2;
            }
        }
        return 0;
    }


private:
    void checkAESNI();
     byte *getRandomBytes()
     {
         if(m_cachedRandomsIdx==m_cacheSize)
         {
             //cout << "PREPARE" << endl;
             prepare(0);
         }
         byte *ret = m_cachedRandoms + m_cachedRandomsIdx*16;
         m_cachedRandomsIdx++;

         return ret;

     }

     void prepare(int isPlanned = 1);

    static unsigned char m_defaultkey[16];
    static unsigned char m_defaultiv[16];

    EVP_CIPHER_CTX m_enc;

    const byte* m_key;

    int m_cacheSize;

    byte *m_cachedRandoms;

    const byte *m_iv;

    byte* m_ctr;

    unsigned long m_ctr_count = 0;

    int m_cachedRandomsIdx;

    int m_idx;
    uint32_t *m_pIdx;
    uint32_t m_u1;
    uint32_t m_u2;
    uint32_t m_u3;
    uint32_t m_u4;

    uint8_t *m_bIdx;
    uint8_t m_b1;
    uint8_t m_b2;
    uint8_t m_b3;
    uint8_t m_b4;
    uint8_t m_b5;
    uint8_t m_b6;
    uint8_t m_b7;
    uint8_t m_b8;
    uint8_t m_b9;
    uint8_t m_b10;
    uint8_t m_b11;
    uint8_t m_b12;
    uint8_t m_b13;
    uint8_t m_b14;
    uint8_t m_b15;
    uint8_t m_b16;

    uint64_t *m_lIdx;
    uint64_t m_l1;
    uint64_t m_l2;
};

class UnsupportAESNIException : public exception
{

public:
    virtual const char* what() const throw();
};

#endif //SCAPIPRG_PRG_HPP
