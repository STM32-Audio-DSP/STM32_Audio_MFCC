/* mfcc.c - lightweight MFCC extractor using CMSIS-DSP
 * Source copied from: Projects/.../ADF_AudioSoundDetector/Src/mfcc.c
 * Purpose: standalone copy for the STM32_Audio_MFCC export repository.
 */

#include "mfcc.h"
#include "arm_math.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

float mfcc_output[MFCC_COEFFS];

/* Internal buffers and tables */
static arm_rfft_fast_instance_f32 rfft;
static float window[MFCC_FFT_SIZE];
static float fft_in[MFCC_FFT_SIZE];
static float fft_out[MFCC_FFT_SIZE];
static float power_spectrum[MFCC_FFT_SIZE/2 + 1];
static float mel_filters[MFCC_MEL_BANDS][MFCC_FFT_SIZE/2 + 1];
static float dct_matrix[MFCC_COEFFS][MFCC_MEL_BANDS];
static uint32_t sr = 16000;

/* Helpers: frequency <-> mel conversions */
static float hz_to_mel(float f) { return 2595.0f * log10f(1.0f + f/700.0f); }
static float mel_to_hz(float m) { return 700.0f * (powf(10.0f, m/2595.0f) - 1.0f); }

static void build_hamming(void)
{
  for (int n = 0; n < MFCC_FFT_SIZE; ++n)
  {
    window[n] = 0.54f - 0.46f * arm_cos_f32((2.0f * PI * n) / (MFCC_FFT_SIZE - 1));
  }
}

static void build_mel_filters(void)
{
  const int nfft = MFCC_FFT_SIZE;
  const int nbin = nfft/2 + 1;
  float low_mel = hz_to_mel(0.0f);
  float high_mel = hz_to_mel((float)sr/2.0f);
  for (int i = 0; i < MFCC_MEL_BANDS + 2; ++i)
  {
    float mel = low_mel + ((float)i / (MFCC_MEL_BANDS + 1)) * (high_mel - low_mel);
    float hz = mel_to_hz(mel);
    float binf = (hz * (float)nfft) / (float)sr;
    (void)binf; /* silence unused warning in this spot */
  }

  /* Build triangular filters */
  float bins[MFCC_MEL_BANDS + 2];
  for (int i = 0; i < MFCC_MEL_BANDS + 2; ++i)
  {
    float mel = low_mel + ((float)i / (MFCC_MEL_BANDS + 1)) * (high_mel - low_mel);
    float hz = mel_to_hz(mel);
    bins[i] = (hz * (float)MFCC_FFT_SIZE) / (float)sr;
  }

  /* zero filters */
  for (int m = 0; m < MFCC_MEL_BANDS; ++m)
    for (int k = 0; k < nbin; ++k)
      mel_filters[m][k] = 0.0f;

  for (int m = 1; m <= MFCC_MEL_BANDS; ++m)
  {
    float f_m_minus = bins[m-1];
    float f_m = bins[m];
    float f_m_plus = bins[m+1];
    for (int k = 0; k < nbin; ++k)
    {
      float fk = (float)k;
      if (fk >= f_m_minus && fk <= f_m)
      {
        mel_filters[m-1][k] = (fk - f_m_minus) / (f_m - f_m_minus + 1e-12f);
      }
      else if (fk >= f_m && fk <= f_m_plus)
      {
        mel_filters[m-1][k] = (f_m_plus - fk) / (f_m_plus - f_m + 1e-12f);
      }
    }
  }
}

static void build_dct_matrix(void)
{
  const float scale = sqrtf(2.0f / (float)MFCC_MEL_BANDS);
  for (int k = 0; k < MFCC_COEFFS; ++k)
  {
    for (int n = 0; n < MFCC_MEL_BANDS; ++n)
    {
      dct_matrix[k][n] = scale * cosf(((float)k + 0.5f) * (float)M_PI * ((float)n) / (float)MFCC_MEL_BANDS);
    }
  }
}

void MFCC_Init(uint32_t sample_rate)
{
  sr = sample_rate;
  arm_rfft_fast_init_f32(&rfft, MFCC_FFT_SIZE);
  build_hamming();
  build_mel_filters();
  build_dct_matrix();
  memset(mfcc_output, 0, sizeof(mfcc_output));
}

void MFCC_ProcessBuffer(const int16_t *pcm, uint32_t length)
{
  if (length < MFCC_FFT_SIZE) return;

  const int hop = MFCC_HOP_SIZE;
  const int nfft = MFCC_FFT_SIZE;
  const int nbin = nfft/2 + 1;
  float mfcc_acc[MFCC_COEFFS];
  memset(mfcc_acc, 0, sizeof(mfcc_acc));
  int frames = 0;

  for (int off = 0; off + nfft <= (int)length; off += hop)
  {
    for (int n = 0; n < nfft; ++n)
      fft_in[n] = (float)pcm[off + n] * window[n];

    arm_rfft_fast_f32(&rfft, fft_in, fft_out, 0);

    power_spectrum[0] = fft_out[0] * fft_out[0];
    for (int k = 1; k < nbin-1; ++k)
    {
      float re = fft_out[2*k];
      float im = fft_out[2*k+1];
      power_spectrum[k] = re*re + im*im;
    }
    power_spectrum[nbin-1] = fft_out[1]*fft_out[1];

    float mel_energies[MFCC_MEL_BANDS];
    for (int m = 0; m < MFCC_MEL_BANDS; ++m)
    {
      float acc = 0.0f;
      for (int k = 0; k < nbin; ++k)
        acc += power_spectrum[k] * mel_filters[m][k];
      mel_energies[m] = logf(acc + 1e-12f);
    }

    float mfcc_local[MFCC_COEFFS];
    for (int k = 0; k < MFCC_COEFFS; ++k)
    {
      float s = 0.0f;
      for (int n = 0; n < MFCC_MEL_BANDS; ++n)
        s += dct_matrix[k][n] * mel_energies[n];
      mfcc_local[k] = s;
      mfcc_acc[k] += s;
    }

    frames++;
  }

  if (frames > 0)
  {
    for (int k = 0; k < MFCC_COEFFS; ++k)
      mfcc_output[k] = mfcc_acc[k] / (float)frames;
  }
}
