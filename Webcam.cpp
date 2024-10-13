#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <assert.h>
#include <limits>
using namespace cv;
using namespace std;

#define WIDTH 640
#define HEIGHT 480
#define CAMERA_FPS 30

Mat ASCII(const Mat &frame, Mat* Glyphs, int glyph_len) {
	Mat ascii(frame.rows, frame.cols, frame.type(), 255);
	int glyph_rows = Glyphs[0].rows;
	int glyph_cols = Glyphs[0].cols;
	int frame_rows = frame.rows;
	int frame_cols = frame.cols;
	int min_index = 0;
	int min_distance = INT_MAX;
	int curr_distance = 0;

	uchar* frame_raw = frame.data;
	uchar* ascii_raw = ascii.data;
	uchar* glyph_raw;

	//printf("Creating ASCII frame...\n");
	// loop through all the windows
	for(int r=0; r<frame_rows - glyph_rows; r+= glyph_rows) {
		for(int c=0; c<frame_cols - glyph_cols; c+= glyph_cols) {
			min_index = 0;
			min_distance = INT_MAX;
			for	(int i=0; i<glyph_len; i++) {
				glyph_raw = Glyphs[i].data;
				curr_distance = 0;
				for(int gr=0; gr<glyph_rows; gr++) {
					for(int gc=0; gc<glyph_cols; gc++) {
						uchar pixel_1 = frame_raw[(r+gr)*frame_cols + (c+gc)];
						uchar pixel_2 = glyph_raw[(gr)*glyph_cols + gc];
						//uchar pixel_1 = frame.at<uchar>(r+gr, c+gc); 
						//uchar pixel_2 = Glyphs[i].at<uchar>(gr, gc);
					
						if (pixel_1 != pixel_2) {
							curr_distance += abs((int)pixel_1 - (int)pixel_2);
						}
						/*
						if(frame.at<uchar>(r+gr, c+gc) != Glyphs[i].at<uchar>(gr, gc)) {
							curr_distance += log(abs((int)frame.at<uchar>(r+gr, c+gc) - (int) Glyphs[i].at<uchar>(gr, gc)));
						}
						*/
					}
				}
				assert(curr_distance >= 0);
				if (curr_distance < min_distance) {
					min_distance = curr_distance;
					min_index = i;
				}
			}
			//printf("min index = %d\n", min_index);
			//printf("min distance = %f\n", min_distance);
		
			glyph_raw = Glyphs[min_index].data;
		
			for(int gr=0; gr<glyph_rows; gr++) {
				for(int gc=0; gc<glyph_cols; gc++) {
					//ascii.at<uchar>(r+gr, c+gc) = Glyphs[min_index].at<uchar>(gr, gc);
					ascii_raw[(r+gr)*frame_cols + (c+gc)] = glyph_raw[(gr)*glyph_cols + gc];
				}
			}
			

			//Glyphs[min_index].copyTo(ascii(Range(r, r+glyph_rows), Range(c, glyph_cols)));
			//cout << "row " << r << " col " << c << endl;
			// c = 640????????
			//Glyphs[min_index].copyTo(ascii(Rect(c, r, glyph_cols, glyph_rows)));
			
			//Mat temp = ascii(Rect(c, r, glyph_cols, glyph_rows));
			
			//cout << "Before Glyph Copy" << endl;
			//cout << temp << endl;
			
			//Glyphs[min_index].copyTo(temp);
			
			//Glyphs[min_index].copyTo(ascii(Rect(r, r+glyph_rows, glyph_rows, glyph_cols)));
			//printf("%d %d %d %d\n", r, c, glyph_rows, glyph_cols);
			//printf("Finished window (%d, %d)...\n", int(r/glyph_rows), int(c/glyph_cols));
			//cout << "Check Glyph Copy" << endl;
			//cout << Glyphs[min_index] << endl;
			//cout << ascii(Range(r, r+glyph_rows), Range(c, c+glyph_cols)) << endl;
		}
	}
	return ascii;
}


int main(int argc, char** argv)
{
    Mat frame;
	Mat ascii;
    VideoCapture cap;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    
	// Preprocess all the glyphs
	int num_glyphs = 0xFFFF;
	char glyph_path[100];

	printf("Allocating glyph array\n");	
	// Determine how many PNGs are in the directory and the size of each PNG
	// Allocate memory for that many matrices
	
	//Mat* glyphs = new Mat[num_glyphs];
	Mat* glyphs = (Mat*)calloc(num_glyphs, sizeof(Mat));
	
	/*
	for(int i=0; i<=num_glyphs; i++) {
		memset(glyph_path, '\0', 100);
		sprintf(glyph_path, "./glyphs/0x%x.png", i);
		glyphs[i-1] = 255 - imread(glyph_path, IMREAD_GRAYSCALE);
		//cout << glyphs[i-1] << endl;
	}*/
	int i=0;
	int hex=0;
	while(hex<=num_glyphs) {
		memset(glyph_path, '\0', 100);
		sprintf(glyph_path, "./glyphs/0x%x.png", hex);
		Mat temp = imread(glyph_path, IMREAD_GRAYSCALE);
		if(!temp.empty()) {
			glyphs[i] = 255 - temp;
			i++;
		}
		hex++;
	}
	/*	
	hex=0;
	while(hex<=num_glyphs) {
		memset(glyph_path, '\0', 100);
		sprintf(glyph_path, "./extra_glyphs/0x%x.png", hex);
		Mat temp = imread(glyph_path, IMREAD_GRAYSCALE);
		if(!temp.empty()) {
			glyphs[i] = 255 - temp;
			i++;
		}
		hex++;
	}
	*/
	num_glyphs = i;
	glyphs = (Mat*)realloc(glyphs, num_glyphs * sizeof(Mat));


	printf("Turning on camera\n");	
	
    cap.open(deviceID, apiID);
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
	cap.set(CAP_PROP_FPS, CAMERA_FPS );
	printf("fps: %f\n", cap.get(CAP_PROP_FPS));
	cap.set(CAP_PROP_FRAME_WIDTH, WIDTH );
	cap.set(CAP_PROP_FRAME_HEIGHT, HEIGHT );
    cout << "Press any key to quit" << endl;
    cout << "Just kidding, you have to kill the process with CTRL + C" << endl;
	while (true)
    {
        //cout << "Reading Frame" << endl;
		cap.read(frame);
        //cout << "Checking Frame" << endl;
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        //cout << "Converting Color" << endl;
		cvtColor(frame, frame, cv::COLOR_RGB2GRAY);
        //cout << "Producing ASCII" << endl;
		ascii = ASCII(frame, glyphs, num_glyphs);
        //cout << "Displaying Image" << endl;
		imshow("Live in ASCII", ascii);
        // wait as little as possible
		if (waitKey(1) >= 0)
			break;
	}
    return 0;
}
