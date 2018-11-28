#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*
* ����ÿ������
*/
void eachPx(Mat srcImg)
{
	int rows = srcImg.rows;
	int cols = srcImg.cols*srcImg.channels();

	int min = 0;// ÿһ������������ʱ��ֵ
	int goalCols = 0;// ���������ٵ��� (����ϵX����)
	int goalRows = 0;// ���������ٵ��� (����ϵY����)
	for (int j = 0; j < cols; j++)
	{
		vector<int> goalPx;
		int firstPx = 0;
		for (int i = 0; i<rows; i++)
		{
			int data = srcImg.ptr<uchar>(i, j)[0];
			if (data == 255)
			{
				goalPx.push_back(i);
			}
		}

		if (goalPx.size() > 1)
		{
			int cur = goalPx.back() - goalPx.front();
			if (min == 0) { min = cur; }
			if (cur < min)
			{
				min = cur;
				goalCols = j;
				goalRows = goalPx.front() + cur / 2;
			}
		}
		
	}

	Mat toColor;
	cvtColor(srcImg, toColor, CV_GRAY2BGR);
	circle(toColor, Point(goalCols, goalRows), 20, Scalar(255, 0, 0), -1);

	namedWindow("ok", WINDOW_KEEPRATIO);
	imshow("ok", toColor);
}

/*
* ��ֵ��
*/
void twoValueFun(Mat src)
{
	Mat  result, grayImg;
	cvtColor(src, grayImg, COLOR_RGB2GRAY);
	threshold(grayImg, result, 127, 255, THRESH_BINARY);
	Mat frame_ok(result, Rect(800, 1500, 1400, 2300));
	eachPx(frame_ok);

	//namedWindow("twoShow", WINDOW_KEEPRATIO);
	//imshow("twoShow", frame_ok);

	//Mat cannyResult;
	//Canny(frame_ok, cannyResult, 10, 200, 3);

	// ������
	/*Mat element7(7, 7, CV_8U, cv::Scalar(1));
	morphologyEx(frame_ok, frame_ok, cv::MORPH_OPEN, element7);
	morphologyEx(frame_ok, frame_ok, cv::MORPH_CLOSE, element7);
	imshow("binary image", frame_ok);
	vector<std::vector<cv::Point>> contours;
	findContours(frame_ok, contours, CV_RETR_EXTERNAL,//�����ⲿ����
	CV_CHAIN_APPROX_NONE);//ÿ��������ȫ������
	Mat result1(frame_ok.size(), CV_8U, cv::Scalar(255));
	drawContours(result1, contours,
	-1,//��ȫ������
	0,//�ú�ɫ��
	2);//���Ϊ2
	namedWindow("contours", WINDOW_KEEPRATIO);
	cv::imshow("contours", result1);*/
	// ��������
}

/*
* ʵʱ����ͼ��canny
*/
int getVideoFromCamera()
{
	VideoCapture cap;
	cap.open(0);
	//cap.set(CV_CAP_PROP_FRAME_WIDTH, 2560);  //���ò�����Ƶ�Ŀ��
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);  //���ò�����Ƶ�ĸ߶�
	if (!cap.isOpened())                        
	{
		return -1;
	}

	Mat frame, frame_L, cannyResult;
	cap >> frame;                                //���������һ֡ͼ��
	Mat grayImage;                               //���ڴ�ŻҶ�����
	
	while (1)
	{
		cap >> frame;                             //���������һ֡ͼ��
		//frame_L = frame(Rect(0, 0, 640, 480));       
		////namedWindow("Source Video", WINDOW_KEEPRATIO);
		//imshow("Source Video", frame_L);

		Mat  result, grayImg;
		cvtColor(frame, grayImg, COLOR_RGB2GRAY);
		threshold(grayImg, result, 127, 255, THRESH_BINARY);

		Mat frame_ok(result, Rect(0, 0, 640, 480));
		eachPx(frame_ok);

		//namedWindow("twoShow", WINDOW_KEEPRATIO);
		//imshow("twoShow", frame_ok);
		//cvtColor(frame_L, grayImage, CV_BGR2GRAY); //OPENCV����ɫͼ��תΪ�Ҷ�����
		//Canny(frame_L, cannyResult, 10, 100, 3); 
		//namedWindow("Canny Video", WINDOW_KEEPRATIO);          
		//imshow("Canny Video", cannyResult);

		//cout << cannyResult.cols << "," << cannyResult.rows << endl;

		if (waitKey(30) >= 0) break;
	}

	cap.release();                               
}

/*
 * ͼ��ǿ��
 */
void toStoreFun(Mat src)
{
	Mat imageRGB[3];
	split(src, imageRGB);
	for (int i = 0; i < 3; i++)
	{
		equalizeHist(imageRGB[i], imageRGB[i]);
	}
	merge(imageRGB, 3, src);

	namedWindow("twoShow", WINDOW_KEEPRATIO);
	imshow("twoShow", src);
}



int main()
{
	system("mode con:cols=100 lines=3000");
	Mat srcImg = imread("true1.jpg");
	
	//twoValueFun(srcImg);
	getVideoFromCamera();
	waitKey(0);
	return 0;
}

void mutiImg()
{
	Mat srcImg = imread("circle1.jpg");

	Mat blurImg;
	cvtColor(srcImg, blurImg, COLOR_RGB2GRAY);
	blur(blurImg, blurImg, Size(5, 5));
	namedWindow("blur", WINDOW_KEEPRATIO);
	imshow("blur", blurImg);

	Mat cannyResult;
	Canny(srcImg, cannyResult, 80, 100, 3);
	namedWindow("blur-canny", WINDOW_KEEPRATIO);
	imshow("blur-canny", cannyResult);

	Mat toShowColor;
	cvtColor(cannyResult, toShowColor, CV_GRAY2BGR);

	vector<Vec4i>g_lines;
	HoughLinesP(cannyResult, g_lines, 1, CV_PI / 255, 100, 200, 200);

	vector<Vec3f> circles;
	HoughCircles(cannyResult, circles, CV_HOUGH_GRADIENT, 1, cannyResult.rows / 5, 150, 70, 0, 0);
	cout << circles.size() << "..........circles" << endl;
	cout << g_lines.size() << "..........lines" << endl;

	/*
	for (size_t i = 0; i < g_lines.size(); i++)
	{
	Vec4i l = g_lines[i];
	line(toShowColor, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 60, 0), 6, CV_AA);
	}

	vector<Vec3f> circles;
	HoughCircles(cannyResult, circles, CV_HOUGH_GRADIENT, 1, blurImg.rows / 20, 100, 100, 0, 0);
	//������ͼ�л��Ƴ�Բ
	for (size_t i = 0; i < circles.size(); i++)
	{
	Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	int radius = cvRound(circles[i][2]);
	//����Բ��
	circle(DstImg, center, 3, Scalar(0, 255, 0), -1, 8, 0);
	//����Բ����
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

			if (k < 0)// �۽ǣ� //&& l[1] - l[3] > 200
			{
				//���������ľ�
				//double dstLength = arcLength(g_vsrcPoints, true);
				legelLines.push_back(l);
				//line(DstImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(240, 26, 46), 1, CV_AA);
			}
		}
		else // ��ֱ����
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
		end_x1 = (end_y1 - b) / k;
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
	cout << "end_start_point----" << end_x1 << "," << end_y1 << endl;
	cout << "end_end_point----" << end_x2 << "," << end_y2 << endl;
	namedWindow("over", WINDOW_KEEPRATIO);
	imshow("over", toShowColor);

	waitKey(0);
}

/*
*  �˲�
*/
void blur()
{
	//cv::cvtColor(srcImg, srcImg, COLOR_RGB2GRAY);//���У��Ҷȴ���

	//Mat dstImg1, dstImg2, dstImg3, dstImg4, dstImg5;
	//Size size0 = Size(5, 5);     
	//boxFilter(srcImg, dstImg1, -1, Size(3, 3), Point(-1, -1), true);  
	//�����˲��� bool normalize��������Ϊtrueʱ��ͬ�ھ�ֵ�˲�   
	//blur(srcImg, dstImg2, size0);  //��ֵ�˲�    
	//GaussianBlur(srcImg, dstImg3, Size(3, 3), 1);//��˹�˲�    
	//medianBlur(srcImg, dstImg4, 5);//��ֵ�˲�    
	//bilateralFilter(srcImg, dstImg5, 5, 10.0, 2.0);//˫���˲�    
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
}