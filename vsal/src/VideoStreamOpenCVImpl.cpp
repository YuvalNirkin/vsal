/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "VideoStreamOpenCVImpl.h"
#include <opencv2/imgproc.hpp>

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
    VideoStreamOpenCVImpl::VideoStreamOpenCVImpl(int device, int frameWidth,
		int frameHeight, double fps) :
        mRequestedWidth(frameWidth), mRequestedHeight(frameHeight),
        mDevice(device),
        mFPS(fps),
        mTimestamp(0),
		mFrameIndex(0),
		mTotalFrames(0)
    {
    }

    VideoStreamOpenCVImpl::VideoStreamOpenCVImpl(const std::string& filePath) :
        mRequestedWidth(0), mRequestedHeight(0),
        mDevice(0),
        mVideoFile(filePath),
        mFPS(0),
        mTimestamp(0),
		mFrameIndex(0),
		mTotalFrames(0)
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
			if(mRequestedWidth > 0)
				mCap.set(cv::CAP_PROP_FRAME_WIDTH, (double)mRequestedWidth);
			if (mRequestedHeight > 0)
				mCap.set(cv::CAP_PROP_FRAME_HEIGHT, (double)mRequestedHeight);
			if(mFPS > 0) mCap.set(cv::CAP_PROP_FPS, mFPS);
            if (!mCap.open(mDevice)) return false;
        }
        else if (!mCap.open(mVideoFile)) return false;

        // Get properties
        int width = (int)mCap.get(cv::CAP_PROP_FRAME_WIDTH);
        int height = (int)mCap.get(cv::CAP_PROP_FRAME_HEIGHT);
        mFPS = mCap.get(cv::CAP_PROP_FPS);
		if (!mVideoFile.empty())
			mTotalFrames = (size_t)mCap.get(cv::CAP_PROP_FRAME_COUNT);

        // Initialize frame
        mFrame.create(height, width, CV_8UC3);

        return true;
    }

    void VideoStreamOpenCVImpl::close()
    {
        if (mCap.isOpened()) mCap.release();
		mFPS = mTimestamp = 0.0;
		mFrameIndex = mTotalFrames = 0;
    }

    bool VideoStreamOpenCVImpl::read()
    {
        mTimestamp = mCap.get(cv::CAP_PROP_POS_MSEC)*1e-3;
		if (mVideoFile.empty())
		{
			++mFrameIndex;
			++mTotalFrames;
		}
		else mFrameIndex = (size_t)mCap.get(cv::CAP_PROP_POS_FRAMES);
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

	bool VideoStreamOpenCVImpl::isLive() const
	{
		return mVideoFile.empty();
	}

	bool VideoStreamOpenCVImpl::isOpened() const
	{
		return mCap.isOpened();
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

	size_t VideoStreamOpenCVImpl::getFrameIndex() const
	{
		return mFrameIndex;
	}

	void VideoStreamOpenCVImpl::seek(size_t index)
	{
		if (mVideoFile.empty()) return;
		mCap.set(cv::CAP_PROP_POS_FRAMES, (double)index);
	}

	size_t VideoStreamOpenCVImpl::size() const
	{
		return mTotalFrames;
	}

}	// namespace vsal