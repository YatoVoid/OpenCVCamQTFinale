#ifndef NFC_DEVICE_HPP
#define NFC_DEVICE_HPP

#include <err.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <list>
#include <algorithm>
#include <nfc/nfc.h>
#include <nfc/nfc-types.h>

#include <nfc-utils.h>




#define MAX_DEVICE_COUNT 16
#define SAK_FLAG_ATS_SUPPORTED 0x20
#define MAX_FRAME_LEN 264
#define CASCADE_BIT 0x04


class nfc {
public:
    nfc();
    ~nfc();
    bool get_UID();
    static nfc_device *pnd;
    bool write_page(uint8_t page, uint8_t *data);
    std::vector<uint8_t> current_UID;
    uint8_t pwd[4] = {0x4D, 0xE9, 0xA2, 0xF7};
    uint8_t cfg1[4] = {0x81, 0x05, 0x00, 0x03};
    uint8_t pack[4] = {0x0A, 0x00, 0x00, 0x00};
    uint8_t cfg0[4] = {0x00, 0x00, 0x00, 0x00};


    uint8_t  abtPWAuth[7] = { 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t iPWD[4] = { 0x4D, 0xE9, 0xA2, 0xF7};
    uint8_t iPACK[2] = { 0x0A };

    bool card_init();


private:
    static void stop_polling(int sig);
    void print_vector(const std::vector<uint8_t>& vec);
    bool print_hex(const uint8_t* data, const size_t len);
    bool transmit_bytes(const uint8_t *pbtTx, const size_t szTx);
    bool transmit_bits(const uint8_t *pbtTx, const size_t szTxBits);
    bool ev1_pwd_auth();
    bool raw_mode_start();
    bool raw_mode_end();
    bool ev1_pwd_auth(uint8_t *pwd);


    bool quiet_output = false;
    bool force_rats = false;
    bool timed = false;
    bool iso_ats_supported = false;
    uint8_t abtRx[MAX_FRAME_LEN];
    int szRxBits;
    size_t szRx = sizeof(abtRx);
    uint8_t abtRawUid[12];
    uint8_t abtAtqa[2];
    uint8_t abtSak;
    uint8_t abtAts[MAX_FRAME_LEN];
    uint8_t szAts = 0;
    size_t szCL = 1; // Always start with Cascade Level 1 (CL1)

    static nfc_context *context;
    std::vector<uint8_t> prev_UID;
    std::list<std::vector<uint8_t>> list_of_vectors;

};

#endif // NFC_DEVICE_HPP
