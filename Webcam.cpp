#include <opencv2/core.hpp>
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
	Mat ascii = Mat(frame.rows, frame.cols, frame.type());
	glyph_r_step = Glyphs[0].rows;
	glyph_c_step = Glyphs[0].cols;
	int min_index = 0;
	double min_distance = INFINITY;
	double curr_distance = 0;
	// loop through all the windows
	for(int r=0; r<frame.rows + glyph_r_step; r+= glyph_r_step) {
		for(int c=0; c<frame.cols + glyph_c_step, c+= glyph_c_step) {
			min_index = 0;
			min_distance = INFINITY;
			for	(int i=0; i<glyph_len; i++) {
				for(int gr=0; gr<glyph_r_step; gr++) {
					for(int gc=0; gc<glyph_c_step; gc++) {
						curr_distance += (frame.at(r+gr, c+gc) - Glyphs[i].at(gr, gc))*(frame.at(r+gr, c+gc) - Glyphs[i].at(gr, gc));
					}
				}
				curr_distance = sqrt(curr_distance);
				if (curr_distance < min_distance) {
					min_distance = curr_distance;
					min_index = i;
				}
			}
			for(int gr=0; gr<glyph_r_step; gr++) {
				for(int gc=0; gc<glyph_c_step; gc++) {
					ascii.at(r+gr, c+gc) = Glyphs[min_index].at(gr, gc);
				}
			}

		}
	}

	return ascii;
}


int main(int, char**)
{
    Mat frame;
    Mat ascii;
    VideoCapture cap;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    
	// Preprocess all the glyphs
	char glyph_path[100];
	
	// Determine how many PNGs are in the directory and the size of each PNG
	// Allocate memory for that many matrices
	Mat* glyphs = malloc(sizeof(Mat)*num_glyphs);
	for(int i=0; i<num_glyphs; i++) {
		sprintf(glyph_path, "./glyphs/%d.png", i);
		glyphs[i] = imread(glyph_path, IMREAD_GRAYSCALE);
	}

	
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
		ascii = ASCII(frame, glyphs, num_glyphs);
        imshow("Live in ASCII", ascii);
        // 17ms is roughly 60 fps
		if (waitKey(17) >= 0)
            break;
    }
    return 0;
}
