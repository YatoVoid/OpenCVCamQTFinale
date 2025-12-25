


#include "nfc_cam.h"





bool CardScanner::hasNumber(const std::string& str) {
    for (char c : str) {
        if (std::isdigit(c)) {
            return true;
        }
    }
    return false;
}

bool CardScanner::hasSpecialChar(const std::string& str) {
    for (char c : str) {
        if (std::isdigit(c)) {
            return true;
        } else if (!std::isalnum(c)) {
            return false;
        }
    }
    return false;
}

int CardScanner::countDigits(const string& str) {
    int count = 0;
    for (char c : str) {
        if (isdigit(c)) {
            count++;
        }
    }


    return count;
}

int CardScanner::countLetters(const string& str) {
    int count = 0;
    for (char c : str) {

        // if (!isdigit(c)) {
        //     count++;
        // }
        if(c== ':'){
            count++;
        }
        if(c==' '){
            count=0;
        }

    }
    return count;
}

void CardScanner::getFinalOutput() {


    int n = totalTriesToDo;
    // for (int i = 0; i < n; i++) {
    //     cout << OutComeAll[i] << endl; // print all
    // }

    int freq = 0;


    for (int i = 0; i < n; i++) {
        int count = 0;
        for (int j = i + 1; j < n; j++) {
            if (OutComeAll[j] == OutComeAll[i]) {
                count++;
            }
        }
        if (count >= freq) {
            res = OutComeAll[i];
            freq = count;
        }
    }

    cout << "The word that occurs most is : " << res << endl;
    // cout << "No of times: " << freq << endl;

    ofstream outputFile("final_output_text.txt");
    if (outputFile.is_open()) {
        outputFile << res << endl;
        outputFile.close();
    }

    totalTries = 0;
    currentScan = false;


}

void CardScanner::imagett(Mat croppedImage,string typeofRFIDtmp) {


    if(totalTries>=totalTriesToDo){
        getFinalOutput();
    }

    cout<<"totalTried: "<<totalTries<<" totalToDo: "<<totalTriesToDo<< endl;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
    api->SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:");


    Mat grayImage;
    Mat binaryImage;


    if(typeofRFIDtmp!="Keyfob"){
        cvtColor(croppedImage, grayImage, COLOR_BGR2GRAY);
        // GaussianBlur(grayImage, grayImage, Size(5, 5), 1);
        adaptiveThreshold(grayImage, binaryImage, 155, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, 2);

    }
    else if(typeofRFIDtmp=="Keyfob"){ // if this is blue
        cvtColor(croppedImage, grayImage, COLOR_BGR2GRAY);
        // adaptiveThreshold(grayImage, binaryImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, 2);
        // cvtColor(croppedImage, hsvImage, COLOR_BGR2HSV);
        // inRange(hsvImage, lower_blue, upper_blue, blueMask);
        // binaryImage.setTo(0, blueMask);
    }


    namedWindow("Binary Image", WINDOW_AUTOSIZE);
    imshow("Binary Image", binaryImage);


    api->SetImage(binaryImage.data, binaryImage.cols, binaryImage.rows, 1, binaryImage.cols);


    string outText = string(api->GetUTF8Text());

    std::cout<<outText <<endl;
    fflush(stdout);



    bool enoughDigitsDetected = false;
    bool enoughNumbersDetected = false;

    int rotations = 0;

    if(outText!=""){
        if (countDigits(outText) == amountOfDigits) {
            enoughDigitsDetected = true;

        }
        if (countLetters(outText) == amountOfLetters) {
            enoughNumbersDetected = true;
        }
        /*--------------------------------------------------------------------------------------*/


        // if(countLetters(outText)+countDigits(outText)>=9){
        //     enoughDigitsDetected = true;
        //     enoughNumbersDetected = true;
        // }


        // if (enoughDigitsDetected && enoughNumbersDetected) {
        if(enoughDigitsDetected || enoughNumbersDetected){
            ofstream outputFile("output_text.txt");
            if (outputFile.is_open()) {
                outputFile << outText;
                outputFile.close();
                OutComeAll[totalTries] = outText;
                totalTries+=1;

            }
        } else {
            // nothing detected
        }

        api->End();
        delete api;
    }

}

VideoCapture CardScanner::openCamera() {
    VideoCapture cap(2);
    if (!cap.isOpened()) {
        cout << "Error: Unable to open camera" << endl;
        return VideoCapture();
    }
    return cap;
}

void CardScanner::processFrames(VideoCapture& cap,string typeofRFIDtmp) {
    Mat frame;
    vector<vector<Point>> cnts;
    vector<Point> largest_contour;
    Rect bounding_rect;


    namedWindow("frame", WINDOW_NORMAL);

    double fps;
    int frames = 1;
    bool card_detected = false;
    bool save_image = false;

    while (currentScan) {

            cap.read(frame);




            if (true) {
                imwrite("detected_card.jpg", frame);



                int cropWidth;
                int cropHeight;


                int startX;
                int startY;


                if(typeofRFIDtmp!="Keyfob"){
                    cropWidth = 180;
                    cropHeight = 50;
                    startX = frame.cols - cropWidth;
                    startY = frame.rows - cropHeight;
                    startX = std::max(startX, 0);
                    startY = std::max(startY-30, 0);
                }
                else if(typeofRFIDtmp=="Keyfob"){
                    cropWidth = 180;
                    cropHeight = 50;
                    startX = frame.cols - cropWidth;
                    startY = frame.rows - cropHeight;
                    startX = std::max(startX-250, 0);
                    startY = std::max(startY-220, 0);
                }




                Rect bounding_rect(startX, startY, cropWidth, cropHeight);


                rectangle(frame, bounding_rect, 0);

                Mat croppedImage = frame(bounding_rect);

                imwrite("bottom_right_corner.jpg", croppedImage);
                imagett(croppedImage,typeofRFIDtmp);
            }



            // for (const auto& contour : cnts) {
            //     Rect boundingBox = boundingRect(contour);
            //     int width = boundingBox.width;
            //     int height = boundingBox.height;
            //     cout<<width <<"-"<<height<<endl;

            //     if (width <= 670 && width >= 500 && height <=320 && height >= 200) {
            //         largest_contour = contour;
            //         card_detected = true;
            //         cout << "FOUND CARD";
            //     } else {
            //         card_detected = false;
            //     }


            //     putText(frame, to_string(card_detected), Point(30, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
            //     // cout << width << "  " << height << endl;
            // }

            // if (card_detected) {
            //     // auto end_time = chrono::steady_clock::now();
            //     // auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - card_start_time).count() / 1000.0;
            //     // if (elapsed_time >= 2) {
            //     //     save_image = true;
            //     // }
            //     save_image = true;
            // } /*else {
            //     card_start_time = chrono::steady_clock::now();
            // }*/




            // auto end_time = chrono::steady_clock::now();
            // auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() / 1000.0;
            // fps = frames / elapsed_time;
            // frames++;
            // putText(frame, to_string(fps), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

            imshow("frame", frame);


            if (waitKey(1) == 'q')
                break;

    }


}




//delete
void display_text(cv::Mat &img, const std::string &text, int x, int y, double scale = 1.0, cv::Scalar color = cv::Scalar(255, 255, 255)) {
    int thickness = 2;
    int baseline = 0;
    cv::Size textSize = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, scale, thickness, &baseline);
    baseline += thickness;

    // Put text on the image
    cv::putText(img, text, cv::Point(x, y + textSize.height), cv::FONT_HERSHEY_SIMPLEX, scale, color, thickness);
}


string CardScanner::run(string typeofRFIDtmp) {

    CardScanner::typeofRFID =typeofRFIDtmp;

    VideoCapture cap = openCamera();
    processFrames(cap,CardScanner::typeofRFID);

    cap.release();
    destroyAllWindows();

    std::string current_uid = res;
    std::string new_uid = current_uid;
    bool uid_confirmed = false;

    cv::Mat window(300, 600, CV_8UC3, cv::Scalar(0, 0, 0));


    while (!uid_confirmed) {

        window.setTo(cv::Scalar(0, 0, 0));


        display_text(window, "Your UID is:"+current_uid, 50, 100, 0.8, cv::Scalar(255, 255, 255));
        display_text(window, "Press 'y' to confirm or 'n' to change the UID.", 50, 150, 0.6, cv::Scalar(255, 255, 255));

        cv::imshow("UID Confirmation", window);


        char key = (char)cv::waitKey(0);

        if (key == 'y' || key == 'Y') {
            uid_confirmed = true;
            std::cout << "UID confirmed: " << current_uid << std::endl;
        } else if (key == 'n' || key == 'N') {
            std::cout << "Enter new UID:";
            std::cin >> new_uid;
            current_uid = new_uid;
        }
    }


    // window.setTo(cv::Scalar(0, 0, 0));
    // display_text(window, "UID confirmed: " + current_uid, 50, 100, 0.8, cv::Scalar(255, 255, 255));
    // cv::imshow("UID Confirmation", window);
    // cv::waitKey(2000);


    cv::destroyWindow("UID Confirmation");





    return current_uid;
    // return *OutComeAll;
}

// CardScanner(int letters, int digits, int totalTries)

// int main() {
//     CardScanner scanner(4, 10, 10);
//     scanner.run();
//     return 0;
// }
