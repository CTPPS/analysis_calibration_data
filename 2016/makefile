all: reference_reconstruction_example

reference_reconstruction_example : reference_reconstruction_example.cc track_lite.h alignment.h fill_info.h proton_reconstruction.h
	g++ -O3 -g -Wall -Wextra -Wno-attributes --std=c++11\
			`root-config --libs` `root-config --cflags` \
			-I"$(CMSSW_RELEASE_BASE)/src" \
			-L"$(CMSSW_RELEASE_BASE)/lib/slc6_amd64_gcc530" \
			-lDataFormatsFWLite \
			-lDataFormatsCommon \
			reference_reconstruction_example.cc -o reference_reconstruction_example
