<html>
<body>
RPi-OpenCV-Face-Rec
===================

Simple implementation of OpenCV Face Recognition from Pierre at http://thinkrpi.wordpress.com/2013/05/22/opencv-and-camera-board-csi/

Note that this version uses the RPi 5MP Camera Module and not a USB or Wireless Network Camera.


Pre-Requisites (taken from http://thinkrpi.wordpress.com/2013/04/05/step-3-install-softwares-for-webcam-and-computer-vision/)

<b> 1. Install CMake </b>

CMake is mandatory to compile.

<pre class="code-text-only" style="display: none;">
<code>sudo apt-get update</code><br>
<code>sudo apt-get install cmake</code>
</pre>

<b> 2. Install OpenCV Lib for face recognition </b>

OpenCV is a wonderful library to do computer vision. Please take time to read information on the official website or on tutorials pages.

Today (April, 2013), only OpenCV 2.3 is available for RPI. Unfortunately, face reco API is only available on version 2.4. Thus, we will need to install OpenCV in two steps.

<b> 3. Install OpenCV 2.3 </b>

Install both dev lib and python lib. The software is written in C but Python is still useful for small scripts.

<pre class="code-text-only" style="display: none;">
sudo apt-get update
sudo apt-get install libopencv-dev
sudo apt-get install python-opencv
</pre>

<b> 4. install face recognition API</b>

The face recognition API is called libfacerec-0.04. All information and doc can be found on this excellent website.

Download the zip file here. https://github.com/bytefish/libfacerec/zipball/v0.04

Unzip it locally on PC and transfer the whole directory to RPi location, <b><i>home/pi/facereclib</i></b>.

Go to the directory and compile it using

<pre class="code-text-only" style="display: none;">
cmake .
make
</pre>

Now, if you want to compile your previous sample with this libfacerec-004 api, you will need to modify your CMakeLists.txt file. It just link with the libface lib.
<br>Note : Replace <b>/home/pi/pierre/</b> path by the path where you copied the libface directory. In Mike's example, this is located in /home/pi/ so the library will be located at /home/pi/facereclib.


Example from Pierre's blog <br>
<pre class="code-text-only" style="display: none;">
cmake_minimum_required(VERSION 2.8)
project( <b>camcv</b> )
SET(COMPILE_DEFINITIONS -Werror)
#OPENCV
find_package( OpenCV REQUIRED )

#except if you.re pierre, change the folder where you installed libfacerec
#optional, only if you want to go till step 6 : face recognition
link_directories( <b><i>/home/pi/pierre/libfacerec-0.04</i></b> ) 

include_directories(/opt/vc/userland/host_applications/linux/libs/bcm_host/include)
include_directories(/opt/vc/userland/interface/vcos)
include_directories(/opt/vc/userland)
include_directories(/opt/vc/userland/interface/vcos/pthreads)
include_directories(/opt/vc/userland/interface/vmcs_host/linux)
add_executable(<b>camcv</b> RaspiCamControl.c RaspiCLI.c RaspiPreview.c camcv.c)
target_link_libraries(camcv /opt/vc/lib/libmmal_core.so /opt/vc/lib/libmmal_util.so /opt/vc/lib/libmmal_vc_client.so /opt/vc/lib/libvcos.so /opt/vc/lib/libbcm_host.so <b><i>/home/pi/pierre/libfacerec-0.04/libopencv_facerec.a</i></b> ${OpenCV_LIBS})
</pre>
<br>
<i><b>home/pi/pierre/libracerec-0.04</b></i> is replaced with <i><b>home/pi/facereclib</b></i>
<br>
Example from Mike's code<br>
<pre class="code-text-only" style="display: none;">
cmake_minimum_required(VERSION 2.8)
project( <b>camcv_vid1</b> )

SET(COMPILE_DEFINITIONS -Werror)

#OPENCV
find_package( OpenCV REQUIRED )

link_directories( <b><i>/home/pi/facereclib</i></b> )

include_directories(/opt/vc/userland/host_applications/linux/libs/bmc_host/include)
include_directories(/opt/vc/userland/interface/vcos)
include_directories(/opt/vc/userland)
include_directories(/opt/vc/userland/interface/vcos/pthreads)
include_directories(/opt/vc/userland/interface/vmcs_host/linux)
include_directories(/opt/vc/include)

add_executable(<b>camcv_vid1</b> RaspiCamControl.c RaspiCLI.c RaspiPreview.c camcv_vid1.cpp)
target_link_libraries(camcv_vid1 /opt/vc/lib/libmmal_core.so
/opt/vc/lib/libmmal_util.so /opt/vc/lib/libmmal_vc_client.so
/opt/vc/lib/libvcos.so /opt/vc/lib/libbcm_host.so <b><i>/home/pi/facereclib/libopencv_facerec.a</i></b> ${OpenCV_LIBS})
</pre>
<br>
<b> Data Files</b>

I have uploaded about 10 pictures of myself and 9 pictures of Joep from the internet as initial subjects. These are located in the /data/ folder.
Note that these files all have to be the same size and have the eyes aligned which is handled by the python scripts below.

<b> Python Scripts<b>

<b><i>create_csv.py</i></b> - This script will create the .csv (or text) file for using as input to the face recognition software.<br>
<b><i>align_images.py</i></b> - This script converts the files to a specified size and aligns the eyes. The user inputs the left and right eye locations, the cropping, and overall size of the file.<br>


<b> 5. Run the Code<br>

Make sure you are in the directory with the <b>camcv_vid1</b> executable and initiate the following command<br>
<pre class="code-text-only" style="display: none;">
<code>./camcv_vid1 facrecdata.csv 1 5500</code></pre>

</body>
</html>
