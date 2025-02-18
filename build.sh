#!/bin/bash
#
# Copyright (c) 2025 EdgeImpulse Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the disclaimer
# below) provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
#
#   * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
#   * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
# THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

set -e

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

for i in "$@"; do
  case $i in
    -b|--build)
      BUILD=1
      shift # past argument
      ;;
    -c|--clean)
      CLEAN=1
      shift # past argument
      ;;
    -f|--flash)
      FLASH=1
      shift # past argument
      ;;
    *)
      shift # past argument
      ;;
  esac
done

if [ ! -z ${CLEAN} ];
then
    cd ${SCRIPTPATH}/EPII_CM55M_APP_S
    make clean
    if [ $? -ne 0 ]; then
        echo "Clean error"
        exit 1
    fi
fi

if [ ! -z ${BUILD} ];
then
    SRC_FILE=output_case1_sec_wlcsp/output.img
    MAX_SRC_FILE_SIZE_MB=1
    MAX_SRC_FILE_SIZE=$((MAX_SRC_FILE_SIZE_MB*1024*1024))
    cd ${SCRIPTPATH}/EPII_CM55M_APP_S
    make --silent -j ${MAKE_JOBS:-$(nproc)}
    cd ${SCRIPTPATH}/we2_image_gen_local/
    cp ${SCRIPTPATH}/EPII_CM55M_APP_S/obj_epii_evb_icv30_bdv10/gnu_epii_evb_WLCSP65/EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf input_case1_secboot/
    ./we2_local_image_gen project_case1_blp_wlcsp.json
    SRC_FILE_SIZE=$(stat --printf="%s" $SRC_FILE)
    if [ "$SRC_FILE_SIZE" -gt "$MAX_SRC_FILE_SIZE" ]; then
        echo "Error: Firmware image size is $SRC_FILE_SIZE bytes, which exceeds the limit ($MAX_SRC_FILE_SIZE bytes)"
        exit 1
    fi
    cp output_case1_sec_wlcsp/output.img ${SCRIPTPATH}/firmware.img
fi

if [ ! -z ${FLASH} ];
then
    cd ${SCRIPTPATH}
    himax-flash-tool -d WiseEye2 -f firmware.img
fi