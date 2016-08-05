/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "VideoStream_uEye.h"
#include <exception>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <uEye.h>
#include <iostream>//

/************************************************************************************
*									  Namespaces									*
************************************************************************************/
//using std::exception;
using std::runtime_error;

/************************************************************************************
*									 Declarations									*
************************************************************************************/


/************************************************************************************
*									Implementation									*
************************************************************************************/
namespace vsal
{
    VideoStream_uEye::VideoStream_uEye() :
        VideoStream_uEye(0, 0, 0), mInitialized(false)
    {
    }

    VideoStream_uEye::VideoStream_uEye(int frameWidth, int frameHeight) :
        VideoStream_uEye(0, frameWidth, frameHeight), mInitialized(false)
    {
    }

    VideoStream_uEye::VideoStream_uEye(int device, int frameWidth, int frameHeight) :
        mRequestedWidth(frameWidth), mRequestedHeight(frameHeight),
        mDevice(device),
        mTimestamp(0),
        mCamHandle(0),
        mMemoryID(0),
        mFrameCounter(0),
        mUpdated(false),
		mInitialized(false)
    {
        int res = IS_SUCCESS;

        if ((res = is_InitCamera((HIDS*)&mCamHandle, 0)) != IS_SUCCESS)
            throw runtime_error("Failed to initialize uEye camera!");

        SENSORINFO sInfo;
        is_GetSensorInfo(mCamHandle, &sInfo);

        // Make sure the requested resolution is not higher than the maximum possible
        mRequestedWidth = std::min(mRequestedWidth, (int)sInfo.nMaxWidth);
        mRequestedHeight = std::min(mRequestedHeight, (int)sInfo.nMaxHeight);

 //       if ((res = is_LoadParameters(mCamHandle, "Default.ucp")) != IS_SUCCESS)
 //           throw runtime_error("Failed to load parameters!");
            
        
        if ((res = is_SetColorMode(mCamHandle, IS_CM_MONO8)) != IS_SUCCESS)
            throw runtime_error("Failed to set color mode!");
        /*
        int nAOISupported = 0;
        if ((res = is_ImageFormat(mCamHandle, IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED, (void*)&nAOISupported, sizeof(nAOISupported))) != IS_SUCCESS)
            throw runtime_error("Failed to set image format!");
        
        if ((res = is_SetImageSize(mCamHandle, frameWidth, frameHeight)) != IS_SUCCESS)
            throw runtime_error("Failed to set color mode!");
       */       

        // Set AOI
        IS_RECT cam_aoi;
        cam_aoi.s32Width = mRequestedWidth;
        cam_aoi.s32Height = mRequestedHeight;
        cam_aoi.s32X = (sInfo.nMaxWidth - cam_aoi.s32Width) / 2;
        cam_aoi.s32Y = (sInfo.nMaxHeight - cam_aoi.s32Height) / 2;
        if ((res = is_AOI(mCamHandle, IS_AOI_IMAGE_SET_AOI, &cam_aoi, sizeof(cam_aoi))) != IS_SUCCESS)
            throw runtime_error("Failed to set Area Of Interest (AOI)");

        allocateSequence();

        /*
        char* ppcImgMem;
        //int pid;
        //if ((res = is_AllocImageMem(mCamHandle, frameWidth, frameHeight, 8, &ppcImgMem, &pid)) != IS_SUCCESS)
        if ((res = is_AllocImageMem(mCamHandle, sInfo.nMaxWidth, sInfo.nMaxHeight, 8, &ppcImgMem, &mMemoryID)) != IS_SUCCESS)
            throw runtime_error("Failed to allocate image memory!");

        if ((res = is_SetImageMem(mCamHandle, ppcImgMem, mMemoryID)) != IS_SUCCESS)
            throw runtime_error("Failed to set image memory!");

        //mFrame.create(sInfo.nMaxHeight, sInfo.nMaxWidth, CV_8U);

        mBuffer = cv::Mat(sInfo.nMaxHeight, sInfo.nMaxWidth, CV_8U, ppcImgMem);
        */
        
        // Set pixel clock
        UINT pixelClockList[150];  // No camera has more than 150 different pixel clocks (uEye manual)
        UINT numberOfSupportedPixelClocks = 0;
        if ((res = is_PixelClock(mCamHandle, IS_PIXELCLOCK_CMD_GET_NUMBER,
            (void*)&numberOfSupportedPixelClocks, sizeof(numberOfSupportedPixelClocks))) != IS_SUCCESS) {
            throw runtime_error("Failed to query number of supported pixel clocks!");
        }
        if (numberOfSupportedPixelClocks > 0) {
            ZeroMemory(pixelClockList, sizeof(pixelClockList));
            if ((res = is_PixelClock(mCamHandle, IS_PIXELCLOCK_CMD_GET_LIST,
                (void*)pixelClockList, numberOfSupportedPixelClocks * sizeof(int))) != IS_SUCCESS) {
                throw runtime_error("Failed to query list of supported pixel clocks!");
            }
        }
        int minPixelClock = (int)pixelClockList[0];
        int maxPixelClock = (int)pixelClockList[numberOfSupportedPixelClocks - 1];

        int pixelClock = maxPixelClock;
        if (res = is_PixelClock(mCamHandle, IS_PIXELCLOCK_CMD_SET, (void*)&pixelClock, sizeof(int)) != IS_SUCCESS)
            throw runtime_error("Failed to set pixel clock!");
        /*
        // Set frame rate
        double minFrameTime, maxFrameTime, intervalFrameTime, newFrameRate;
        if ((res = is_GetFrameTimeRange(mCamHandle, &minFrameTime,
            &maxFrameTime, &intervalFrameTime)) != IS_SUCCESS) {
            throw runtime_error("Failed to query valid frame rate range!");
        }
        
        double maxFrameRate = 1.0 / minFrameTime;
        if ((res = is_SetFrameRate(mCamHandle, maxFrameRate, &newFrameRate)) != IS_SUCCESS)
            throw runtime_error("Failed to set frame rate!");
            */
        //set auto settings
        double on = 1;
        double empty;
        /*
        if ((res = is_SetAutoParameter(mCamHandle, IS_SET_ENABLE_AUTO_WHITEBALANCE, &on, &empty)) != IS_SUCCESS)
            throw runtime_error("Failed to set color mode!");
        if ((res = is_SetAutoParameter(mCamHandle, IS_SET_ENABLE_AUTO_GAIN, &on, &empty)) != IS_SUCCESS)
            throw runtime_error("Failed to set color mode!");
        */
        //if ((res = is_SetAutoParameter(mCamHandle, IS_SET_ENABLE_AUTO_FRAMERATE, &on, &empty)) != IS_SUCCESS)
        //    throw runtime_error("Failed to auto framerate!");

        if ((res = is_SetAutoParameter(mCamHandle, IS_SET_ENABLE_AUTO_SHUTTER, &on, &empty)) != IS_SUCCESS)
            throw runtime_error("Failed to set color mode!");

        if ((res = is_SetDisplayMode(mCamHandle, IS_SET_DM_DIB)) != IS_SUCCESS)
            throw runtime_error("Failed to set display mode!");

        // Enable hardware gamma
        setHardwareGamma(true);
    }

    VideoStream_uEye::~VideoStream_uEye()
    {
        close();
    }

    bool VideoStream_uEye::open()
    {
        int res = IS_SUCCESS;
        if ((res = is_CaptureVideo(mCamHandle, IS_WAIT)) != IS_SUCCESS) // IS_WAIT, IS_DONT_WAIT
            throw runtime_error("Failed to start capturing video!");
		mInitialized = (res == IS_SUCCESS);

        return true;
    }

    void VideoStream_uEye::close()
    {
        // Stop live video
        is_StopLiveVideo(mCamHandle, IS_WAIT);

        // Remove the buffers from the sequence
        is_ClearSequence(mCamHandle);

        // Free seq buffers
        for (int i = SEQUENCE_SIZE; i >= 0; i--)
        {
            is_FreeImageMem(mCamHandle, m_pcSeqImgMem[i], m_lSeqMemId[i]);
        }

        // Memory and events are automatically released
        is_ExitCamera(mCamHandle);

		mInitialized = false;
    }

    bool VideoStream_uEye::read()
    {
        int res = IS_SUCCESS;
        //VOID* pMem;

 //       if (is_FreezeVideo(mCamHandle, IS_WAIT) != IS_SUCCESS)
 //           return false;

//        if ((res = is_GetImageMem(mCamHandle, &pMem)) != IS_SUCCESS)
//            return false;        

        //memcpy(mFrame.data, pMem, mFrame.cols * mFrame.rows);
        //mBuffer.copyTo(mFrame);
        

        // Find the latest image buffer
        int nNum, i;
        char *pcMem, *pcMemLast;
        is_GetActSeqBuf(mCamHandle, &nNum, &pcMem, &pcMemLast);
        for (i = 0; i < SEQUENCE_SIZE; ++i)
        {
            if (pcMemLast == m_pcSeqImgMem[i])
                break;
        }

        // lock buffer for processing
        res = is_LockSeqBuf(mCamHandle, m_nSeqNumId[i], m_pcSeqImgMem[i]);

        //// start processing...................................

        UEYEIMAGEINFO ImageInfo;
        if (is_GetImageInfo(mCamHandle, m_lSeqMemId[i], &ImageInfo, sizeof(ImageInfo)) != IS_SUCCESS)
            return false;

        mTimestamp = ImageInfo.u64TimestampDevice*1e-7;
        mUpdated = mFrameCounter != ImageInfo.u64FrameNumber || mFrameCounter == 0;
        mFrameCounter = ImageInfo.u64FrameNumber;

        memcpy(mFrame.data, pcMem, mFrame.cols * mFrame.rows);

        //// processing completed................................

        // unlock buffer
        is_UnlockSeqBuf(mCamHandle, m_nSeqNumId[i], m_pcSeqImgMem[i]);

        return true;
    }

    int VideoStream_uEye::getWidth() const
    {
        return mFrame.cols;
    }

    int VideoStream_uEye::getHeight() const
    {
        return mFrame.rows;
    }

    double VideoStream_uEye::getTimestamp() const
    {
        return mTimestamp;
    }

    double VideoStream_uEye::getFPS() const
    {
        double fps;
        //is_SetFrameRate(mCamHandle, IS_GET_FRAMERATE, &fps);
        is_GetFramesPerSecond(mCamHandle, &fps);
        return fps;
    }

    void VideoStream_uEye::getFrameData(unsigned char* data) const
    {
        memcpy(data, mFrame.data, mFrame.total() * mFrame.elemSize());
    }

	bool VideoStream_uEye::isLive() const
	{
		return true;
	}

	bool VideoStream_uEye::isOpened() const
	{
		return mInitialized;
	}

    bool VideoStream_uEye::isUpdated()
    {
        return mUpdated;
    }

    cv::Mat VideoStream_uEye::getFrame()
    {
        return mFrame;
    }

    cv::Mat VideoStream_uEye::getFrameGrayscale()
    {
        return mFrame;
    }

	size_t VideoStream_uEye::getFrameIndex() const
	{
		return (size_t)mFrameCounter;
	}

	void VideoStream_uEye::seek(size_t index)
	{
	}

	size_t VideoStream_uEye::size() const
	{
		return (size_t)mFrameCounter;
	}

    void VideoStream_uEye::allocateSequence()
    {
        int res = IS_SUCCESS;

        SENSORINFO sInfo;
        is_GetSensorInfo(mCamHandle, &sInfo);

        // Query camera's current resolution settings, for redundancy
        IS_RECT cam_aoi;
        if ((res = is_AOI(mCamHandle, IS_AOI_IMAGE_GET_AOI, (void*)&cam_aoi, sizeof(cam_aoi))) != IS_SUCCESS)
            throw runtime_error("Could not retrieve Area Of Interest (AOI)!");

        for (int i = 0; i < SEQUENCE_SIZE; ++i)
        {
            // Allocate buffer memory
            if ((res = is_AllocImageMem(mCamHandle, cam_aoi.s32Width, cam_aoi.s32Height, 8, &m_pcSeqImgMem[i], &m_lSeqMemId[i])) != IS_SUCCESS)
                throw runtime_error("Failed to allocate image memory!");

            // Put memory into seq buffer
            if ((res = is_AddToSequence(mCamHandle, m_pcSeqImgMem[i], m_lSeqMemId[i])) != IS_SUCCESS)
                throw runtime_error("Failed to add to sequence!");
            m_nSeqNumId[i] = i + 1;   // store sequence buffer number Id
        }

        //mFrame.create(sInfo.nMaxHeight, sInfo.nMaxWidth, CV_8U);
        mFrame.create(cam_aoi.s32Height, cam_aoi.s32Width, CV_8U);
    }

    void VideoStream_uEye::checkError(int err)
    {
        INT err2 = IS_SUCCESS;
        IS_CHAR* msg;
        if (err != IS_SUCCESS) {
            if (mCamHandle != 0) {
                is_GetError(mCamHandle, &err2, &msg);
                if (err2 != IS_SUCCESS) {
                    throw runtime_error(msg);
                }
            }
            else {
                throw runtime_error("Camera failed to initialize");
            }
        }
    }

    bool VideoStream_uEye::getHardwareGamma()
    {
        return is_SetHardwareGamma(mCamHandle, IS_GET_HW_GAMMA);
    }

    bool VideoStream_uEye::setHardwareGamma(bool enable)
    {
        if (enable)
        {
            if (is_SetHardwareGamma(mCamHandle, IS_SET_HW_GAMMA_ON) != IS_SUCCESS)
            {
                is_SetHardwareGamma(mCamHandle, IS_SET_HW_GAMMA_OFF);
                return false;
            }
        }
        else
        {
            is_SetHardwareGamma(mCamHandle, IS_SET_HW_GAMMA_OFF);
        }

        return true;
    }

}	// namespace vsal