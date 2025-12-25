cmake_minimum_required(VERSION 3.5)

project(OpenCVCamQT LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV package
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add libnfc include directories
include_directories("/usr/local/include/libnfc-1.7.1")
include_directories("/usr/local/include/libnfc-1.7.1/utils")  # Ensure nfc-utils.h is found

include_directories("/usr/local/include")


# Add executable and source files
add_executable(OpenCVCamQT

    main.cpp
    nfc_cam.cpp
    nfc_cam.h
    nfc_device.hpp
    nfc_device.cpp
)



# Link OpenCV libraries
target_link_libraries(OpenCVCamQT ${OpenCV_LIBS})

# Link libnfc libraries (assuming they are installed in /usr/local)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Link to Qt 6
target_link_libraries(OpenCVCamQT Qt6::Core Qt6::Widgets Qt6::Gui)



target_link_libraries(OpenCVCamQT /usr/lib/libnfc.so)



# Find Leptonica using pkg-config
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(LEPTONICA REQUIRED lept)
# include_directories(${LEPTONICA_INCLUDE_DIRS})
# link_directories(${LEPTONICA_LIBRARY_DIRS})
# target_link_libraries(OpenCVCamQT ${LEPTONICA_LIBRARIES})

# Find and link Tesseract using pkg-config
pkg_check_modules(TESSERACT REQUIRED tesseract)
include_directories(${TESSERACT_INCLUDE_DIRS})
link_directories(${TESSERACT_LIBRARY_DIRS})
target_link_libraries(OpenCVCamQT ${TESSERACT_LIBRARIES})

# Install directives for GNU
include(GNUInstallDirs)
install(TARGETS OpenCVCamQT
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Output the include directories for debugging purposes
message("Include directories: ${CMAKE_INCLUDE_PATH}")
