cmake_minimum_required(VERSION 2.8)
project( camcv_vid1 )

SET(COMPILE_DEFINITIONS -Werror)

#OPENCV
find_package( OpenCV REQUIRED )

link_directories( /home/pi/facereclib )

include_directories(/opt/vc/userland/host_applications/linux/libs/bmc_host/include)
include_directories(/opt/vc/userland/interface/vcos)
include_directories(/opt/vc/userland)
include_directories(/opt/vc/userland/interface/vcos/pthreads)
include_directories(/opt/vc/userland/interface/vmcs_host/linux)
include_directories(/opt/vc/include)

add_executable(camcv_vid1 RaspiCamControl.c RaspiCLI.c RaspiPreview.c camcv_vid1.cpp)
target_link_libraries(camcv_vid1 /opt/vc/lib/libmmal_core.so
/opt/vc/lib/libmmal_util.so /opt/vc/lib/libmmal_vc_client.so
/opt/vc/lib/libvcos.so /opt/vc/lib/libbcm_host.so /home/pi/facereclib/libopencv_facerec.a ${OpenCV_LIBS})

