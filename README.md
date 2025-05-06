# C Project: AArch64 Assembler, Emulator, and Piano Simulator Extension

## Overview

This project is a comprehensive C-based toolkit for AArch64 assembly, including:
- An assembler that translates AArch64 assembly code into binary.
- An emulator for running and testing AArch64 binaries.
- An extension: a "Super Fabulous Piano Simulator" for Raspberry Pi, allowing users to play C major notes, switch octaves, and adjust volume with a potentiometer.

## Project Structure

- `src/`: Core assembler and emulator source code.
  - `assemble.c`, `emulate.c`: Main entry points for assembler and emulator.
  - `assembles/`, `emulates/`: Modularized instruction handling (branch, data processing, single data transfer, special instructions, parser, utilities).
- `extension/`: Piano simulator source code (`main.c`).
- `programs/`: Example binaries and assembly programs.
- `doc/`: Documentation, including the project report (`Report.tex`).
- `testsuite/`: Automated test suite for assembler and emulator.

## Assembler

The assembler translates AArch64 assembly into binary, supporting arithmetic, branching, data transfer, and special instructions. It uses a two-pass approach:
1. Build a symbol table for labels.
2. Parse and encode instructions into binary.

## Emulator

The emulator executes AArch64 binaries, simulating the AArch64 pipeline and supporting the same instruction set as the assembler.

## Extension: Super Fabulous Piano Simulator

### Features

- Simulates C major notes (C, D, E, F, G, A, B).
- Switches between two octaves using dedicated buttons.
- Volume control via a potentiometer.
- Built for Raspberry Pi using the `pigpio` library.
- Physical interface: buttons for notes and octave switching, buzzer for sound output.

### How It Works

- Each button triggers a note; octave buttons switch between two octaves.
- The buzzer is toggled at the correct frequency to generate musical notes.
- The potentiometer adjusts the buzzer's volume.

### Hardware Requirements

- Raspberry Pi 3B+ (or compatible)
- 7 buttons (for notes)
- 2 buttons (for octave switching)
- 1 buzzer (speaker)
- 1 potentiometer (for volume)
- Jumper wires and breadboard

### Software Requirements

- C compiler (e.g., `gcc`)
- `pigpio` library (install with `sudo apt-get install pigpio`)

### Running the Piano Simulator

1. Wire the components as shown in `src/extension.png`.
2. Build and run `extension/main.c` on your Raspberry Pi:
   ```sh
   gcc main.c -lpigpio -o piano
   sudo ./piano
   ```

## Challenges

- GPIO port limitations and hardware quirks.
- Generating accurate frequencies for musical notes.
- Limited documentation for C-based GPIO libraries; switched from `WiringPi` to `pigpio`.

## Testing

- Staged testing: first with basic notes, then with volume control, and finally with octave switching.
- Debugged by listening for correct notes and adjusting hardware/software as needed.

## Authors

- Weijun Huang, Lihaomin Qiu, Molan Yang, Yifan Jiang

## License

This project is for educational purposes.
