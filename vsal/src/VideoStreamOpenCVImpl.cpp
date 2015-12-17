/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "VideoStreamOpenCVImpl.h"
#include <opencv2\imgproc.hpp>

/************************************************************************************
*									  Namespaces									*
************************************************************************************/

/************************************************************************************
*									 Declarations									*
************************************************************************************/


/************************************************************************************
*									Implementation									*
************************************************************************************/
namespace vsal
{
    VideoStreamOpenCVImpl::VideoStreamOpenCVImpl(int device, int frameWidth, int frameHeight) :
        mRequestedWidth(frameWidth), mRequestedHeight(frameHeight),
        mDevice(device),
        mFPS(0),
        mTimestamp(0)
    {
    }

    VideoStreamOpenCVImpl::VideoStreamOpenCVImpl(const std::string& filePath) :
        mRequestedWidth(0), mRequestedHeight(0),
        mDevice(0),
        mVideoFile(filePath),
        mFPS(0),
        mTimestamp(0)
    {
    }

    VideoStreamOpenCVImpl::~VideoStreamOpenCVImpl()
    {
        close();
    }

    bool VideoStreamOpenCVImpl::open()
    {
        if (mVideoFile.empty())
        {
            mCap.set(cv::CAP_PROP_FRAME_WIDTH, (double)mRequestedWidth);
            mCap.set(cv::CAP_PROP_FRAME_HEIGHT, (double)mRequestedHeight);
            if (!mCap.open(mDevice)) return false;
        }
        else if (!mCap.open(mVideoFile)) return false;

        // Get properties
        int width = (int)mCap.get(cv::CAP_PROP_FRAME_WIDTH);
        int height = (int)mCap.get(cv::CAP_PROP_FRAME_HEIGHT);
        mFPS = mCap.get(cv::CAP_PROP_FPS);

        // Initialize frame
        mFrame.create(height, width, CV_8UC3);

        return true;
    }

    void VideoStreamOpenCVImpl::close()
    {
        if (mCap.isOpened()) mCap.release();
    }

    bool VideoStreamOpenCVImpl::read()
    {
        mTimestamp = mCap.get(cv::CAP_PROP_POS_MSEC)*1e-3;
        return mCap.read(mFrame);
    }


    int VideoStreamOpenCVImpl::getWidth() const
    {
        return mFrame.cols;
    }

    int VideoStreamOpenCVImpl::getHeight() const
    {
        return mFrame.rows;
    }

    double VideoStreamOpenCVImpl::getTimestamp() const
    {
        return mTimestamp;
    }

    double VideoStreamOpenCVImpl::getFPS() const
    {
        return mFPS;
    }

    void VideoStreamOpenCVImpl::getFrameData(unsigned char* data) const
    {
        memcpy(data, mFrame.data, mFrame.total() * mFrame.elemSize());
    }

    bool VideoStreamOpenCVImpl::isUpdated()
    {
        return true;
    }

    cv::Mat VideoStreamOpenCVImpl::getFrame()
    {
        return mFrame;
    }

    cv::Mat VideoStreamOpenCVImpl::getFrameGrayscale()
    {
        cv::Mat gray;
        cv::cvtColor(mFrame, gray, cv::COLOR_BGR2GRAY);
        return gray;
    }

}	// namespace vsal