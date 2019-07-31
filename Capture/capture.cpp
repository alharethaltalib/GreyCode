#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/structured_light.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

static const char* keys =
{ "{@path | | Path of the folder where the captured pattern images will be save }"
	 "{@proj_width      | | Projector width            }"
	 "{@proj_height     | | Projector height           }" };

static void help()
{
	cout << "\nThis example shows how to use the \"Structured Light module\" to acquire a graycode pattern"
		"\nCall (with the two cams connected):\n"
		"./example_structured_light_cap_pattern <path> <proj_width> <proj_height> \n"
		<< endl;
}

int main(int argc, char** argv)
{

	structured_light::GrayCodePattern::Params params;

	CommandLineParser parser(argc, argv, keys);
	String path = "E:\\Computer Vision\\19gray\\cap\\";
	params.width = 1024;
	params.height = 768;

	if (path.empty() || params.width < 1 || params.height < 1)
	{
		help();
		return -1;
	}

	// Set up GraycodePattern with params
	Ptr<structured_light::GrayCodePattern> graycode = structured_light::GrayCodePattern::create(params);

	// Storage for pattern
	vector<Mat> pattern;
	graycode->generate(pattern);

	cout << pattern.size() << " pattern images + 2 images for shadows mask computation to acquire with both cameras"
		<< endl;

	// Generate the all-white and all-black images needed for shadows mask computation
	Mat white;
	Mat black;
	graycode->getImagesForShadowMasks(black, white);

	pattern.push_back(white);
	pattern.push_back(black);

	// Setting pattern window on second monitor (the projector's one)
	namedWindow("Pattern Window", WINDOW_NORMAL);
	resizeWindow("Pattern Window", params.width, params.height);
	moveWindow("Pattern Window", params.width + 1920, -20);
	setWindowProperty("Pattern Window", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

	//waitKey(500);

	// Open camera number 1, using libgphoto2
	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		// check if cam1 opened
		cout << "cam1 not opened!" << endl;
		help();
		//return -1;
	}

	// Turning off autofocus
	cap.set(CAP_PROP_SETTINGS, 1);

	int i = 0;
	while (i < (int)pattern.size())
	{
		cout << "Waiting to save image number " << i + 1 << endl << "Press any key to acquire the photo" << endl;

		imshow("Pattern Window", pattern[i]);

		Mat frame1;

		cap >> frame1;  // get a new frame from camera 1

		if (frame1.data)
		{

			Mat tmp;
			cout << "cam 1 size: " << Size((int)cap.get(CAP_PROP_FRAME_WIDTH), (int)cap.get(CAP_PROP_FRAME_HEIGHT)) << endl;

			cout << "zoom cam 1: " << cap.get(CAP_PROP_ZOOM) << endl;

			cout << "focus cam 1: " << cap.get(CAP_PROP_FOCUS) << endl;

			cout << "Press enter to save the photo or an other key to re-acquire the photo" << endl;

			namedWindow("cam1", WINDOW_NORMAL);
			resizeWindow("cam1", 640, 480);

			// Moving window of cam2 to see the image at the same time with cam1

			// Resizing images to avoid issues for high resolution images, visualizing them as grayscale
			resize(frame1, tmp, Size(640, 480), 0, 0, INTER_LINEAR_EXACT);
			cvtColor(tmp, tmp, COLOR_RGB2GRAY);
			imshow("cam1", tmp);

			bool save1 = false;
			bool save2 = false;

			int key = waitKey(0);

			// Pressing enter, it saves the output
			if (key == 13)
			{
				ostringstream name;
				name << i + 1;

				save1 = imwrite(path + "pattern_cam1_im" + name.str() + ".png", frame1);
				save2 = imwrite(path + "pattern_im" + name.str() + ".png", pattern[i]);

				if ((save1) && (save2))
				{
					cout << "pattern cam1 and cam2 images number " << i + 1 << " saved" << endl << endl;
					i++;
				}
				else
				{
					cout << "pattern cam1 and cam2 images number " << i + 1 << " NOT saved" << endl << endl << "Retry, check the path" << endl << endl;
				}
			}
			// Pressing escape, the program closes
			else if (key == 27)
			{
				cout << "Closing program" << endl;
			}
			else if (key == 114 || key == 82)  //r	R
			{
				cout << "-------------------------\n-------------------------\n" << endl;
			}
		}
		else
		{
			cout << "No frame data, waiting for new frame" << endl;
		}
	}

	// the camera will be deinitialized automatically in VideoCapture destructor

	return 0;
}