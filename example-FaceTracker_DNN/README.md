This is a version of the FaceTracker using DNN, linked with a version of dlib statically compiled with CUDA on Linux.

Instructions:
[Install CUDA](http://developer.download.nvidia.com/compute/cuda/7.5/Prod/docs/sidebar/CUDA_Installation_Guide_Linux.pdf),
[Compile and install dlib](http://dlib.net/compile.html), then compile FaceTracker; to run properly, it requires this file, uncompressed in the bin/data folder: http://dlib.net/files/mmod_human_face_detector.dat.bz2

Note: it is not required to use the static dlib; the shared dlib library would work as well, 
provided that dlib is compiled and installed on the computer running it. 
