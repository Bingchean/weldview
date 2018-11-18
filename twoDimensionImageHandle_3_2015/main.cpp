#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat srcImg = imread("test.jpg");
	//cv::cvtColor(srcImg, srcImg, COLOR_RGB2GRAY);//进行，灰度处理

	//Mat dstImg1, dstImg2, dstImg3, dstImg4, dstImg5;
	//Size size0 = Size(5, 5);    
	//boxFilter(srcImg, dstImg1, -1, Size(3, 3), Point(-1, -1), true);  
	//方框滤波器 bool normalize参数设置为true时等同于均值滤波   
	//blur(srcImg, dstImg2, size0);  //均值滤波    
	//GaussianBlur(srcImg, dstImg3, Size(3, 3), 1);//高斯滤波    
	//medianBlur(srcImg, dstImg4, 5);//中值滤波    
	//bilateralFilter(srcImg, dstImg5, 5, 10.0, 2.0);//双边滤波    
	//threshold(dstImg1,dstImg1, 127, 255, THRESH_BINARY);    
	//threshold(dstImg2,dstImg2, 127, 255, THRESH_BINARY);    
	//threshold(dstImg3,dstImg3, 127, 255, THRESH_BINARY);    
	//threshold(dstImg4,dstImg4, 127, 255, THRESH_BINARY);    
	//threshold(dstImg5,dstImg5, 127, 255, THRESH_BINARY);    

	//namedWindow("gray", WINDOW_NORMAL);
	//namedWindow("boxFilter",WINDOW_NORMAL);
	//namedWindow("blur", WINDOW_NORMAL);
	//namedWindow("GaussianBlur", WINDOW_NORMAL);
	//namedWindow("medianBlur", WINDOW_NORMAL);
	//namedWindow("bilateralFilter", WINDOW_NORMAL);

	/*imshow("gray", srcImg);
	imshow("boxFilter", dstImg1);
	imshow("blur", dstImg2);
	imshow("GaussianBlur", dstImg3);
	imshow("medianBlur", dstImg4);
	imshow("bilateralFilter", dstImg5);*/

	Mat blurImg;
	cvtColor(srcImg, blurImg, COLOR_RGB2GRAY);
	blur(blurImg, blurImg, Size(5, 5));
	namedWindow("blur", WINDOW_KEEPRATIO);
	imshow("blur", blurImg);

	Mat cannyResult;
	Canny(blurImg, cannyResult, 100, 200, 3);
	namedWindow("blur-canny", WINDOW_KEEPRATIO);
	imshow("blur-canny", cannyResult);

	Mat toShowColor;
	cvtColor(cannyResult, toShowColor, CV_GRAY2BGR);

	vector<Vec4i>g_lines;
	HoughLinesP(cannyResult, g_lines, 1, CV_PI / 255, 100, 200, 200);

	/*for (size_t i = 0; i < g_lines.size(); i++) 
	{
		Vec4i l = g_lines[i];
		line(DstImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(240, 26, 46), 1, CV_AA);
	}

	vector<Vec3f> circles;
	HoughCircles(cannyResult, circles, CV_HOUGH_GRADIENT, 1, blurImg.rows / 20, 100, 100, 0, 0);
	//依次在图中绘制出圆
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		//绘制圆心
		circle(DstImg, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//绘制圆轮廓
		circle(DstImg, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}

	namedWindow("over",WINDOW_FREERATIO);
	imshow("over",DstImg);

	waitKey(0);
	return 0;
	*/

	vector<Vec4i> legelLines;
	for (size_t i = 0; i<g_lines.size(); i++)
	{
		Vec4i l = g_lines[i];

		if (l[2] - l[0] != 0)
		{
			float k = (l[3] - l[1]) / (l[2] - l[0]);// (y2-y1) / (x2-x1)

			if (k < 0 )// 钝角？ //&& l[1] - l[3] > 200
			{
				//计算轮廓的矩
				//double dstLength = arcLength(g_vsrcPoints, true);
				legelLines.push_back(l);
				//line(DstImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(240, 26, 46), 1, CV_AA);
			}
		}
		else // 垂直的线
		{
			if (l[1] - l[3] > 250)
			{
				//line(DstImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 18), 1, CV_AA);
			}
		}
	}

	cout << legelLines.size() << endl;

	Vec4i l = legelLines[1];
	int x1 = l[0];
	int y1 = l[1];
	int x2 = l[2];
	int y2 = l[3];

	float k;
	int b, end_x1, end_x2, end_y1, end_y2;
	if (x2 - x1 != 0)
	{
		k = (y2 - y1) / (x2 - x1);
		b = y1 - k * x1;
		end_y2 = 0;
		end_x2 = -b / k;

		end_y1 = cannyResult.rows;
		end_x1 = (end_y1 - b)/k;
	}
	else
	{
		end_x1 = x1;
		end_x2 = x1;
		end_y1 = cannyResult.cols;
		end_y2 = 0;
	}

	line(toShowColor, Point(end_x1, end_y1), Point(end_x2, end_y2), Scalar(255, 60, 0), 7, CV_AA);

	cout << "H----" << cannyResult.rows << endl;
	cout << "W----" << cannyResult.cols << endl;
	cout << "end_start_point----" <<end_x1<<","<<end_y1 <<endl;
	cout << "end_end_point----" << end_x2 << "," << end_y2 << endl;
	namedWindow("over", WINDOW_KEEPRATIO);
	imshow("over", toShowColor);

	waitKey(0);
	return 0;
}