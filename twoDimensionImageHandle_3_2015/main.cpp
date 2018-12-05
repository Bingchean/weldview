#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define random(x) (rand()%x)
const int PER_SIZE_JUDGE = 100;
const int POINT_CIRCLE_FONT = 10;
const int LINE_FONT = 3;

/*bool goal_point_legal(Mat src_img) 
{
	bool result = true;
	for (int j = 0; j < src_img.cols; j++)
	{
		bool cur_col_resulf = false;
		for (int i = 0;i < src_img.rows;i++) 
		{
			int data = src_img.ptr<uchar>(i, j)[0];
			if (data == 255)
			{	// 当前列只要有白色当前列就是合法的
				cur_col_resulf = true;
				break;
			}
		}
		//只要有一列不合法整体就不合法
		if (!cur_col_resulf)
		{
			result = false;
			break;
		}
	}
	return result;
}*/

void each_px_optimize(Mat src_img)
{
	int rows = src_img.rows;
	int cols = src_img.cols*src_img.channels();

	//cout << rows << "," << src_img.cols << "," << src_img.channels() << endl;
	vector<vector<vector<int>>> goal_clos;							//1：选定列；2：连续区域；3：行数
	vector<int> sel_cols_list;										// 选中列集合的序号
	for (int sel_cols = 0; sel_cols < cols ; sel_cols += PER_SIZE_JUDGE)
	{
		sel_cols_list.push_back(sel_cols);
		vector<vector<int>> goal_zone;								// 1：连续区域；2：行数
		for (int i = 0; i < rows; i++)
		{
			int data = src_img.ptr<uchar>(i, sel_cols)[0];
			if (data == 255)
			{
				int last_px;										//每列连续区域集合最后一个集合的最后一行.(为空赋为1)
				if (!goal_zone.empty())
				{
					vector<int> a = goal_zone.back();
					last_px = a.back();
				}
				else
					last_px = 1;

				if (last_px == 1)									// 初始化集合
				{
					vector<int> rows_list;
					rows_list.push_back(i);
					goal_zone.push_back(rows_list);
				}
				else												// 有值判断是否连续
				{
					if (i != last_px + 1)
					{
						vector<int> rows_list;
						rows_list.push_back(i);
						goal_zone.push_back(rows_list);
					}
					else
					{
						vector<int> rows_list = goal_zone.back();
						goal_zone.pop_back();
						rows_list.push_back(i);
						goal_zone.push_back(rows_list);
					}
				}
			}
		}
		goal_clos.push_back(goal_zone);
	}

	Mat toColor;
	cvtColor(src_img, toColor, CV_GRAY2BGR);
	
	int point_index = sel_cols_list.size() / 2 / 3;
	int left_index_1 = point_index;
	int left_index_2 = point_index * 2;
	int right_index_1 = point_index * 4;
	int right_index_2 = point_index * 5;

	int up_left_point[4];										 // 断点左边，上面两个点的坐标 x1y1x2y2
	int up_right_point[4];										 // 断点右边，上面两个点的坐标 x3y3x4y4

	// 选定的四列中没有黑色区域块
	if (!(goal_clos[left_index_1].empty() ||
		goal_clos[left_index_2].empty() ||
		goal_clos[right_index_1].empty() ||
		goal_clos[right_index_2].empty()))
	{
		up_left_point[0] = sel_cols_list[left_index_1];				 // x1
		up_left_point[1] = goal_clos[left_index_1].front().front();  // y1
		up_left_point[2] = sel_cols_list[left_index_2];  			 // x2
		up_left_point[3] = goal_clos[left_index_2].front().front();	 // y2
		int dif_left_b = goal_clos[left_index_1].front().back() - goal_clos[left_index_1].front().front();

		up_right_point[0] = sel_cols_list[right_index_1];			 // x3
		up_right_point[1] = goal_clos[right_index_1].front().front();// y3
		up_right_point[2] = sel_cols_list[right_index_2];  			 // x4
		up_right_point[3] = goal_clos[right_index_2].front().front();// y4
		int dif_right_b = goal_clos[right_index_1].front().back() - goal_clos[right_index_1].front().front();

		circle(toColor, Point(up_left_point[0], up_left_point[1]), POINT_CIRCLE_FONT, Scalar(255, 0, 0), -1);
		circle(toColor, Point(up_left_point[2], up_left_point[3]), POINT_CIRCLE_FONT, Scalar(255, 0, 0), -1);
		circle(toColor, Point(up_left_point[0], up_left_point[1] + dif_left_b), POINT_CIRCLE_FONT, Scalar(255, 0, 0), -1);
		circle(toColor, Point(up_left_point[2], up_left_point[3] + dif_left_b), POINT_CIRCLE_FONT, Scalar(255, 0, 0), -1);

		circle(toColor, Point(up_right_point[0], up_right_point[1]), POINT_CIRCLE_FONT, Scalar(255, 0, 0), -1);
		circle(toColor, Point(up_right_point[2], up_right_point[3]), POINT_CIRCLE_FONT, Scalar(255, 0, 0), -1);
		circle(toColor, Point(up_right_point[0], up_right_point[1] + dif_right_b), POINT_CIRCLE_FONT, Scalar(255, 0, 0), -1);
		circle(toColor, Point(up_right_point[2], up_right_point[3] + dif_right_b), POINT_CIRCLE_FONT, Scalar(255, 0, 0), -1);


		float k_left, k_right;					// 斜率左边上下一致，右边上线一致
		int b_up_left, b_up_right;

		k_left = (up_left_point[3] - up_left_point[1]) / (up_left_point[2] - up_left_point[0]);
		k_right = (up_right_point[3] - up_right_point[1]) / (up_right_point[2] - up_right_point[0]);
		b_up_left = up_left_point[1] - k_left * up_left_point[0];
		b_up_right = up_right_point[1] - k_right * up_right_point[0];

		line(toColor,
			Point(up_left_point[0], up_left_point[1]),			// 左上
			Point(up_left_point[2], up_left_point[3]),
			Scalar(240, 26, 46), LINE_FONT, CV_AA);
		line(toColor,					    // 左下
			Point(up_left_point[0], up_left_point[1] + dif_left_b),
			Point(up_left_point[2], up_left_point[3] + dif_left_b),
			Scalar(240, 26, 46), LINE_FONT, CV_AA);

		line(toColor,						// 右上
			Point(up_right_point[0], up_right_point[1]),
			Point(up_right_point[2], up_right_point[3]),
			Scalar(240, 26, 46), LINE_FONT, CV_AA);
		line(toColor,                       // 右下
			Point(up_right_point[0], up_right_point[1] + dif_right_b),
			Point(up_right_point[2], up_right_point[3] + dif_right_b),
			Scalar(240, 26, 46), LINE_FONT, CV_AA);
	}
	
	namedWindow("ok", WINDOW_KEEPRATIO);
	imshow("ok", toColor);
}

/*
* 遍历每个像素
*/
void each_px(Mat srcImg)
{
	int rows = srcImg.rows;
	int cols = srcImg.cols*srcImg.channels();

	int min = 0;// 每一列特征点最少时的值
	int min_goal_cols = 0;// 特征点最少的列 (坐标系X坐标)
	int min_goal_rows = 0;// 特征点最少的行 (坐标系Y坐标)
	for (int j = 0; j < cols; j++)
	{
		vector<int> min_goal_px;  // 目标图像(255)所在行
		vector<int> empty_goal_px;// 空像素列集合
		int firstPx = 0;
		for (int i = 0; i<rows; i++)
		{
			int data = srcImg.ptr<uchar>(i, j)[0];
			if (data == 255)
			{
				min_goal_px.push_back(i);
			}
		}

		if (min_goal_px.size() > 0)
		{
			int cur = min_goal_px.back() - min_goal_px.front();
			if (min == 0) { min = cur; }
			if (cur < min)
			{
				min = cur;
				min_goal_cols = j;
				min_goal_rows = min_goal_px.front() + cur / 2;
			}
		}
		else
		{
			empty_goal_px.push_back(j);
		}
	}

	Mat toColor;
	cvtColor(srcImg, toColor, CV_GRAY2BGR);
	circle(toColor, Point(min_goal_cols, min_goal_rows), 20, Scalar(255, 0, 0), -1);

	namedWindow("ok", WINDOW_KEEPRATIO);
	imshow("ok", toColor);
}

/*
* 二值化
*/
void twoValueFun(Mat src)
{
	Mat  result, grayImg;
	cvtColor(src, grayImg, COLOR_RGB2GRAY);
	threshold(grayImg, result, 127, 255, THRESH_BINARY);
	Mat frame_ok(result, Rect(800, 1500, 1400, 2300));//6s photo test size.

	each_px_optimize(frame_ok);

	//namedWindow("twoShow", WINDOW_KEEPRATIO);
	//imshow("twoShow", frame_ok);

	//Mat cannyResult;
	//Canny(frame_ok, cannyResult, 10, 200, 3);

	// 画轮廓
	/*Mat element7(7, 7, CV_8U, cv::Scalar(1));
	morphologyEx(frame_ok, frame_ok, cv::MORPH_OPEN, element7);
	morphologyEx(frame_ok, frame_ok, cv::MORPH_CLOSE, element7);
	imshow("binary image", frame_ok);
	vector<std::vector<cv::Point>> contours;
	findContours(frame_ok, contours, CV_RETR_EXTERNAL,//检索外部轮廓
	CV_CHAIN_APPROX_NONE);//每个轮廓的全部像素
	Mat result1(frame_ok.size(), CV_8U, cv::Scalar(255));
	drawContours(result1, contours,
	-1,//画全部轮廓
	0,//用黑色画
	2);//宽度为2
	namedWindow("contours", WINDOW_KEEPRATIO);
	cv::imshow("contours", result1);*/
	// 轮廓结束
}

/*
* 实时分析图像
*/
int getVideoFromCamera()
{
	VideoCapture cap;
	cap.open(0);
	//cap.set(CV_CAP_PROP_FRAME_WIDTH, 2560);  //设置捕获视频的宽度
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);  //设置捕获视频的高度
	if (!cap.isOpened())                        
	{
		return -1;
	}

	Mat frame, frame_L, cannyResult;
	cap >> frame;                                //从相机捕获一帧图像
	Mat grayImage;                               //用于存放灰度数据
	
	while (1)
	{
		cap >> frame;                             //从相机捕获一帧图像

		Mat  result, grayImg;
		cvtColor(frame, grayImg, COLOR_RGB2GRAY);
		threshold(grayImg, result, 127, 255, THRESH_BINARY);

		Mat frame_ok(result, Rect(0, 0, 640, 480));
		each_px_optimize(frame_ok);

		if (waitKey(30) >= 0) break;
	}

	cap.release();              
}

/*
 * 图形强化
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

			if (k < 0)// 钝角？ //&& l[1] - l[3] > 200
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
*  滤波
*/
void blur()
{
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
}