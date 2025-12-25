#ifndef NFC_CAM_H
#define NFC_CAM_H


#include <fstream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <nfc/nfc.h>




using namespace cv;
using namespace std;



class CardScanner  {
private:
    int amountOfLetters;
    int amountOfDigits;
    int totalTries;
    const int totalTriesToDo;

    string* OutComeAll;
    string res;

    bool hasNumber(const std::string& str);
    bool hasSpecialChar(const std::string& str);
    int countDigits(const string& str);
    int countLetters(const string& str);
    void getFinalOutput();

    void imagett(Mat croppedImage,string typeofRFID);
    VideoCapture openCamera();
    void processFrames(VideoCapture& cap,string typeofRFIDtmp);

public:
    bool currentScan;
    string typeofRFID;
    CardScanner(int letters, int digits, int totalTriesToDo) :
        amountOfLetters(letters), amountOfDigits(digits), totalTries(0),
        totalTriesToDo(totalTriesToDo), currentScan(true) {
        OutComeAll = new string[totalTriesToDo];
    }

    ~CardScanner() {
        delete[] OutComeAll;
    }

    string run(string typeofRFID);
};





#endif // NFC_CAM_H
