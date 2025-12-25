#include "nfc_device.hpp"



nfc_device* nfc::pnd = nullptr;
nfc_context* nfc::context = nullptr;


nfc::nfc() {
    nfc_init(&context);
    if (context == NULL) {
        ERR("Unable to init libnfc (malloc)");
        exit(EXIT_FAILURE);
    }

    // Try to open the NFC reader
    pnd = nfc_open(context, NULL);
    if (pnd == NULL) {
        ERR("Error opening NFC reader");
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }

    // Initialise NFC device as "initiator"
    if (nfc_initiator_init(pnd) < 0) {
        nfc_perror(pnd, "nfc_initiator_init");
        nfc_close(pnd);
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }

    // Configure the CRC
    if (nfc_device_set_property_bool(pnd, NP_HANDLE_CRC, false) < 0) {
        nfc_perror(pnd, "nfc_device_set_property_bool");
        nfc_close(pnd);
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }
    // Use raw send/receive methods
    if (nfc_device_set_property_bool(pnd, NP_EASY_FRAMING, false) < 0) {
        nfc_perror(pnd, "nfc_device_set_property_bool");
        nfc_close(pnd);
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }
    // Disable 14443-4 autoswitching
    if (nfc_device_set_property_bool(pnd, NP_AUTO_ISO14443_4, false) < 0) {
        nfc_perror(pnd, "nfc_device_set_property_bool");
        nfc_close(pnd);
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }

    // Set up signal handler
    signal(SIGINT, stop_polling);
}


nfc::~nfc() {
    nfc_exit(context);
    nfc_abort_command(pnd);
}

bool nfc::transmit_bytes(const uint8_t *pbtTx, const size_t szTx) {
    uint32_t cycles = 0;
    int res;
    if (timed) {
        if ((res = nfc_initiator_transceive_bytes_timed(pnd, pbtTx, szTx, abtRx, sizeof(abtRx), &cycles)) < 0)
            return false;
        if ((!quiet_output) && (res > 0)) {
            printf("Response after %u cycles\n", cycles);
        }
    } else {
        if ((res = nfc_initiator_transceive_bytes(pnd, pbtTx, szTx, abtRx, sizeof(abtRx), 0)) < 0)
            return false;
    }
    szRx = res;
    return true;
}

bool nfc::transmit_bits(const uint8_t *pbtTx, const size_t szTxBits) {
    uint32_t cycles = 0;
    if (timed) {
        if ((szRxBits = nfc_initiator_transceive_bits_timed(pnd, pbtTx, szTxBits, NULL, abtRx, sizeof(abtRx), NULL, &cycles)) < 0)
            return false;
        if ((!quiet_output) && (szRxBits > 0)) {
            printf("Response after %u cycles\n", cycles);
        }
    } else {
        if ((szRxBits = nfc_initiator_transceive_bits(pnd, pbtTx, szTxBits, NULL, abtRx, sizeof(abtRx), NULL)) < 0)
            return false;
    }
    return true;
}

void nfc::print_vector(const std::vector<uint8_t>& vec) {
    printf("UID: ");
    fflush(stdout);
    for (const auto& byte : vec) {
        printf("%02x ", byte);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
}

bool nfc::print_hex(const uint8_t* data, const size_t len) {

    const uint8_t nullData[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    if (!data || len == 0 || data==nullData) return 0;

    current_UID.assign(data, data + len);

    auto it = std::find(list_of_vectors.begin(), list_of_vectors.end(), current_UID);

    if (it == list_of_vectors.end()) {
        printf("The following (NFC) ISO14443A tag was found new:\n");
        fflush(stdout);
        print_vector(current_UID);
        list_of_vectors.push_back(current_UID);

        return 1;
    } else {
        printf("UID already detected.\n");
        return 0;
    }
}


bool nfc::raw_mode_start(void)
{
    // Configure the CRC
    if (nfc_device_set_property_bool(pnd, NP_HANDLE_CRC, false) < 0) {
        nfc_perror(pnd, "nfc_configure");
        return false;
    }
    // Use raw send/receive methods
    if (nfc_device_set_property_bool(pnd, NP_EASY_FRAMING, false) < 0) {
        nfc_perror(pnd, "nfc_configure");
        return false;
    }
    return true;
}

bool nfc::raw_mode_end(void)
{
    // reset reader
    // Configure the CRC
    if (nfc_device_set_property_bool(pnd, NP_HANDLE_CRC, true) < 0) {
        nfc_perror(pnd, "nfc_device_set_property_bool");
        return false;
    }
    // Switch off raw send/receive methods
    if (nfc_device_set_property_bool(pnd, NP_EASY_FRAMING, true) < 0) {
        nfc_perror(pnd, "nfc_device_set_property_bool");
        return false;
    }
    return true;
}


bool nfc::ev1_pwd_auth(uint8_t *pwd)
{
    if (!raw_mode_start())
        return false;
    memcpy(&abtPWAuth[1], pwd, 4);
    iso14443a_crc_append(abtPWAuth, 5);
    if (!transmit_bytes(abtPWAuth, 7))
        return false;
    if (!raw_mode_end())
        return false;
    return true;
}


bool nfc::card_init(){
    nfc_init(&context);
    if (context == nullptr) {
        std::cerr << "Unable to init libnfc" << std::endl;
        return EXIT_FAILURE;
    }

    pnd = nfc_open(context, nullptr);
    if (pnd == nullptr) {
        std::cerr << "Unable to open NFC device." << std::endl;
        nfc_exit(context);
        return EXIT_FAILURE;
    }

    std::cout << "NFC reader: " << nfc_device_get_name(pnd) << " opened" << std::endl;

    if (nfc_initiator_init(pnd) < 0) {
        nfc_perror(pnd, "nfc_initiator_init");
        nfc_close(pnd);
        nfc_exit(context);
        return EXIT_FAILURE;
    }

    if (nfc_device_set_property_bool(pnd, NP_INFINITE_SELECT, false) < 0) {
        nfc_perror(pnd, "nfc_device_set_property_bool");
        nfc_close(pnd);
        nfc_exit(context);
        return EXIT_FAILURE;
    }

    nfc_modulation nmMifare = {NMT_ISO14443A, NBR_106};
    nfc_target nt;
    if (nfc_initiator_select_passive_target(pnd, nmMifare, nullptr, 0, &nt) <= 0) {
        std::cerr << "No MIFARE Ultralight tag found." << std::endl;
        nfc_close(pnd);
        nfc_exit(context);
        return EXIT_FAILURE;
    }

    // printf("Authing with PWD: %02x%02x%02x%02x ", 0x89, 0x89, 0x89, 0x89);
    // if (!ev1_pwd_auth(iPWD)) {
    //     printf("\n");
    //     std::cout << "AUTH failed!\n";
    //     nfc_close(pnd);
    //     nfc_exit(context);
    //     return EXIT_FAILURE;
    // } else {
    //     printf("Success - PACK: %02x%02x\n", abtRx[0], abtRx[1]);
    //     // memcpy(iPACK, abtRx, 2);
    // }


    return true;
}



bool nfc::write_page(uint8_t page, uint8_t *data) {

    uint8_t cmd[6];
    uint8_t abtRx[100];
    int szRx;

    // The WRITE command format:
    // [0xA2] [PAGE] [DATA0] [DATA1] [DATA2] [DATA3]
    cmd[0] = 0xA2; // WRITE command
    cmd[1] = page; // Target page

    // Copy the data to the command buffer
    memcpy(&cmd[2], data, 4);

    // Transmit the command
    if ((szRx = nfc_initiator_transceive_bytes(pnd, cmd, 6, abtRx, sizeof(abtRx), 0)) < 0) {
        nfc_perror(pnd, "nfc_initiator_transceive_bytes");
        return false;
    }

    // Check if the response is a success response (usually a ACK)
    if (szRx != 1 || abtRx[0] != 0x0A) {
        printf("Write failed or wrong response\n");
        return false;
    }

    return true;
}






void nfc::stop_polling(int sig) {
    (void) sig;
    if (pnd != nullptr)
        nfc_abort_command(pnd);
    nfc_exit(context);
    exit(EXIT_FAILURE);
}

bool nfc::get_UID(){

    bool tag_available = false;
    nfc_context *context;
    static bool initialized = false;

    uint8_t  abtReqa[1] = { 0x26 };
    uint8_t  abtSelectAll[2] = { 0x93, 0x20 };
    uint8_t  abtSelectTag[9] = { 0x93, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t  abtRats[4] = { 0xe0, 0x50, 0x00, 0x00 };
    uint8_t  abtHalt[4] = { 0x50, 0x00, 0x00, 0x00 };
    std::fill(std::begin(abtRx), std::end(abtRx), 0);
    szRxBits = 0;
    szRx = sizeof(abtRx);
    std::fill(std::begin(abtRawUid), std::end(abtRawUid), 0);
    std::fill(std::begin(abtAtqa), std::end(abtAtqa), 0);
    abtSak = 0;
    std::fill(std::begin(abtAts), std::end(abtAts), 0);
    szAts = 0;
    szCL = 2;

    abtSelectTag[0] = 0x95;

    nfc_init(&context);


    if (!initialized) {
        nfc_init(&context);
        if (context == NULL) {
            ERR("Unable to init libnfc (malloc)");
            exit(EXIT_FAILURE);
        }
        initialized = true;
    }

    // Try to open the NFC reader
    pnd = nfc_open(context, NULL);

    if (pnd == NULL) {
        ERR("Error opening NFC reader");
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }

    // Initialise NFC device as "initiator"
    if (nfc_initiator_init(pnd) < 0) {
        nfc_perror(pnd, "nfc_initiator_init");
        nfc_close(pnd);
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }

    // Configure the CRC
    if (nfc_device_set_property_bool(pnd, NP_HANDLE_CRC, false) < 0) {
        nfc_perror(pnd, "nfc_device_set_property_bool");
        nfc_close(pnd);
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }
    // Use raw send/receive methods
    if (nfc_device_set_property_bool(pnd, NP_EASY_FRAMING, false) < 0) {
        nfc_perror(pnd, "nfc_device_set_property_bool");
        nfc_close(pnd);
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }
    // Disable 14443-4 autoswitching
    if (nfc_device_set_property_bool(pnd, NP_AUTO_ISO14443_4, false) < 0) {
        nfc_perror(pnd, "nfc_device_set_property_bool");
        nfc_close(pnd);
        nfc_exit(context);
        exit(EXIT_FAILURE);
    }

    // Send the 7 bits request command specified in ISO 14443A (0x26)
    if(tag_available==false){
        if (transmit_bits(abtReqa, 7)) {
            // printf("Etag available\n");
            // fflush(stdout);
            // nfc_close(pnd);
            // nfc_exit(context);
            // exit(EXIT_FAILURE);
            tag_available=true;
        }
        else{
            tag_available=false;
        }
    }


    if(tag_available){

        memcpy(abtAtqa, abtRx, 2);

        // Anti-collision
        transmit_bytes(abtSelectAll, 2);

        // Check answer
        // if ((abtRx[0] ^ abtRx[1] ^ abtRx[2] ^ abtRx[3] ^ abtRx[4]) != 0) {
        //     printf("WARNING: BCC check failed!\n");
        // }

        // Save the UID CL1
        memcpy(abtRawUid, abtRx, 4);

        //Prepare and send CL1 Select-Command
        memcpy(abtSelectTag + 2, abtRx, 5);
        iso14443a_crc_append(abtSelectTag, 7);
        transmit_bytes(abtSelectTag, 9);
        abtSak = abtRx[0];

        // Test if we are dealing with a CL2
        if (abtSak & CASCADE_BIT) {
            szCL = 2;//or more
            // Check answer
            // if (abtRawUid[0] != 0x88) {
            //     printf("WARNING: Cascade bit set but CT != 0x88!\n");
            // }
        }

        if (szCL == 2) {
            // We have to do the anti-collision for cascade level 2

            // Prepare CL2 commands
            abtSelectAll[0] = 0x95;

            // Anti-collision
            transmit_bytes(abtSelectAll, 2);

            // Check answer
            // if ((abtRx[0] ^ abtRx[1] ^ abtRx[2] ^ abtRx[3] ^ abtRx[4]) != 0) {
            //     printf("WARNING: BCC check failed!\n");
            // }

            // Save UID CL2
            memcpy(abtRawUid + 4, abtRx, 4);

            // Selection
            abtSelectTag[0] = 0x95;
            memcpy(abtSelectTag + 2, abtRx, 5);
            iso14443a_crc_append(abtSelectTag, 7);
            transmit_bytes(abtSelectTag, 9);
            abtSak = abtRx[0];

            // Test if we are dealing with a CL3
            if (abtSak & CASCADE_BIT) {
                szCL = 2;
                // Check answer
                // if (abtRawUid[0] != 0x88) {
                //     printf("WARNING: Cascade bit set but CT != 0x88!\n");
                // }
            }

            if (szCL == 3) {
                // We have to do the anti-collision for cascade level 3

                // Prepare and send CL3 AC-Command
                abtSelectAll[0] = 0x97;
                transmit_bytes(abtSelectAll, 2);

                // Check answer
                // if ((abtRx[0] ^ abtRx[1] ^ abtRx[2] ^ abtRx[3] ^ abtRx[4]) != 0) {
                //     printf("WARNING: BCC check failed!\n");
                // }

                // Save UID CL3
                memcpy(abtRawUid + 8, abtRx, 4);

                // Prepare and send final Select-Command
                abtSelectTag[0] = 0x97;
                memcpy(abtSelectTag + 2, abtRx, 5);
                iso14443a_crc_append(abtSelectTag, 7);
                transmit_bytes(abtSelectTag, 9);
                abtSak = abtRx[0];
            }
        }

        // Request ATS, this only applies to tags that support ISO 14443A-4
        if (abtRx[0] & SAK_FLAG_ATS_SUPPORTED) {
            iso_ats_supported = true;
        }
        if ((abtRx[0] & SAK_FLAG_ATS_SUPPORTED) || force_rats) {
            iso14443a_crc_append(abtRats, 2);
            if (transmit_bytes(abtRats, 4)) {
                memcpy(abtAts, abtRx, szRx);
                szAts = szRx;
            }
        }

        // Done, halt the tag now
        iso14443a_crc_append(abtHalt, 2);
        transmit_bytes(abtHalt, 4);

        // printf("\nFound tag with\n UID: ");
        switch (szCL) {
        case 1:
            printf("%02x%02x%02x%02x", abtRawUid[0], abtRawUid[1], abtRawUid[2], abtRawUid[3]);
            // printf("Case1");
            break;

        case 3:
            printf("%02x%02x%02x", abtRawUid[1], abtRawUid[2], abtRawUid[3]);
            printf("%02x%02x%02x", abtRawUid[5], abtRawUid[6], abtRawUid[7]);
            printf("%02x%02x%02x%02x", abtRawUid[8], abtRawUid[9], abtRawUid[10], abtRawUid[11]);
            // printf("Case3");
            break;

        case 2:
            // printf("%02x%02x%02x", abtRawUid[1], abtRawUid[2], abtRawUid[3]);
            // printf("%02x%02x%02x%02x", abtRawUid[4], abtRawUid[5], abtRawUid[6], abtRawUid[7]);
            const uint8_t* UID_data = abtRawUid;
            return print_hex(UID_data,sizeof(abtRawUid)-4);
            // printf("Case2");
            // fflush(stdout);
            break;
        }
        nfc_exit(context);
        nfc_abort_command(pnd);

        // print_hex(szCL);
    }

    nfc_exit(context);
    nfc_abort_command(pnd);

    return 0;
}


// int main(int argc, char *argv[])
// {

//     nfc nfcD;

//     while(true){
//         nfcD.get_UID();
//     }
//     return 0;
// }


