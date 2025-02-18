# Edge Impulse firmware for Seeed Grove Vision AI Module V2 (Himax WiseEye2)

[Edge Impulse](https://www.edgeimpulse.com) enables developers to create the next generation of intelligent device solutions with embedded Machine Learning. This repository contains the Edge Impulse firmware for the Seeed Grove Vision AI Module V2 (based on Himax WiseEye2 MCU). This device supports all Edge Impulse device features, including ingestion and inferencing.

> **Note:**  Do you just want to use this development board with Edge Impulse? No need to build this firmware. See the instructions [here](https://docs.edgeimpulse.com/docs/edge-ai-hardware/mcu-+-ai-accelerators/himax-seeed-grove-vision-ai-module-v2-wise-eye-2) for a prebuilt image and instructions.

## Development

The source code fo the Edge Impulse firmware can be found here: EPII_CM55M_APP_S/app/scenario_app/edge_impulse_firmware

## Building firmware (native)

1. Install required tools

* [Git](https://git-scm.com/downloads) - make sure `git` is in your PATH.
* [GNU ARM Embedded Toolchain 13.2.rel1](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads/13-2-rel1)

2. Clone this repository
3. In the repository root, run the build script
   ```
   ./build.sh --build
   ```
4. Your firmware should be ready in the `firmware.img` file in the root directory.

## Building firmware (Docker)

1. Install required tools

* [Docker Desktop](https://www.docker.com/products/docker-desktop/)

2. Clone this repository
3. Build Docker image (in the repository root)
   ```
   docker build -t himax-we2 .
   ```
4. Buld the firmware
   ```
   docker run --rm -v $PWD:/app himax-we2 ./build.sh --build
   ```
5. Your firmware should be ready in the `firmware.img` file in the root directory.

## Flashing the firmware

1. Install required tools

* [Python 3 + PIP](https://www.python.org/downloads/)
* [Node.js 20](https://nodejs.org/en/download/) or higher.

2. Install Python dependencies
   ```
   pip3 install -r xmodem/requirements.txt
   ```
3. Install Edge Impulse CLI tools
   ```
    npm install -g edge-impulse-cli
   ```
4. Connect your module (using USB Type C cable) and find it's port name
5. Flash firmware and model file
   ```
   python3 xmodem/xmodem_send.py --port=YOUR_BOARD_PORT_NAME --baudrate=921600 --protocol=xmodem --file=firmware.img --model="EPII_CM55M_APP_S/app/scenario_app/edge_impulse_firmware/ei-model/face-detection.tflite 0x200000 0x00000"
   ```
6. Run the impulse
   ```
   edge-impulse-run-impulse --debug
   ```
