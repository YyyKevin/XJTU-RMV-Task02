#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(){
    Mat image = imread("../resources/test_image.png"); //读取图片存储为Mat


    if (image.empty()) {
        cout << "无法读取图像!" << endl;
        return -1;  
    } else {
        cout << "读取图像成功!" << endl;
    }

    //1.图像颜色空间转换
    //转化为灰度图
    Mat imageGray;
    cvtColor(image, imageGray, COLOR_BGRA2GRAY);
    //转化为 HSV 图片
    Mat imageHSV;
    cvtColor(image, imageHSV,COLOR_BGR2HSV);

    //2.应用各种滤波操作      
    //应用均值滤波
    Mat imageBlur;
    blur(image,imageBlur,Size(10,10),Point(-1,-1));
    //应用高斯滤波
    Mat imageGaussianBlur;
    GaussianBlur(image,imageGaussianBlur,Size(25,25),0,0);

    //3.特征提取
    //基于 HSV 颜色空间提取红色区域
    Mat imageRed = Mat::zeros(image.size(), imageHSV.type());  // 创建一个与原图相同尺寸的空图像
    for(int i = 0; i < image.rows; ++i){
        for(int j = 0; j < image.cols; ++j){
            // 提取当前像素的 HSV 值
            Vec3b hsvPixel = imageHSV.at<Vec3b>(i,j);
            int h = hsvPixel[0];
            int s = hsvPixel[1];
            int v = hsvPixel[2];
            if ((h <= 10 || (h >= 156 && h <= 180)) && s >= 43 && s <= 255 && v >= 46 && v <= 255) {
                imageRed.at<Vec3b>(i,j) = image.at<Vec3b>(i,j);  // 保留红色区域
            } else {
                imageRed.at<Vec3b>(i,j) = Vec3b(0, 0, 0);  // 将其他区域设为黑色
            }
        }
    }
    //找出轮廓并绘制
    Mat imageRedGray;
    cvtColor(imageRed, imageRedGray, COLOR_BGR2GRAY);//将红色区域转换为灰度图  

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(imageRedGray, contours, hierarchy,RETR_CCOMP, CHAIN_APPROX_NONE,Point(0,0));

    Mat imageContour = Mat::zeros(image.size(), image.type());
    drawContours(imageContour, contours, -1, Scalar(255,255,255),2,LINE_8,hierarchy);  // 绘制轮廓

    //找到boundingbox并绘制
    double areaSum;
    Mat imageBoundingbox=image.clone();
    for(int i = 0; i < contours.size(); ++i){
        Rect boundingBox = boundingRect(contours[i]);
        rectangle(imageBoundingbox, boundingBox, Scalar(0,255,0),2);
        //计算轮廓的面积
        double area = contourArea(contours[i]);
        areaSum+=area;
    }
    cout<<"总面积为："<<areaSum<<endl;

    //提取高亮颜色区域并进行图形学处理
    //二值化
    Mat imageBinary;
    threshold(imageGray,imageBinary, 125, 255, THRESH_BINARY); 
    //膨胀
    Mat imageDilated;
    dilate(imageBinary, imageDilated, Mat());                   
    //腐蚀
    Mat imageEroded;
    erode(imageBinary, imageEroded,Mat());
    //漫水处理
    Mat imageFloodfill = imageEroded.clone();
    floodFill(imageFloodfill,Point(0,0),Scalar(255));

    //4.图像绘制
    Mat imageDrawing = Mat::zeros(image.size(), image.type());
    circle(imageDrawing, Point(900, 300), 150, Scalar(255, 0, 0), 3);  //绘制圆
    rectangle(imageDrawing, Point(200, 200), Point(600, 600), Scalar(0, 255, 0), 2); //绘制正方形
    putText(imageDrawing,"I don't like OpenCV!",Point(100,900),FONT_HERSHEY_SIMPLEX,5,Scalar(255,255,255),4);
    
    //5.对图像进行处理
    //将图像旋转35度
    double angle = 35;
    Point2f center((image.cols - 1) / 2.0, (image.rows - 1) / 2.0);
    Mat rotateImage = getRotationMatrix2D(center, angle, 1.0);
    Mat imageRotate;
    warpAffine(image,imageRotate,rotateImage, image.size());
    //将图像裁减为左上角1/4
    Mat imageCuting = Mat::zeros(Size(image.cols/2, image.rows/2), image.type());
    for(int i = 0; i < image.rows/2; ++i){
        for(int j = 0; j < image.cols/2; ++j){
            imageCuting.at<Vec3b>(i, j) = image.at<Vec3b>(i, j);
        }
    }   
    
    //保存图片
    imwrite("../resources/imagegray.png",imageGray);
    imwrite("../resources/imageHSV.png",imageHSV);
    imwrite("../resources/imageBlur.png",imageBlur);
    imwrite("../resources/imageGaussianBlur.png",imageGaussianBlur);
    imwrite("../resources/imageRed.png",imageRed);
    imwrite("../resources/imageContour.png",imageContour);
    imwrite("../resources/imageBoundingbox.png",imageBoundingbox);
    imwrite("../resources/imageBinary.png",imageBinary);
    imwrite("../resources/imageDilated.png",imageDilated);
    imwrite("../resources/imageEroded.png",imageEroded);
    imwrite("../resources/imageFloodfill.png",imageFloodfill);
    imwrite("../resources/imageDrawing.png",imageDrawing);
    imwrite("../resources/imageCuting.png",imageCuting);

    return 0;
}
