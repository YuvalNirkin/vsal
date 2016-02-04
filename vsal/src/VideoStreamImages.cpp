/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "VideoStreamImages.h"
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

/************************************************************************************
*									  Namespaces									*
************************************************************************************/
using namespace boost::filesystem;

/************************************************************************************
*									 Declarations									*
************************************************************************************/


/************************************************************************************
*									Implementation									*
************************************************************************************/
namespace vsal
{
    VideoStreamImages::VideoStreamImages(const std::string& dirPath, double fps) :
        mDirPath(dirPath),
        mWidth(0), mHeight(0),
        mCurrFrameIndex(0),
        mFPS(fps),
        mDt(1.0/fps),
        mFirstFrame(true)
    {
    }

    VideoStreamImages::~VideoStreamImages()
    {
        close();
    }

    bool VideoStreamImages::open()
    {
        close();
        getFrames(mDirPath, mFrames);
        if (mFrames.empty()) return false;

        // Read the first frame to extract meta data  
        mFrame = cv::imread(mFrames[0]);
        return true;
    }

    void VideoStreamImages::close()
    {
        if (!mFrame.empty()) mFrames.clear();
        mFirstFrame = true;
        mCurrFrameIndex = 0;
        mFrame.release();
    }

    bool VideoStreamImages::read()
    {
        if (mFrames.empty()) return false;
        if (mFirstFrame) // First frame (we already read it)
        {
            mFirstFrame = false;
            return true;
        }
        if ((mCurrFrameIndex + 1) >= mFrames.size()) return false;

        // Read next frame
        mFrame = cv::imread(mFrames[++mCurrFrameIndex]);
        return true;
    }

    int VideoStreamImages::getWidth() const
    {
        return mFrame.cols;
    }

    int VideoStreamImages::getHeight() const
    {
        return mFrame.rows;
    }

    double VideoStreamImages::getTimestamp() const
    {
        return mCurrFrameIndex*mDt;
    }

    double VideoStreamImages::getFPS() const
    {
        return mFPS;
    }

    void VideoStreamImages::getFrameData(unsigned char* data) const
    {
        memcpy(data, mFrame.data, mFrame.total() * mFrame.elemSize());
    }

    bool VideoStreamImages::isUpdated()
    {
        return true;
    }

    cv::Mat VideoStreamImages::getFrame()
    {
        return mFrame;
    }

    cv::Mat VideoStreamImages::getFrameGrayscale()
    {
        cv::Mat gray;
        cv::cvtColor(mFrame, gray, cv::COLOR_BGR2GRAY);
        return gray;
    }

    void VideoStreamImages::getFrames(const std::string& dirPath,
        std::vector<std::string>& frames) const
    {
        boost::regex filter("(.*\\.(png|jpg))"); // png jpg
        boost::smatch what;
        //for (recursive_directory_iterator it(sDir); it != recursive_directory_iterator(); ++it)
        directory_iterator end_itr; // Default ctor yields past-the-end
        for (directory_iterator it(dirPath); it != end_itr; ++it)
        {
            // Skip if not a file
            if (!boost::filesystem::is_regular_file(it->status())) continue;

            // Skip if no match
            if (!boost::regex_match(it->path().leaf().string(), what, filter)) continue;

            frames.push_back(it->path().string());
        }
    }

}	// namespace vsal