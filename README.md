## <b>Templates_TrustZoneDisabled Example Description</b>

This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application when security is not enabled **(TZEN=0)**.

This project is targeted to run on STM32U585xx devices on **B-U585I-IOT02A** boards from STMicroelectronics.

Path of the project under the STM32CubeU5 Cube Firmware: STM32Cube_FW_U5_V1.8.0\Projects\B-U585I-IOT02A\Templates\TrustZoneDisabled

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 160 Mhz.

The template project calls also CACHE_Enable() function in order to enable the Instruction
and Data Caches. This function is provided as template implementation that the User may
integrate in his application in order to enhance the performance.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Reference, Template, TrustZone disabled.

### <b>Directory contents</b> 

  - Templates/TrustZoneDisabled/Src/main.c                  Main program
  - Templates/TrustZoneDisabled/Src/system_stm32u5xx.c      STM32U5xx system clock configuration file
  - Templates/TrustZoneDisabled/Src/stm32u5xx_it.c          Interrupt handlers
  - Templates/TrustZoneDisabled/Src/stm32u5xx_hal_msp.c     HAL MSP module
  - Templates/TrustZoneDisabled/Inc/main.h                  Main program header file
  - Templates/TrustZoneDisabled/Inc/aps6408_conf.h          aps6408 Configuration file
  - Templates/TrustZoneDisabled/Inc/b_u585i_iot02_conf.h    BSP Configuration file
  - Templates/TrustZoneDisabled/Inc/stm32u5xx_hal_conf.h    HAL Configuration file
  - Templates/TrustZoneDisabled/Inc/stm32u5xx_it.h          Interrupt handlers header file
  - Templates/TrustZoneDisabled/Inc/mx25lm51245g_conf.h     BSP Component Configuration file

### <b>Hardware and Software environment</b> 

  - This template runs on STM32U585xx devices without security enabled (TZEN=0).
    
  - This template has been tested with STMicroelectronics B-U585I-IOT02A (MB1551)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

## FFT / Audio (MFCC) Example

This repository adapts the TrustZone-disabled template into an audio-processing example. It demonstrates capturing audio on the target U5 IoT board, computing FFTs and extracting MFCC features from audio frames, and reporting results for analysis (for example via UART logs or debug output). The concrete implementation lives under the `Src/` and `Inc/` folders — inspect `Src/` for audio capture, signal-processing, and output routines.

Use this example to learn and experiment with on-device audio feature extraction, feature visualization, and downstream ML pipelines.

## Collaborating: clone, commit, push, and maintain

Workflows for students and contributors:

- Clone the repository:

```bash
git clone https://github.com/EmbedDevWhiz/STM32_Audio_MFCC.git
cd STM32_Audio_MFCC
```

- Create a branch for your work (do not push directly to `main`!!!):

```bash
git checkout -b feature/<short-description>
```

- Make changes, stage, and commit following the commit message convention below, then push:

```bash
git add <files>
git commit -m "feat(audio): add FFT pipeline for microphone input"
git push -u origin feature/<short-description>
```

- Open a Pull Request on GitHub from your branch into `main`. Keep PRs small and focused.

- Before merging, update your branch from `main` and resolve conflicts:

```bash
git fetch origin
git checkout feature/<short-description>
git rebase origin/main
# resolve any conflicts, then:
git push --force-with-lease
```

## Commit message convention (recommended)

Use a simple Conventional Commits format to make history readable and automate changelogs:

`<type>(<scope>): <short summary>`

Common types:
- `feat`: a new feature
- `fix`: a bug fix
- `docs`: documentation only changes
- `style`: formatting, missing semi-colons, etc; no code change
- `refactor`: code change that neither fixes a bug nor adds a feature
- `perf`: performance improvements
- `test`: adding or fixing tests
- `chore`: build process or auxiliary tools

Examples:
- `feat(audio): add FFT-based feature extractor`
- `fix(uart): correct UART baud rate calculation`
- `docs: update README with collaboration instructions`

Short PR description template:


Summary: One-sentence summary of what changed

Details: Short paragraph explaining why the change was made and any implementation notes.

Testing: How the change was tested (board model, commands used, logs to review).

