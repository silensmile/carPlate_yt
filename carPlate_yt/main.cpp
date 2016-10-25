#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>

using namespace cv;
using namespace std;

int main()
{
	Mat pic = imread("pic.jpg");
	Mat src = imread("pic.jpg");
	Mat dst;
	Mat pos;
	Mat image = pic;
	Mat newimage;
	namedWindow("origin");
	imshow("origin", pic);//显示原始图片
	//使用漫水填充算法得到车牌区域//
	Rect ccomp;
	floodFill(src, Point(0, 0), Scalar(0, 90, 170), &ccomp, Scalar(20, 20, 20), Scalar(20, 20, 20));

	//变为灰度图
	cvtColor(src, src, CV_RGB2GRAY);
	//imshow("效果图1", src);

	//进行形态学腐蚀膨胀操作//
	Mat core = getStructuringElement(MORPH_RECT, Size(15, 15));//自定义运算核
	erode(src, src, core);
	dilate(src, dst, core);
	//imshow("效果图2", dst);

	//二值化处理//
	//adaptiveThreshold(dst,dst,100,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,5,5);
	threshold(dst, dst, 73, 255, THRESH_BINARY);
	//imshow("效果图3", dst);

	//对感兴趣区域进行分割提取//
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;//定义轮廓和层次结构

	findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));//查找轮廓
	vector<Point> contours_poly(contours.size());
	Rect dstRect;//定义矩形边框
	approxPolyDP(Mat(contours[0]), contours_poly, 3, true);
	dstRect = boundingRect(Mat(contours_poly));
	//rectangle(pic, dstRect, Scalar(CV_RGB(255, 0, 0)), 2, 8, 0);
	//imshow("车牌区域", pic);

	pic(Rect(dstRect)).copyTo(newimage);//将车牌区域提取出来为新图片
	imshow("提取出的车牌", newimage);
	
	//对车牌进行的处理
	cvtColor(newimage, dst, CV_RGB2GRAY);
	//imshow("A", dst);

	Rect dstRect1(19, 5, dstRect.width - 20, dstRect.height - 11);
	dst(Rect(dstRect1)).copyTo(pos);
	imshow("效果图3", pos);
	//车牌的腐蚀膨胀和二值化处理
	Mat core1 = getStructuringElement(MORPH_RECT, Size(1, 50));//自定义运算核
	Mat core2 = getStructuringElement(MORPH_RECT, Size(5, 1));//自定义运算核
	//Mat core2 = getStructuringElement(MORPH_RECT, Size(1, 5));//自定义运算核
	threshold(pos, dst, 131, 255, THRESH_BINARY);

	dilate(dst, dst, core1);

	erode(dst, dst, core2);
	////imshow("B", dst);
	//erode(dst, dst, core2);
	//dilate(dst, dst, core2);



	vector<vector<Point>> contours1;
	vector<Vec4i> hierarchy1;//定义轮廓和层次结构
	vector<Point> contours_poly1(contours1.size());
	vector<Rect> dstRect2;
	vector<Mat> single;
	char str[5] = "pic0";
	findContours(dst, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));//查找轮廓
	//将车牌的每一个号码所在的矩形区域分出来
	for (int i = 0; i < contours1.size(); i++)
	{
		approxPolyDP(Mat(contours1[i]), contours_poly1, 3, true);
		dstRect2.push_back(boundingRect(Mat(contours_poly1)));
	}

	std::sort(dstRect2.begin(), dstRect2.end(),
		[](Rect rec1, Rect rec2) {return rec1.x < rec2.x; }
	);
	for (int j = 0; j < contours1.size(); j++)
	{
	single.push_back(pos(Rect(dstRect2[j])));
	str[3] = (char)(j + '0');
	imshow(str, single[j]);
	}
	
	/*与模板的匹配部分（以其中一个数字为例进行比较）*/
	Mat mod = imread("module.jpg",0);
	Mat board(Size(mod.cols * 3, mod.rows * 3), mod.type(), Scalar(CV_RGB(0, 0, 0)));
	addWeighted(mod, 1, board(Rect(mod.cols, mod.rows, mod.cols, mod.rows)), 0, 0,
		board(Rect(mod.cols, mod.rows, mod.cols, mod.rows)), -1);
	
	vector<double> grade(single.size());
	for (int j = 1; j < single.size(); j++)
	{
		Mat result;
		matchTemplate(single[j], board, result, CV_TM_CCOEFF_NORMED);
		double max, min;
		minMaxLoc(result, &min, &max);
		grade[j] = max;
	}//将模板图像与每个数字的矩形区域匹配进行匹配度计算

	imshow("result", single[5]);


	//绘制轮廓//
	/*RotatedRect box = minAreaRect(Mat(dst));
	Point2f vertex[4];
	box.points(vertex);

	for (int i = 0; i < 4; i++)
	{
		line(dst,vertex[i],vertex[(i+1)%4],Scalar(255,0,0),2,LINE_AA);
	}*/
	//int i = 0;
	//for (; i >= 0; i = hierarchy[i][0])
	//{
	//	Scalar color(0,0,255);//用红线绘制轮廓
	//	drawContours(image,contours,i,color,FILLED,LINE_AA,hierarchy);
	//}
	//imshow("轮廓",image);


	waitKey(0);
	return 0;
}