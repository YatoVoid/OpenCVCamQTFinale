#arch
FROM archlinux:latest

# required packages
RUN pacman -Syu --noconfirm \
    gtk3 \
    gstreamer \
    gst-plugins-base \
    gst-plugins-good \
    gst-plugins-bad \
    gst-plugins-ugly \
    xorg-xhost \
    pkgconf \
    base-devel \
    jdk-openjdk \
    openmpi \
    cmake \
    gcc \
    opencv \
    leptonica \
    tesseract \
    qt6-base \
    qt6-svg \
    qt6-tools \
    qt6-multimedia \
    pcsclite \
    wget \
    vtk \
    hdf5 \
    unzip \
    usbutils \
    udev \
    && pacman -Scc --noconfirm



#Copy the Project directory
COPY . /app 

#Before building OpenCV we have to put config files correctly

RUN mkdir -p /etc/nfc/devices.d/ && \
    echo -e "name = \"PN532 board via UART\"\nconnstring = pn532_uart:/dev/ttyUSB0\nallow_intrusive_scan = true" > /etc/nfc/devices.d/pn532_uart.conf && \
    echo "allow_autoscan = true" > /etc/nfc/libnfc.conf 




#Libnfc build
WORKDIR /tmp
RUN wget https://github.com/nfc-tools/libnfc/releases/download/libnfc-1.7.1/libnfc-1.7.1.tar.bz2 && \
    tar -jxvf libnfc-1.7.1.tar.bz2 && \
    cd libnfc-1.7.1 && \
    ./configure --sysconfdir=/etc --prefix=/usr --with-drivers=pn532_uart && \
    make && make install && \  
    cp -r /tmp/libnfc-1.7.1 /usr/local/include && \
    rm -rf /tmp/libnfc-1.7.1 /tmp/libnfc-1.7.1.tar.bz2


#Libnfc blacklist file nfc
RUN echo -e "blacklist nfc\nblacklist pn533\nblacklist pn533_usb" > /etc/modprobe.d/blacklist-libnfc.conf



#opencv4
WORKDIR /tmp
RUN wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip && \
    unzip opencv.zip && \
    mv opencv-4.x opencv4 && \
    cd opencv4 && \
    mkdir build && \
    cd build && \
    cmake ../ && \
    cmake --build . && \
    make install && \
    cp -r ../../opencv4 /usr/local/include
    # rm -rf /tmp/opencv.zip /tmp/build






WORKDIR /app


#Building Project
RUN cd /app && \ 
    ls && \
    cd build && \
    cmake ../ && \
    make  
    

WORKDIR /app
#Tesseract tRAINING data
    
RUN cd /app && \
    cp eng.traineddata /usr/share/tessdata/


# RUN wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage && \
#     chmod +x appimagetool-x86_64.AppImage && \
#     mv appimagetool-x86_64.AppImage /usr/local/bin/appimagetool



# WORKDIR /app/build

# RUN mkdir -p AppDir/usr/share/applications/ && \
#     mv ../opencvqt.desktop AppDir/usr/share/applications/opencvqt.desktop && \
#     mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps/ && \
#     cp ../OpencvQT.png AppDir/usr/share/icons/hicolor/256x256/apps/

# RUN mkdir -p AppDir/usr/bin
# RUN ls
# RUN cp OpenCVCamQT AppDir/usr/bin/



# RUN wget https://github.com/probonopd/linuxdeployqt/releases/tag/continuous/linuxdeployqt-continuous-x86_64.AppImage
#     ./linuxdeployqt-continuous-x86_64.AppImage AppDir/usr/share/applications/opencvqt.desktop --appimage 
# #     appimagetool AppDir
