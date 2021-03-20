#include <iostream>
#include <string>
#include <vector>
#include <hmath.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

int main()
{
    cv::Vec2i aspect_ratio = { 16,9 };

    int grid_scale = 30;
    int img_scale = 400;

    cv::Size image_resolution = { aspect_ratio[0] * img_scale,aspect_ratio[1] * img_scale };
    cv::Size grid_resolution = { aspect_ratio[0] * grid_scale,aspect_ratio[1] * grid_scale };

    hmath::
}