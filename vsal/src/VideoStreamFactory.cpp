/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "vsal/VideoStreamFactory.h"
#ifndef __ANDROID__
#include "VideoStreamOpenCVImpl.h"
#include "VideoStreamImages.h"
#endif // !__ANDROID__

#include <vector>
#include <algorithm>
#include <iostream>
#ifndef __ANDROID__
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#endif // !__ANDROID__
#include <opencv2/imgproc.hpp>

#if SUPPORT_UEYE
#include "uEye/VideoStream_uEye.h"
#include <uEye.h>
#endif

/************************************************************************************
*									  Namespaces									*
************************************************************************************/
using std::vector;
#ifndef __ANDROID__
using namespace boost::filesystem;
using namespace boost::program_options;
#endif // !__ANDROID__

/************************************************************************************
*									 Declarations									*
************************************************************************************/


/************************************************************************************
*									Implementation									*
************************************************************************************/
namespace vsal
{
    VideoStreamFactory::VideoStreamFactory()
    {
    }

    VideoStreamFactory::~VideoStreamFactory()
    {
    }

    VideoStreamFactory& VideoStreamFactory::getInstance()
    {
        static VideoStreamFactory instance;
        return instance;
    }

#if SUPPORT_UEYE
    int uEyeCameraCount()
    {
        int num = 0;
        if (is_GetNumberOfCameras(&num) != IS_SUCCESS)
            return 0;
        return num;
    }

    unsigned int uEyeListCameras(std::vector<unsigned int>& serial, std::vector<unsigned int>& dev_id)
    {
        int num = uEyeCameraCount();
        if (num > 0) {
            UEYE_CAMERA_LIST *list = (UEYE_CAMERA_LIST *)malloc(sizeof(DWORD) + num * sizeof(UEYE_CAMERA_INFO));
            list->dwCount = num;
            if (is_GetCameraList(list) == IS_SUCCESS) {
                num = list->dwCount;
                serial.resize(num);
                dev_id.resize(num);
                for (int i = 0; i < num; i++) {
                    serial[i] = atoll(list->uci[i].SerNo);
                    dev_id[i] = list->uci[i].dwDeviceID;
                }
            }
            else {
                num = 0;
            }
            free(list);
            return num;
        }
        return 0;
    }

    VideoStream* VideoStreamFactory::create(int device, int frameWidth, int frameHeight)
    {
        std::vector<unsigned int> ueye_serial, ueye_dev_id;
        unsigned int camCount = uEyeListCameras(ueye_serial, ueye_dev_id);

        if (camCount > 0)
        {
            if (device > 0)
            {
                if (std::find(ueye_dev_id.begin(), ueye_dev_id.end(), device) != ueye_dev_id.end())
                    return new VideoStream_uEye(device, frameWidth, frameHeight);
                else return new VideoStreamOpenCVImpl(device, frameWidth, frameHeight);
            }
            else return new VideoStream_uEye(device, frameWidth, frameHeight);
        }
        else return new VideoStreamOpenCVImpl(device, frameWidth, frameHeight);
    }
#elif __ANDROID__
    VideoStream* VideoStreamFactory::create(int device, int frameWidth, int frameHeight)
    {
		return nullptr;
    }
#else
	VideoStream* VideoStreamFactory::create(int device, int frameWidth, int frameHeight)
	{
		return new VideoStreamOpenCVImpl(device, frameWidth, frameHeight);
	}
#endif

#if __ANDROID__
	VideoStream* VideoStreamFactory::create(const std::string& path)
	{
		return nullptr;
	}

	VideoStream* VideoStreamFactory::create(int argc, char** argv)
	{
		return nullptr;
	}

	VideoStream* VideoStreamFactory::create(const std::string& path, int device,
		int frameWidth = 0, int frameHeight = 0)
	{
		return nullptr;
	}
#else
	VideoStream* VideoStreamFactory::create(const std::string& path)
	{
		if (is_regular_file(path))
		{
			if(VideoStreamImages::is_image(path)) return new VideoStreamImages(path);
			else return new VideoStreamOpenCVImpl(path);
		}
		else return new VideoStreamImages(path);
	}

	VideoStream* VideoStreamFactory::create(const std::string& path, int device,
		int frameWidth, int frameHeight)
	{
		// Create video source
		if (device >= 0) return create(device, frameWidth, frameHeight);
		else if (!path.empty()) return create(path);
		else return nullptr;
	}

	VideoStream* VideoStreamFactory::create(int argc, char** argv)
	{
		// Parse command line arguments
		std::string inputPath;
		int device;
		unsigned int width, height;
		double fps;
		try {
			options_description desc("Allowed options");
			desc.add_options()
				("help", "display the help message")
				("input,i", value<std::string>(&inputPath), "input path")
				("device,d", value<int>(&device)->default_value(-1),
					"device id")
				("width,w", value<unsigned int>(&width)->default_value(640),
					"frame width")
				("height,h", value<unsigned int>(&height)->default_value(480),
					"frame height")
				("fps,f", value<double>(&fps)->default_value(30.0),
					"frames per second")
				;
			variables_map vm;
			store(command_line_parser(argc, argv).options(desc).
				positional(positional_options_description().add("input", -1)).run(), vm);
			if (vm.count("help")) {
				std::cout << "Usage: preview [options]" << std::endl;
				std::cout << desc << std::endl;
				return nullptr;
			}
			notify(vm);
		}
		catch (const error& e) {
			std::cout << "Error while parsing command-line arguments: " << e.what() << std::endl;
			std::cout << "Use --help to display a list of options." << std::endl;
			return nullptr;
		}

		// Create video source
		return create(inputPath, device, width, height);
	}
#endif  

}	// namespace vsal