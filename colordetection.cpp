#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(int, char)
{
   VideoCapture cap(0);      // capture the video from webcam

   if (!cap.isOpened()) {      // if not success, exit program
      cout << "Cannot open the web cam" << endl;
      return -1;
   }

   namedWindow("Control", WINDOW_AUTOSIZE);      // create a window called "Control"

   int iLowH = 0;
   int iHighH = 179;

   int iLowS = 0;
   int iHighS = 255;

   int iLowV = 0;
   int iHighV = 40;

   //Create trackbars in "Control", window
   createTrackbar("LowH", "Control", &iLowH, 179);// Hue (0-179)
   createTrackbar("HighH", "Control", &iHighH, 179);

   createTrackbar("LowS", "Control", &iLowS, 255);   // Saturation(0-255)
   createTrackbar("highS", "Control", &iHighS, 255);

   createTrackbar("LowV", "Control", &iLowV, 255);      // Value(0-255)
   createTrackbar("HighV", "Control", &iHighV, 255);


   int iLastX = -1;
   int iLastY = -1;

   //Captrue a temporary image from the camera
   Mat imgTmp;
   cap.read(imgTmp);

   //Create a black image with the size as the camera output
   Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);


   while (true) {
      Mat imgOriginal;

      bool bSuccess = cap.read(imgOriginal);      // read a new frame from video

      if (!bSuccess)
      {
         cout << "Cannot read a frame from video stream" << endl;
         break;
      }

      Mat imgHSV;

      cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

      Mat imgThresholded;

      inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

      /*
      첫번째 이미지 처리
      */
      //morphological opening (removes small objects from the foreground)
      erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
      dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

      // morphological closing (removes small holes from the foreground)
      dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
      erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

      //Calculate the moments of the thresholed image
      Moments oMoments = moments(imgThresholded);

      double dM01 = oMoments.m01;
      double dM10 = oMoments.m10;
      double dArea = oMoments.m00;

      // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
      if (dArea > 10000)
      {
         //calculate the position of the ball
         int posX = dM10 / dArea;
         int posY = dM01 / dArea;

         if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
         {
            //Draw a red line from the previous point to the current point
            line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
         }

         iLastX = posX;
         iLastY = posY;
      }
      imshow("Thresholeded Image", imgThresholded);

      imgOriginal = imgOriginal + imgLines;
      imshow("Original", imgOriginal);

      if (waitKey(30) == 27) {
         cout << "esc key is pressed by user" << endl;
         break;

      }
   }

   return 0;
}