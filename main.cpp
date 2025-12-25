
#include "nfc_cam.h"
#include "nfc_device.hpp"

#include <thread>


string increment_uid(const std::string& uid_str) {

    std::string hex_str = uid_str;
    hex_str.erase(std::remove(hex_str.begin(), hex_str.end(), ':'), hex_str.end());


    unsigned long long uid_int = std::stoull(hex_str, nullptr, 16);


    uid_int += 1;


    std::stringstream ss;
    ss << std::uppercase << std::setfill('0') << std::setw(12) << std::hex << uid_int;
    std::string new_hex_str = ss.str();


    std::string new_uid_str;
    for (size_t i = 0; i < new_hex_str.size(); i += 2) {
        new_uid_str += new_hex_str.substr(i, 2);
        if (i < new_hex_str.size() - 2) {
            new_uid_str += ":";
        }
    }

    return new_uid_str;
}



int main() {



    nfc nfcD;
    //CardScanner(int letters, int digits, int totalTries)
    CardScanner scanner(4, 4, 50);

    string userMethodChoice ="0" ;


    cout<<"Choose Method Of Configuration"<<endl<<"1.Using Camera (KEYCARD)"<<endl<<"2.Using Continues Scanning Counting The Next UID"<<endl<<"3.Using Camera (KEYFOB)"<<endl<<"Choice(1 - 3): ";
    cin>>userMethodChoice;


    cout<<"Choice:"<<userMethodChoice;

    if(userMethodChoice=="1"){
        while(1){

            if(nfcD.get_UID()!=true){
                std::cout<<"NEokey"<<"\n";
                fflush(stdout);
                continue;
            }



            std::cout<<"Okey"<<"\n";
            fflush(stdout);

            string front_UID = scanner.run("");




            // Hex + string

            std::stringstream ss;
            for (auto byte : nfcD.pwd) {
                ss << std::hex << std::setfill('0') << std::setw(2) << (int)byte;
            }
            std::string currentHexString = ss.str();



            front_UID.erase(std::remove(front_UID.begin(), front_UID.end(), ':'), front_UID.end());
            std::string frontHexString = front_UID;


            unsigned long long currentHexValue = std::stoull(currentHexString, nullptr, 16);
            unsigned long long frontHexValue = std::stoull(frontHexString, nullptr, 16);


            unsigned long long result = currentHexValue + frontHexValue;
            cout<<"UNSIGNED: "<< result;



            uint8_t ID[4];
            ID[0] = (result >> 24) & 0xFF;
            ID[1] = (result >> 16) & 0xFF;
            ID[2] = (result >> 8) & 0xFF;
            ID[3] = result & 0xFF;





            // std::cout << "ID bytes: ";
            // for (int i = 0; i < 4; ++i) {
            //     std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)ID[i] << " ";
            // }
            // std::cout << std::endl;

            // nfcD.card_init();

            // nfcD.write_page(7, ID);

            // nfcD.card_init();

            // nfcD.write_page(17, nfcD.cfg1);

            // nfcD.card_init();

            // nfcD.write_page(18, nfcD.pwd);

            // nfcD.card_init();

            // nfcD.write_page(19, nfcD.pack);

            // nfcD.card_init();

            // nfcD.write_page(16, nfcD.cfg0);

            front_UID = "";
            scanner.currentScan = true;

            // // system("ffplay -nodisp -autoexit /home/wali/VisualStudioCpp/OpenCVCamQT/BplinkSound.mp3");

        }
    }







    if(userMethodChoice=="2"){

        string starter_UID = "00:00:00:00:00";
        string next_UID = "";

        bool set_starter_UID = false;

        cout<<"Your Starter UID:";
        cin >>starter_UID;

        set_starter_UID = true;

        string front_UID = starter_UID;



        while(set_starter_UID){

            cout<<"Your current UID: "<<front_UID<<endl;

            try {
                if (!nfcD.get_UID()) {
                    std::cout << "NEokey" << std::endl;
                    fflush(stdout);
                    for (int i = 1; i > 0; --i) {
                        this_thread::sleep_for(chrono::seconds(1));
                    }
                    continue;
                }
            } catch (const std::exception& e) {
                // Handle any exceptions that nfcD.get_UID() might throw
                std::cerr << "Error occurred while getting UID: " << e.what() << std::endl;
                fflush(stderr);  // Flush the error output
                continue;  // Skip the current iteration and try again
            } catch (...) {
                // Catch any other types of exceptions
                std::cerr << "Unknown error occurred while getting UID." << std::endl;
                fflush(stderr);  // Flush the error output
                continue;  // Skip the current iteration and try again
            }




            std::cout<<"Configuring With UID: "<<front_UID<<"\n";
            fflush(stdout);

            // string front_UID = scanner.run();




            // Hex + string

            std::stringstream ss;
            for (auto byte : nfcD.pwd) {
                ss << std::hex << std::setfill('0') << std::setw(2) << (int)byte;
            }
            std::string currentHexString = ss.str();



            front_UID.erase(std::remove(front_UID.begin(), front_UID.end(), ':'), front_UID.end());
            std::string frontHexString = front_UID;


            unsigned long long currentHexValue = std::stoull(currentHexString, nullptr, 16);
            unsigned long long frontHexValue = std::stoull(frontHexString, nullptr, 16);


            unsigned long long result = currentHexValue + frontHexValue;
            cout<<"UNSIGNED: "<< result;



            uint8_t ID[4];
            ID[0] = (result >> 24) & 0xFF;
            ID[1] = (result >> 16) & 0xFF;
            ID[2] = (result >> 8) & 0xFF;
            ID[3] = result & 0xFF;





            // std::cout << "ID bytes: ";
            // for (int i = 0; i < 4; ++i) {
            //     std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)ID[i] << " ";
            // }
            // std::cout << std::endl;

            // nfcD.card_init();

            // nfcD.write_page(7, ID);

            // nfcD.card_init();

            // nfcD.write_page(17, nfcD.cfg1);

            // nfcD.card_init();

            // nfcD.write_page(18, nfcD.pwd);

            // nfcD.card_init();

            // nfcD.write_page(19, nfcD.pack);

            // nfcD.card_init();

            // nfcD.write_page(16, nfcD.cfg0);

            front_UID = increment_uid(front_UID); // incriment hereeee
            system("ffplay -nodisp -autoexit /home/wali/VisualStudioCpp/OpenCVCamQT/BplinkSound.mp3");
            std::cout << "Resuming program after 3 seconds!" << std::endl;
            cout << "Starting the countdown..." << endl;


            for (int i = 3; i > 0; --i) {
                cout << i << " seconds remaining" << endl;
                this_thread::sleep_for(chrono::seconds(1));
            }

            cout << "Time's up!" << endl;




            scanner.currentScan = true;
        }
    }




    if(userMethodChoice=="3"){
        while(1){

            if(nfcD.get_UID()!=true){
                std::cout<<"NEokey"<<"\n";
                fflush(stdout);
                continue;
            }



            std::cout<<"Okey"<<"\n";
            fflush(stdout);

            string front_UID = scanner.run("Keyfob");




            // Hex + string

            std::stringstream ss;
            for (auto byte : nfcD.pwd) {
                ss << std::hex << std::setfill('0') << std::setw(2) << (int)byte;
            }
            std::string currentHexString = ss.str();



            front_UID.erase(std::remove(front_UID.begin(), front_UID.end(), ':'), front_UID.end());
            std::string frontHexString = front_UID;


            unsigned long long currentHexValue = std::stoull(currentHexString, nullptr, 16);
            unsigned long long frontHexValue = std::stoull(frontHexString, nullptr, 16);


            unsigned long long result = currentHexValue + frontHexValue;
            cout<<"UNSIGNED: "<< result;



            uint8_t ID[4];
            ID[0] = (result >> 24) & 0xFF;
            ID[1] = (result >> 16) & 0xFF;
            ID[2] = (result >> 8) & 0xFF;
            ID[3] = result & 0xFF;





            // std::cout << "ID bytes: ";
            // for (int i = 0; i < 4; ++i) {
            //     std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)ID[i] << " ";
            // }
            // std::cout << std::endl;

            nfcD.card_init();

            nfcD.write_page(7, ID);

            nfcD.card_init();

            nfcD.write_page(17, nfcD.cfg1);

            nfcD.card_init();

            nfcD.write_page(18, nfcD.pwd);

            nfcD.card_init();

            nfcD.write_page(19, nfcD.pack);

            nfcD.card_init();

            nfcD.write_page(16, nfcD.cfg0);

            front_UID = "";
            scanner.currentScan = true;

            // // system("ffplay -nodisp -autoexit /home/wali/VisualStudioCpp/OpenCVCamQT/BplinkSound.mp3");

        }
    }


    return 0;
}



//CardScanner(int letters, int digits, int totalTries) :
// int main() {
//     CardScanner scanner(4, 10, 10);
//     scanner.run();
//     return 0;
// }
