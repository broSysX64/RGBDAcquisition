#include "imageStream.h"

cv::Mat leftRGB,lastLeftRGB;
cv::Mat rightRGB,lastRightRGB;

cv::Mat greyLeft, greyLastLeft;
cv::Mat greyRight, greyLastRight;


unsigned int framesProcessed=0;


int passNewFrame(unsigned char * colorFrame , unsigned int colorWidth ,unsigned int colorHeight , unsigned int swapColorFeeds , unsigned int shiftYLeft , unsigned int shiftYRight )
{
cv::Mat rgbImg(colorHeight,colorWidth,CV_8UC3,colorFrame);
    //cv::Mat depthImg(depthHeight,depthWidth,CV_16UC1,depthFrame);

    std::cerr<<"Doing Left split of images..\n";

    cv::Mat tmpleftImage;
    cv::Mat tmprightImage;
    cv::Rect leftROI;
    cv::Rect rightROI;


    leftROI = cv::Rect(0,0,(rgbImg.cols/2)-1,rgbImg.rows-1);
    rightROI = cv::Rect(rgbImg.cols/2,0,(rgbImg.cols/2)-1,rgbImg.rows-1);

    if (swapColorFeeds)
    {
     tmpleftImage= cv::Mat(rgbImg,rightROI);
     tmprightImage= cv::Mat(rgbImg,leftROI);
    }else
    {
     tmpleftImage= cv::Mat(rgbImg,leftROI);
     tmprightImage= cv::Mat(rgbImg,rightROI);
    }

    cv::Mat bgrleftImage = cv::Mat::zeros(tmpleftImage.size(), tmpleftImage.type());
    if (shiftYLeft==0) { bgrleftImage=tmpleftImage;  } else
                       {
                         tmpleftImage(cv::Rect(0,shiftYLeft, tmpleftImage.cols,tmpleftImage.rows-shiftYLeft)).copyTo(bgrleftImage(cv::Rect(0,0,tmpleftImage.cols,tmpleftImage.rows-shiftYLeft)));
                       }

    cv::Mat bgrrightImage = cv::Mat::zeros(tmprightImage.size(), tmprightImage.type());
    if (shiftYRight==0) { bgrrightImage=tmprightImage;  } else
                       {
                        tmprightImage(cv::Rect(0,shiftYRight, tmprightImage.cols,tmprightImage.rows-shiftYRight)).copyTo(bgrrightImage (cv::Rect(0,0,tmprightImage.cols,tmprightImage.rows-shiftYRight)));
                       }

    cv::Mat leftImage;
    cv::Mat rightImage;


    cv::cvtColor(bgrleftImage,leftImage, cv::COLOR_RGB2BGR);
    cv::cvtColor(bgrrightImage,rightImage, cv::COLOR_RGB2BGR);


    if(framesProcessed==0)
    {
      leftRGB=bgrleftImage;
      rightRGB=bgrrightImage;
      lastLeftRGB=leftRGB;
      lastRightRGB=rightRGB;

      cv::cvtColor(bgrleftImage, greyLeft, CV_BGR2GRAY);
      cv::cvtColor(bgrrightImage, greyRight, CV_BGR2GRAY);
      greyLastLeft = greyLeft;
      greyLastRight = greyRight;

    } else
    {
      lastLeftRGB=leftRGB;
      lastRightRGB=rightRGB;
      leftRGB=bgrleftImage;
      rightRGB=bgrrightImage;

      greyLastLeft = greyLeft;
      greyLastRight = greyRight;
      cv::cvtColor(bgrleftImage, greyLeft, CV_BGR2GRAY);
      cv::cvtColor(bgrrightImage, greyRight, CV_BGR2GRAY);
    }

    ++framesProcessed;
    return 1;
};
