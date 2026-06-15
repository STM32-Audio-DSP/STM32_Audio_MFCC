**Overview**
- **Purpose**: Explain the signal-processing math behind the MFCC code in [Src/mfcc.c](Src/mfcc.c#L1-L400) and how each function implements the steps used to compute Mel-Frequency Cepstral Coefficients (MFCCs).

**MFCC Pipeline (high level)**
- Step-by-step pipeline (explicit):

- **1. Framing**
  - Divide the input PCM stream into overlapping frames of length `N = MFCC_FFT_SIZE`.
  - Frames start every `hop = MFCC_HOP_SIZE` samples; typical overlap = `N - hop`.
  - Number of frames in a buffer of length `L` (if `L >= N`):

    $$\mathrm{frames} = 1 + \left\lfloor\dfrac{L - N}{\mathrm{hop}}\right\rfloor$$

- **2. Windowing**
  - Multiply each frame `x[n]` by a precomputed window `w[n]` (Hamming) to form `x_w[n] = x[n]*w[n]`.

- **3. FFT (RFFT)**
  - Compute a real FFT of `x_w` to obtain packed complex spectrum `X[k]` for `k = 0..N/2`.

- **4. Power spectrum**
  - Compute per-bin power `P[k] = |X[k]|^2` for `k = 0..N/2`.

- **5. Mel filterbank**
  - Apply `M = MFCC_MEL_BANDS` triangular filters `H_m[k]` to `P[k]` and compute band energies

    $$E_m = \sum_{k=0}^{N/2} P[k]\,H_m[k], \quad m = 0..M-1$$

- **6. Log compression**
  - Compute `L_m = \log(E_m + \varepsilon)` element-wise to stabilize dynamic range (`\varepsilon = 1\times10^{-12}`).

- **7. DCT-II → MFCCs**
  - Project `L` into cepstral space with `K = MFCC_COEFFS` coefficients via

    $$c_k = \sum_{n=0}^{M-1} L_n \cos\left(\dfrac{\pi (k+0.5)n}{M}\right),\quad k=0..K-1$$

- **8. Aggregate/Output**
  - Per-frame MFCC vectors are accumulated and averaged over the processed frames in the buffer; the final averaged vector is written to `mfcc_output`.

**Windowing**
**Windowing**
- Formula (discrete):

  $$w[n] = 0.54 - 0.46\,\cos\left(\dfrac{2\pi n}{N-1}\right)$$

- Meaning and usage:
  - `N` is the FFT size (`MFCC_FFT_SIZE`).
  - `n` is the sample index inside the frame, `n = 0, 1, ..., N-1`.
  - Multiply the raw frame `x[n]` by `w[n]` element-wise before the FFT: `x_w[n] = x[n] * w[n]`.
  - Purpose: the Hamming window tapers the frame edges to reduce spectral leakage when computing the FFT.

**FFT and Power Spectrum**
- The code initializes CMSIS RFFT via `arm_rfft_fast_init_f32(&rfft, N)` and runs the RFFT with `arm_rfft_fast_f32(&rfft, fft_in, fft_out, 0)`.
- For a real-input RFFT the library packs the complex FFT output into `fft_out` with the following layout (used by the code):
  - `fft_out[0]` = real(X[0]) (DC)
  - `fft_out[1]` = real(X[N/2]) (Nyquist, for even N)
  - For k = 1..N/2-1: real(X[k]) = fft_out[2*k], imag(X[k]) = fft_out[2*k+1].

- After the RFFT the complex spectrum values X[k] (for k = 0..N/2) are packed into `fft_out` as listed above.

- Power spectrum (per-bin energy): for each frequency bin k compute

  $$P[k] = |X[k]|^2 = \big(\mathrm{Re}(X[k])\big)^2 + \big(\mathrm{Im}(X[k])\big)^2$$

  - For `k = 0` (DC): `P[0] = fft_out[0]^2` (since imaginary part is zero).
  - For `1 <= k <= N/2 - 1`: `Re(X[k]) = fft_out[2*k]`, `Im(X[k]) = fft_out[2*k+1]`, so compute `P[k] = fft_out[2*k]^2 + fft_out[2*k+1]^2`.
  - For `k = N/2` (Nyquist, when N is even): `P[N/2] = fft_out[1]^2`.

**Mel Scale and Filterbank Construction**
- The Mel scale maps linear frequency `f` (Hz) to Mel units `m`:

  $$m = 2595\,\log_{10}\!\left(1 + \dfrac{f}{700}\right)$$

  - Inverse (Mel → Hz):

  $$f = 700\left(10^{m/2595} - 1\right)$$

- To get `M` triangular filters (`MFCC_MEL_BANDS`) the code:
  - Computes equally spaced points in Mel space between 0 and `sr/2` (Nyquist).
  - Converts those Mel points back to Hz and maps Hz to FFT bin indices via

    $$\mathrm{bin} = \dfrac{f\cdot N}{\mathrm{sr}}$$

  - Constructs triangular filters so each filter `H_m[k]` rises linearly from 0 to 1 between `bin[m-1]` and `bin[m]`, then falls to 0 between `bin[m]` and `bin[m+1]`.

- Filter energy (per mel band) is the dot product of the power spectrum with the filter:

  $$E_m = \sum_{k=0}^{N/2} P[k] \cdot H_m[k]$$

**Log Compression**
- Apply natural log to the filter energies to compress dynamic range and stabilize distributions:

  $$L_m = \log\big(E_m + \varepsilon\big)$$

  where `\varepsilon = 1e-12` in the code prevents taking log(0).

**DCT (Type II) — cepstral projection**
- DCT-II formula (clear index ranges):

  $$c_k = \sum_{n=0}^{M-1} L_n \,\cos\left(\dfrac{\pi (k + 0.5) n}{M}\right), \quad k = 0,1,\dots,K-1$$

  - `M` = number of mel filters (`MFCC_MEL_BANDS`).
  - `K` = number of cepstral coefficients (`MFCC_COEFFS`).
  - `build_dct_matrix()` precomputes

  $$D[k,n] = \mathrm{scale}\;\cos\left(\dfrac{(k+0.5)\pi n}{M}\right),\quad \mathrm{scale}=\sqrt{\dfrac{2}{M}}$$

  - MFCC vector `c` is `c = D * L` (matrix multiply). Some implementations scale the `k=0` term differently; this code uses the uniform `scale` above.

**Framing, hop size, and aggregation**
- Frame extraction and counting:
  - Frames start at sample indices `0, hop, 2*hop, ...` while `start + N <= length`.
  - If `length < N` the buffer yields no frames (skip processing).
  - For each valid buffer the code computes

    $$\mathrm{frames} = 1 + \left\lfloor\dfrac{\mathrm{length} - N}{\mathrm{hop}}\right\rfloor$$

- Accumulation and averaging:
  - Each frame's MFCC vector `c^{(i)}` is added to an accumulator `mfcc_acc` (element-wise).
  - After all frames are processed the averaged MFCC is

    $$\overline{c} = \dfrac{1}{\mathrm{frames}}\sum_{i=0}^{\mathrm{frames}-1} c^{(i)}$$

  - The code writes `mfcc_output[j] = mfcc_acc[j] / frames` for `j = 0..K-1`.

**Implementation Notes (mapping code → math)**
- `MFCC_Init(sample_rate)`
  - Calls `arm_rfft_fast_init_f32(&rfft, MFCC_FFT_SIZE)` to initialize the RFFT plan for length $N$.
  - Builds the Hamming window in `build_hamming()` using the Hamming formula.
  - Builds mel filters in `build_mel_filters()` and the DCT basis in `build_dct_matrix()`.
- `MFCC_ProcessBuffer(pcm, length)`
  - Iterates frames (step `MFCC_HOP_SIZE`). For each frame:
    - Applies the precomputed `window[n]` to samples.
    - Calls `arm_rfft_fast_f32(&rfft, fft_in, fft_out, 0)`.
    - Forms `power_spectrum` using the RFFT packing rules.
    - Computes `mel_energies[m]` = `log(sum(P[k]*mel_filters[m][k]) + eps)`.
    - Computes MFCC via multiplication with the DCT matrix.
    - Accumulates per-frame MFCCs and averages at the end to produce `mfcc_output`.
Additional implementation details and mapping:
  - `build_hamming()` fills `window[n]` for `n=0..N-1` using the Hamming formula; the implementation uses `arm_cos_f32` for efficiency when available.
  - `build_mel_filters()` computes `M+2` mel bin boundaries (including 0 and Nyquist), converts them to FFT bin indices, and constructs `H_m[k]` as linear ramps between those indices.
  - `build_dct_matrix()` precomputes `D[k,n]` with `scale = sqrt(2.0f / M)` so that the MFCC computation is a single matrix-vector multiply: `c = D * L`.
  - `power_spectrum` is a length `N/2+1` array computed from the packed `fft_out` returned by `arm_rfft_fast_f32`.
  - Numerical stability: log uses `eps = 1e-12f`; if filters produce tiny energies consider applying a floor or per-band normalization.

**Practical / Real-time considerations**

- CPU and latency
  - `arm_rfft_fast_f32` and the subsequent dot-products dominate runtime; cost scales with `N log N + M*N/2 + K*M` per frame (rough complexity estimate).
  - Avoid performing MFCC processing in a high-priority IRQ: copy DMA data to a buffer and signal a background task/thread to run `MFCC_ProcessBuffer()`.

- Memory and buffer management
  - Keep a small pool of buffers for DMA halves/wholes to avoid blocking the audio capture.
  - Preallocate `power_spectrum`, `mel_energies`, and temporary FFT buffers to avoid dynamic allocation at runtime.

- Numerical robustness
  - Use `eps = 1e-12f` before log to prevent -Inf; consider a slightly larger floor if numerical underflow occurs on very quiet inputs.

- Verification tips
  - Add a short UART or ITM dump of `mfcc_output` after processing a buffer to validate values on the host.
  - Start with a short frame size (e.g., 256) to iterate faster during debugging; switch to production `N` later.

