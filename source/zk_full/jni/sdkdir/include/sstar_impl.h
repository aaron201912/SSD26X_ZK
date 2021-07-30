/*
 *
 *  SStar License
 *
 *  internal implementation
 *
 *
 *
 */


#ifndef _SSTAR_KWS_IMPL_H
#define _SSTAR_KWS_IMPL_H

#ifdef __cplusplus
extern    "C"
{
#endif


#define FRAME_LEN      400     // uit is frame, 25ms
#define FRAME_LENA     512     // align frame
#define FRAME_SHIFT    160     // shift 10ms
#define FRAME_CONCAT   240     // sample for concat

#define FEATURE_MFCC   0
#define FEATURE_LMFB   1

#define FEATURE_PREEM_COEFF	0.97

#define LFREQ2MEL      20
#define HFREQ2MEL      8000

#define FBANK_NUM      80
#define MFCC_NUM       80
#define MFCC_DEC_BITS  0       // actmax 128

#define NN_DNN         0
#define NN_DSCNN       1

#define IN_FEATURE     FBANK_NUM
#define IN_TIME        140      // 1.4s, 25ms frame, 10ms shift
#define IN_DIM         IN_TIME*IN_FEATURE
#define KWS_OUT_DIM        (31)       // define @cmdword[] sli, unk, nihao/tianmao
#define AED_OUT_DIM        (3)       // define @cmdword[] sli, unk, nihao/tianmao

#ifdef __cplusplus
}
#endif

#endif


