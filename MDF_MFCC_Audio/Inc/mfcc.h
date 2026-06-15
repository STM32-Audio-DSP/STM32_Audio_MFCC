/* mfcc.h - simple MFCC extractor using CMSIS-DSP
 */
#ifndef MFCC_H
#define MFCC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MFCC_FFT_SIZE       512
#define MFCC_HOP_SIZE       256
#define MFCC_MEL_BANDS      40
#define MFCC_COEFFS         13

/* Public output buffer containing last computed MFCCs (length MFCC_COEFFS) */
extern float mfcc_output[MFCC_COEFFS];

/* Initialize MFCC engine. Call once before processing. */
void MFCC_Init(uint32_t sample_rate);

/* Process a buffer of int16 PCM samples (mono). length is number of samples. */
void MFCC_ProcessBuffer(const int16_t *pcm, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* MFCC_H */
