/* The Clear BSD License
 *
 * Copyright (c) 2025 EdgeImpulse Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "flash_memory.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "spi_eeprom_comm.h"

/* Flash Information */
#define FLASH_TOTAL_SIZE        (0x01000000)    /* 16 MB */
#define FLASH_SECTOR_SIZE       (0x00001000)    /* 4 kB */
#define FLASH_PAGE_SIZE         (256)           /* 256 B */
#define FLASH_PROGRAM_UNIT      (0x01)          /* Minimum write size */
#define HX_CMSIS_FLASH0_ADDR    (0x00f00000)    /* Flash base address offset by 15 MB */
#define HX_CMSIS_FLASH0_SIZE    (0x00100000)    /* 1 MB */

static int32_t is_range_valid(uint32_t offset)
{
    int32_t rc = 0;

    if (offset > (HX_CMSIS_FLASH0_ADDR + HX_CMSIS_FLASH0_SIZE - 1)) {
        rc = -1;
    }

    return rc;
}

uint32_t EiFlashMemory::read_data(uint8_t *data, uint32_t address, uint32_t num_bytes)
{
    int32_t rc = 0;
    ei_printf("Reading %d bytes from address 0x%08x\n", num_bytes, address);
    // offset address by flash base address
    address += HX_CMSIS_FLASH0_ADDR;

    /* Check flash memory boundaries */
    rc = is_range_valid(address + num_bytes);
    if (rc != 0) {
        ei_printf("Flash memory read out of range\n");
        return 0;
    }

    rc = hx_lib_spi_eeprom_2read(USE_DW_SPI_MST_Q, address, (uint8_t *)data, num_bytes);

    if (rc != E_OK) {
        ei_printf("Failed to read from SPI EEPROM (%d)\n", rc);
        return 0;
    }

    return num_bytes;
}

uint32_t EiFlashMemory::write_data(const uint8_t *data, uint32_t address, uint32_t num_bytes)
{
    int32_t rc = 0;
    ei_printf("Writing %d bytes to address 0x%08x\n", num_bytes, address);
    address += HX_CMSIS_FLASH0_ADDR;

    rc  = is_range_valid(address + num_bytes);
    if(rc != 0) {
        ei_printf("Flash memory write out of range\n");
        return 0;
    }

    rc = hx_lib_spi_eeprom_write(USE_DW_SPI_MST_Q, address, (uint8_t *)data, num_bytes, 0);
    if (rc != E_OK) {
        ei_printf("Failed to write to SPI EEPROM (%d)\n", rc);
        return 0;
    }

    return num_bytes;
}

uint32_t EiFlashMemory::erase_data(uint32_t address, uint32_t num_bytes)
{
    int32_t rc = 0;
    ei_printf("Erasing %d bytes from address 0x%08x\n", num_bytes, address);
    address += HX_CMSIS_FLASH0_ADDR;
    uint32_t num_bytes_to_erase = num_bytes;

    // calculate address offset from block alignment
    uint16_t temp = address & (block_size - 1);
    if(temp != 0) {
        // address is not block aligned
        address -= temp;
        // we have to increase the number of bytes by the block offset bytes
        num_bytes_to_erase += temp;
    }

    // check if number of bytes to erase is multiple of block size
    temp = num_bytes_to_erase % block_size;
    if(temp != 0) {
        // if not, we have to increase the number of bytes to erase to full blocks
        num_bytes_to_erase += (block_size - temp);
    }

    rc  = is_range_valid(address);
    if (rc != 0) {
        ei_printf("Flash memory erase out of range\n");
        return 0;
    }

    rc = hx_lib_spi_eeprom_erase_sector(USE_DW_SPI_MST_Q, address, (FLASH_ERASE_SIZE_E)num_bytes_to_erase);
    if (rc != E_OK) {
        ei_printf("Failed to erase SPI EEPROM (%d)\n", rc);
        return 0;
    }

    return num_bytes;
}

EiFlashMemory::EiFlashMemory(uint32_t config_size):EiDeviceMemory(config_size, 0, HX_CMSIS_FLASH0_SIZE, FLASH_SECTOR_SIZE)
{
    int rc = hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    if (rc != E_OK) {
        ei_printf("Failed to open SPI EEPROM (%d)\n", rc);
    }
}
