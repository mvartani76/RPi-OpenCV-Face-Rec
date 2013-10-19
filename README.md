RPi-OpenCV-Face-Rec
===================

Simple implementation of OpenCV Face Recognition from Pierre at http://thinkrpi.wordpress.com/2013/05/22/opencv-and-camera-board-csi/

Note that this version uses the RPi 5MP Camera Module and not a USB or Wireless Network Camera.


Pre-Requisites (taken from http://thinkrpi.wordpress.com/2013/04/05/step-3-install-softwares-for-webcam-and-computer-vision/)

<b> 1. Install CMake </b>

CMake is mandatory to compile.

sudo apt-get update<br>
sudo apt-get install cmake<br>

<b> 2. Install OpenCV Lib for face recognition </b>

OpenCV is a wonderful library to do computer vision. Please take time to read information on the official website or on tutorials pages.

Today (April, 2013), only OpenCV 2.3 is available for RPI. Unfortunatly, face reco API is only available on version 2.4. Thus, we will need to install OpenCV in two steps.

<b> 2.1 Install OpenCV 2.3 </b>

Install both dev lib and python lib. My soft is C-written. Anyway, Python is still usefull for small scripts. I recommend to install it.

sudo apt-get update<br>
sudo apt-get install libopencv-dev<br>
sudo apt-get install python-opencv<br>

<b> 3.2 install face recognition API</b>

The face recognition API is called libfacerec-0.04. All information and doc can be found on this excellent website.

Download the zip file here. https://github.com/bytefish/libfacerec/zipball/v0.04

I unzip it on my mac and transfer the whole directory on my rpi.

Go on the directory and just compile it using

cmake .
make

Now, if you want to compile your previous sample with this libfacerec-004 api, you will need to modify your CMakeLists.txt file. It just link with the libface lib. Note :  Replace /home/pi/pierre/ path by the path where you copied the libface directory. In Mike's example, this is located in /home/pi/ so the library will be located at /home/pi/facereclib.

Example from Pierre's blog
cmake_minimum_required(VERSION 2.8)
project( reco)
find_package( OpenCV REQUIRED )
add_executable( displayimage display_image.cpp )
link_directories( /home/pi/pierre/libfacerec-0.04 )
target_link_libraries( displayimage /home/pi/pierre/libfacerec-0.04/libopencv_facerec.a ${OpenCV_LIBS} )

home/pi/pierre/libracerec-0.04 is replaced with home/pi/facereclib

<b> Data Files</b>

I have uploaded about 10 pictures of myself and 9 pictures of Joep from the internet as initial subjects. These are located in the /data/ folder.
Note that these files all have to be the same size and have the eyes aligned which is handled by the python scripts below.

<b> Python Scripts<b>

<i>create_csv.py</i> - This script will create the .csv (or text) file for using as input to the face recognition software.<br>
<i>align_images.py</i> - This script makes converts the files to a specified size and aligns the eyes. The user inputs the left and right eye locations, the cropping, and overall size of the file.<br>
