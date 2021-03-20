#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

int main()
{
    cv::Mat pepper_1 = cv::imread("../pepper.bmp");
    cv::Mat pepper_2 = cv::imread("../pepper2.bmp");

    double beta;

    cv::Mat blended;
    
    for (double alpha = 1.0; alpha >= 0.0; alpha -= 0.01)
    {
        beta = 1 - alpha;

        cv::addWeighted(pepper_1, alpha, pepper_2, beta, 0.0, blended);

        cv::imshow("Blended", blended);
        cv::waitKey(10);
    }
    
    return 0;   
}