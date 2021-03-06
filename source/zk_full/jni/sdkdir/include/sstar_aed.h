/*
 *
 *  SStar License
 *
 *
 *
 *
 *
 */

#ifndef _SSTAR_AED_H
#define _SSTAR_AED_H

#ifdef __cplusplus
extern    "C"
{
#endif
#if 0
#define SRATE     16000
#define CHUNK     16       // ms
#define MAX_BUF   512*16*2   // ms, byte

#define E_KWS     1
#define E_SOURCE  10
#define E_FEATURE 20
#define E_NN      30
#define E_DNN     40
#define E_DSCNN   50
#endif

typedef struct AEDInit {
    int sample_rate;
    int chunk_len;          // unit is byte
    int nn_type;
    float threshold[256];
} AEDInit_s;

// init internal buf
int aed_init(AEDInit_s *init);

// get support command
const char **aed_getcmd(int *num);

// send chunk buf to kws, return word index
int aed_senddata(short *buf, int len);

int aed_predict(int *ret_index);


// free internal buffer alloc by init
int aed_free(void);

int aed_test();

float cosine_distance(float *v1, float *v2, int len);
int compute_distance(float *threshold);

#ifdef __cplusplus
}
#endif

#endif


