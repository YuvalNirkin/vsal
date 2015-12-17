/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "VideoStream_uEye.h"
#include <exception>
#include <opencv2\imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <uEye.h>
#include <iostream>//

/************************************************************************************
*									  Namespaces									*
************************************************************************************/
using std::exception;

/************************************************************************************
*									 Declarations									*
************************************************************************************/


/************************************************************************************
*									Implementation									*
************************************************************************************/
namespace og
{
    VideoStream_uEye::VideoStream_uEye() :
        VideoStream_uEye(0, 0, 0)
    {
    }

    VideoStream_uEye::VideoStream_uEye(int frameWidth, int frameHeight) :
        VideoStream_uEye(0, frameWidth, frameHeight)
    {
    }

    VideoStream_uEye::VideoStream_uEye(int device, int frameWidth, int frameHeight) :
        mRequestedWidth(frameWidth), mRequestedHeight(frameHeight),
        mDevice(device),
        mTimestamp(0),
        mCamHandle(0),
        mMemoryID(0),
        mFrameCounter(0),
        mUpdated(false)
    {
        int res = IS_SUCCESS;

        if ((res = is_InitCamera(&mCamHandle, 0)) != IS_SUCCESS)
            throw exception("Failed to initialize uEye camera!");

        SENSORINFO sInfo;
        is_GetSensorInfo(mCamHandle, &sInfo);

        
 //       if ((res = is_LoadParameters(mCamHandle, "Default.ucp")) != IS_SUCCESS)
 //           throw exception("Failed to load parameters!");
            
        
        if ((res = is_SetColorMode(mCamHandle, IS_CM_MONO8)) != IS_SUCCESS)
            throw exception("Failed to set color mode!");
        /*
        int nAOISupported = 0;
        if ((res = is_ImageFormat(mCamHandle, IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED, (void*)&nAOISupported, sizeof(nAOISupported))) != IS_SUCCESS)
            throw exception("Failed to set image format!");
        
        if ((res = is_SetImageSize(mCamHandle, frameWidth, frameHeight)) != IS_SUCCESS)
            throw exception("Failed to set color mode!");
       */       

        allocateSequence();

        /*
        char* ppcImgMem;
        //int pid;
        //if ((res = is_AllocImageMem(mCamHandle, frameWidth, frameHeight, 8, &ppcImgMem, &pid)) != IS_SUCCESS)
        if ((res = is_AllocImageMem(mCamHandle, sInfo.nMaxWidth, sInfo.nMaxHeight, 8, &ppcImgMem, &mMemoryID)) != IS_SUCCESS)
            throw exception("Failed to allocate image memory!");

        if ((res = is_SetImageMem(mCamHandle, ppcImgMem, mMemoryID)) != IS_SUCCESS)
            throw exception("Failed to set image memory!");

        //mFrame.create(sInfo.nMaxHeight, sInfo.nMaxWidth, CV_8U);

        mBuffer = cv::Mat(sInfo.nMaxHeight, sInfo.nMaxWidth, CV_8U, ppcImgMem);
        */
        int pixelClock = 86;
        if (res = is_PixelClock(mCamHandle, IS_PIXELCLOCK_CMD_SET, (void*)&pixelClock, sizeof(int)) != IS_SUCCESS)
            throw exception("Failed to set pixel clock!");

        //set auto settings
        double on = 1;
        double empty;
        /*
        if ((res = is_SetAutoParameter(mCamHandle, IS_SET_ENABLE_AUTO_WHITEBALANCE, &on, &empty)) != IS_SUCCESS)
            throw exception("Failed to set color mode!");
        if ((res = is_SetAutoParameter(mCamHandle, IS_SET_ENABLE_AUTO_GAIN, &on, &empty)) != IS_SUCCESS)
            throw exception("Failed to set color mode!");
        */
        //if ((res = is_SetAutoParameter(mCamHandle, IS_SET_ENABLE_AUTO_FRAMERATE, &on, &empty)) != IS_SUCCESS)
        //    throw exception("Failed to auto framerate!");

        if ((res = is_SetAutoParameter(mCamHandle, IS_SET_ENABLE_AUTO_SHUTTER, &on, &empty)) != IS_SUCCESS)
            throw exception("Failed to set color mode!");

        if ((res = is_SetDisplayMode(mCamHandle, IS_SET_DM_DIB)) != IS_SUCCESS)
            throw exception("Failed to set display mode!");
    }

    VideoStream_uEye::~VideoStream_uEye()
    {
        close();
    }

    bool VideoStream_uEye::open()
    {
        int res = IS_SUCCESS;
        if ((res = is_CaptureVideo(mCamHandle, IS_WAIT)) != IS_SUCCESS) // IS_WAIT, IS_DONT_WAIT
            throw exception("Failed to start capturing video!");

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
        is_GetFramesPerSecond(mCamHandle, &fps);
        return fps;
    }

    void VideoStream_uEye::getFrameData(unsigned char* data) const
    {
        memcpy(data, mFrame.data, mFrame.total() * mFrame.elemSize());
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

    void VideoStream_uEye::allocateSequence()
    {
        int res = IS_SUCCESS;

        SENSORINFO sInfo;
        is_GetSensorInfo(mCamHandle, &sInfo);

        for (int i = 0; i < SEQUENCE_SIZE; ++i)
        {
            // Allocate buffer memory
            if ((res = is_AllocImageMem(mCamHandle, sInfo.nMaxWidth, sInfo.nMaxHeight, 8, &m_pcSeqImgMem[i], &m_lSeqMemId[i])) != IS_SUCCESS)
                throw exception("Failed to allocate image memory!");

            // Put memory into seq buffer
            if ((res = is_AddToSequence(mCamHandle, m_pcSeqImgMem[i], m_lSeqMemId[i])) != IS_SUCCESS)
                throw exception("Failed to add to sequence!");
            m_nSeqNumId[i] = i + 1;   // store sequence buffer number Id
        }

        mFrame.create(sInfo.nMaxHeight, sInfo.nMaxWidth, CV_8U);
    }

    void VideoStream_uEye::checkError(int err)
    {
        INT err2 = IS_SUCCESS;
        IS_CHAR* msg;
        if (err != IS_SUCCESS) {
            if (mCamHandle != 0) {
                is_GetError(mCamHandle, &err2, &msg);
                if (err2 != IS_SUCCESS) {
                    throw exception(msg);
                }
            }
            else {
                throw exception("Camera failed to initialize");
            }
        }
    }

}	// namespace og