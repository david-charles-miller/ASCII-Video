#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <cmath>
using namespace cv;
using namespace std;

/*int SSD_ASCII(Mat sub_frame, Mat* Glyphs) {
	int min_index = 0;
	double min_distance = 0;
	
	// Do sum of squared differences between current window and each glyph

	return min_index;
}
*/

Mat ASCII(Mat frame, Mat* Glyphs, int glyph_len) {
	Scalar s = 255;
	//Mat* Glyphs = *G;
	Mat ascii = Mat(frame.rows, frame.cols, frame.type(), &s);
	int glyph_r_step = Glyphs[0].rows;
	int glyph_c_step = Glyphs[0].cols;
	int min_index = 0;
	double min_distance = INFINITY;
	double curr_distance = 0;
	/*
	for	(int i=0; i<glyph_len; i++) {
		printf("%d\n", i);
		cout << Glyphs[i] << endl;
	}*/

	//printf("Creating ASCII frame...\n");
	// loop through all the windows
	for(int r=0; r<frame.rows + glyph_r_step; r+= glyph_r_step) {
		for(int c=0; c<frame.cols + glyph_c_step; c+= glyph_c_step) {
			min_index = 0;
			min_distance = INFINITY;
			for	(int i=0; i<glyph_len; i++) {
				curr_distance = 0;
				printf("%d\n", i);
				for(int gr=0; gr<glyph_r_step; gr++) {
					for(int gc=0; gc<glyph_c_step; gc++) {
						//printf("distance \n");
						//if (i>520) {
						//	printf("%d %d %d %d\n", r, c, gr, gc);
						//}
						//curr_distance += (frame.at<int>(r+gr, c+gc) - Glyphs[i].at<int>(gr, gc))*(frame.at<int>(r+gr, c+gc) - Glyphs[i].at<int>(gr, gc));
						curr_distance += log(fabs(frame.at<int>(r+gr, c+gc) - Glyphs[i].at<int>(gr, gc)));
					}
				}
				//printf("curr_distance = %f\n", curr_distance);
				//curr_distance = sqrt(curr_distance);
				if (curr_distance < min_distance) {
					min_distance = curr_distance;
					min_index = i;
				}
			}
			printf("min index = %d\n", min_index);
			printf("min distance = %f\n", min_distance);
			for(int gr=0; gr<glyph_r_step; gr++) {
				for(int gc=0; gc<glyph_c_step; gc++) {
					//printf("%d %d %d %d\n", r, c, gr, gc);
					//printf("%d\n", Glyphs[min_index].at<int>(gr, gc));
					ascii.at<int>(r+gr, c+gc) = Glyphs[min_index].at<int>(gr, gc);
				}
			}
			//printf("%d %d %d %d\n", r, c, glyph_r_step, glyph_c_step);
			//printf("Finished window (%d, %d)...\n", int(r/glyph_r_step), int(c/glyph_c_step));

		}
	}

	return ascii;
}


int main(int argc, char** argv)
{
    Mat frame;
    Mat bwframe;
	Mat ascii;
    VideoCapture cap;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    
	// Preprocess all the glyphs
	int num_glyphs = 525;
	char glyph_path[100];

	printf("Allocating glyph array\n");	
	// Determine how many PNGs are in the directory and the size of each PNG
	// Allocate memory for that many matrices
	Mat* glyphs = new Mat[num_glyphs];
	for(int i=1; i<=num_glyphs; i++) {
		sprintf(glyph_path, "./glyphs/%d.png", i);
		//printf("%s\n", glyph_path);
		glyphs[i-1] = 255 - imread(glyph_path, IMREAD_GRAYSCALE);
		//Mat temp = imread(glyph_path, IMREAD_COLOR);
		//Mat temp2 = Mat(temp.rows, temp.cols, temp.type(), (255, 255, 255));

		//imshow("Glyph", glyphs[i-1]);
		//if (waitKey(1000) >= 0)
        //    break;
		//glyphs[i-1] = temp;
		//printf("Glyph data\n");
		cout << glyphs[i-1] << endl;
	}

	printf("Turning on camera\n");	
	
    cap.open(deviceID, apiID);
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    cout << "Press any key to quit" << endl;
    while (true)
    {
        cap.read(frame);
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
		cvtColor(frame, bwframe, CV_RGB2GRAY);
		ascii = ASCII(frame, glyphs, num_glyphs);
        imshow("Live in ASCII", ascii);
        // 17ms is roughly 60 fps
		if (waitKey(34) >= 0)
            break;
    }
    return 0;
}
