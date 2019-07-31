#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/structured_light.hpp>
#include <opencv2/opencv_modules.hpp>
#include <Windows.h>
#include <Eigen/dense>
#include <Eigen/Geometry>
#include <opencv2/surface_matching.hpp>
#include <opencv2/surface_matching/ppf_helpers.hpp>


// (if you did not build the opencv_viz module, you will only see the disparity images)
#ifdef HAVE_OPENCV_VIZ
#include <opencv2/viz.hpp>
#endif

using namespace std;
using namespace cv;

static const char* keys =
{ "{@images_list | | Image list where the captured pattern images are saved}"
	"{@calib_param_path     | | Calibration_parameters            }"
	"{@proj_width      | | The projector width used to acquire the pattern          }"
	"{@proj_height     | | The projector height used to acquire the pattern}"
	"{@white_thresh     | | The white threshold height (optional)}"
	"{@black_thresh     | | The black threshold (optional)}" };

static void help()
{
	cout << "\nThis example shows how to use the \"Structured Light module\" to decode a previously acquired gray code pattern, generating a pointcloud"
		"\nCall:\n"
		"./example_structured_light_pointcloud <images_list> <calib_param_path> <proj_width> <proj_height> <white_thresh> <black_thresh>\n"
		<< endl;
}


//Mat plane_from_points(Mat display)
//{
//	vector<Vec3f> c;
//	for (size_t i = 0; i < display.cols; i++)
//	{
//		for (size_t j = 0; j < display.rows; j++)
//		{
//			c.push_back(Vec3f(i, j, display.at<float>(i, j)));
//		}
//	}
//
//	// copy coordinates to  matrix in Eigen format
//	size_t num_atoms = c.size();
//	Eigen::Matrix< Vec3f::, Eigen::Dynamic, Eigen::Dynamic> coord(3, num_atoms);
//	for (size_t i = 0; i < num_atoms; ++i) coord.col(i) = c[i];
//
//	// calculate centroid
//	//Vector3 centroid(coord.row(0).mean(), coord.row(1).mean(), coord.row(2).mean());
//	Point3f centroid = Point3f(coord.row(0).mean(), coord.row(1).mean(), coord.row(2).mean());
//
//	// subtract centroid
//	coord.row(0).array() -= centroid(0); 
//	coord.row(1).array() -= centroid(1); 
//	coord.row(2).array() -= centroid(2);
//
//	// we only need the left-singular matrix here
//
//	auto svd = coord.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);
//	Point3f plane_normal = svd.matrixU().rightCols<1>();
//
//	float x = plane_normal[0];
//	float y = plane_normal[1];
//	float z = plane_normal[2];
//
//
//	float angle = atan2(x, z) * 180 / CV_PI;
//	float angle2 = atan2(y, z) * 180 / CV_PI;
//	
//	Mat plane = Mat(angle, angle2, CV_32F);
//	
//	return plane;
//}

Mat PlaneFitting(Mat disparity)
{
	Vec3f sum;
	float xx, xy, yy, n;
	n = disparity.cols * disparity.rows;
	for (size_t i = 0; i < disparity.cols; i++)
	{
		for (size_t j = 0; j < disparity.rows; j++)
		{

		}
	}

}


int main(int argc, char** argv)
{
	structured_light::GrayCodePattern::Params params;
	CommandLineParser parser(argc, argv, keys);

	params.width = 1024;
	params.height = 768;

	// Set up GraycodePattern with params
	Ptr<structured_light::GrayCodePattern> graycode = structured_light::GrayCodePattern::create(params);
	size_t white_thresh = 0;
	size_t black_thresh = 0;

	if (argc == 2)
	{
		// If passed, setting the white and black threshold, otherwise using default values
		white_thresh = parser.get<unsigned>(4);
		black_thresh = parser.get<unsigned>(5);

		graycode->setWhiteThreshold(white_thresh);
		graycode->setBlackThreshold(black_thresh);
	}
	String imagelist = "E:\\Computer Vision\\19gray\\cam\\cam (";
	string patternlist = "E:\\Computer Vision\\19gray\\pattern\\pattern_im";

	// Loading calibration parameters

	size_t numberOfPatternImages = graycode->getNumberOfPatternImages();
	vector<vector<Mat> > captured_pattern;
	captured_pattern.resize(2);
	captured_pattern[0].resize(numberOfPatternImages);
	captured_pattern[1].resize(numberOfPatternImages);

	Mat color = imread(imagelist + to_string(numberOfPatternImages + 1) + ").bmp", IMREAD_COLOR);
	Size imagesSize = color.size();

	// Stereo rectify

	// Loading pattern images
	for (size_t i = 0; i < numberOfPatternImages; i++)
	{
		captured_pattern[0][i] = imread(imagelist + to_string(i + 1) + ").bmp", IMREAD_GRAYSCALE);
		captured_pattern[1][i] = imread(patternlist + to_string(i + 1) + ".png", IMREAD_GRAYSCALE);

		resize(captured_pattern[0][i], captured_pattern[0][i], Size(1024, 768));
		resize(captured_pattern[1][i], captured_pattern[1][i], Size(1024, 768));

		if ((!captured_pattern[0][i].data) || (!captured_pattern[1][i].data))
		{
			cout << "Empty images" << endl;
			help();
			return -1;
		}
	}
	cout << "done" << endl;

	vector<Mat> blackImages;
	vector<Mat> whiteImages;

	blackImages.resize(2);
	whiteImages.resize(2);

	// Loading images (all white + all black) needed for shadows computation
	cvtColor(color, whiteImages[0], COLOR_RGB2GRAY);

	whiteImages[1] = imread(patternlist + "41.png", IMREAD_GRAYSCALE);
	blackImages[0] = imread(imagelist + "42).bmp", IMREAD_GRAYSCALE);
	blackImages[1] = imread(patternlist + "42.png", IMREAD_GRAYSCALE);

	resize(whiteImages[0], whiteImages[0], Size(1024, 768));
	resize(whiteImages[1], whiteImages[1], Size(1024, 768));
	resize(blackImages[0], blackImages[0], Size(1024, 768));
	resize(blackImages[1], blackImages[1], Size(1024, 768));

	cout << endl << "Decoding pattern ..." << endl;
	Mat disparityMap;
	bool decoded = graycode->decode(captured_pattern, disparityMap, blackImages, whiteImages, structured_light::DECODE_3D_UNDERWORLD);

	if (decoded)
	{
		cout << endl << "pattern decoded" << endl;
		imwrite("E:\\Computer Vision\\19gray\\decode\\disparityMap.png", disparityMap);
		// To better visualize the result, apply a colormap to the computed disparity
		double min;
		double max;
		minMaxIdx(disparityMap, &min, &max);
		Mat cm_disp, scaledDisparityMap, DS, FDS;
		cout << "disp min " << min << endl << "disp max " << max << endl;
		convertScaleAbs(disparityMap, scaledDisparityMap, 255 / (max - min));
		applyColorMap(scaledDisparityMap, cm_disp, COLORMAP_TWILIGHT);
		imwrite("E:\\Computer Vision\\19gray\\decode\\scaledDisparityMap.png", scaledDisparityMap);

		// Show the result
		//resize(cm_disp, cm_disp, Size(640, 480), 0, 0, INTER_LINEAR_EXACT);
		imshow("cm disparity m", cm_disp);
		imwrite("E:\\Computer Vision\\19gray\\decode\\cm_disp.png", cm_disp);

		// Compute the point cloud
		//disparityMap.convertTo(disparityMap, CV_32FC1);
		imwrite("E:\\Computer Vision\\19gray\\decode\\convertToCV_32FC1disp.png", disparityMap);
		// Compute a mask to remove background
		Mat dst, thresholded_disp;
		threshold(scaledDisparityMap, thresholded_disp, 0, 255, THRESH_OTSU + THRESH_BINARY);
		//resize(thresholded_disp, dst, Size(640, 480), 0, 0, INTER_LINEAR_EXACT);
		imshow("threshold disp otsu", thresholded_disp);
		imwrite("E:\\Computer Vision\\19gray\\decode\\thresholded_disp.png", thresholded_disp);



		///
		Beep(1568, 200);
		Beep(1568, 200);
		Beep(1568, 200);
		Beep(1245, 1000);
		Beep(1397, 200);
		Beep(1397, 200);
		Beep(1397, 200);
		Beep(1175, 1000);
		///

		//disparityMap.p

#if 0//def HAVE_OPENCV_VIZ
		// Apply the mask to the point cloud
		Mat pointcloud_tresh, color_tresh;
		color.copyTo(color_tresh, thresholded_disp);

		// Show the point cloud on viz
		viz::Viz3d myWindow("Point cloud with color");
		myWindow.setBackgroundMeshLab();
		myWindow.showWidget("coosys", viz::WCoordinateSystem());
		myWindow.showWidget("pointcloud", viz::WCloud(pointcloud_tresh, color_tresh));
		myWindow.showWidget("text2d", viz::WText("Point cloud", Point(20, 20), 20, viz::Color::green()));
		myWindow.spin();
#endif // HAVE_OPENCV_VIZ

	}

	waitKey();
	return 0;
}