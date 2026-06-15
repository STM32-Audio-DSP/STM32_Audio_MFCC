STM32 Audio MFCC
=================

This folder contains a standalone copy of the MFCC implementation extracted from the ADF_AudioSoundDetector example. It depends on CMSIS-DSP (arm_math.h) for RFFT and some math helpers. Copy `mfcc.c` and `mfcc.h` into your STM32 project and add the required CMSIS-DSP sources (RFFT/FFT, CommonTables, SupportFunctions, FastMathFunctions) and include path `Drivers/CMSIS/DSP/Include`.

Files included:
- mfcc.c
- mfcc.h

Quick push instructions
----------------------
Run the following commands locally to push into the target repository as a new folder `stm32_mfcc` (replace remote and branch names as needed):

```bash
git clone https://github.com/STM32-Audio-DSP/STM32_Audio_MFCC.git
cd STM32_Audio_MFCC
mkdir stm32_mfcc
cp "<path-to>/Export/STM32_Audio_MFCC/mfcc.c" stm32_mfcc/
cp "<path-to>/Export/STM32_Audio_MFCC/mfcc.h" stm32_mfcc/
cp "<path-to>/Export/STM32_Audio_MFCC/README.md" stm32_mfcc/

git add stm32_mfcc
git commit -m "Add STM32 MFCC extractor (CMSIS-DSP)"
git push origin main
```

Notes:
- Replace `<path-to>` with the absolute path to this folder on your machine.
- Ensure the target repo branch (`main`) exists and you have push access.
