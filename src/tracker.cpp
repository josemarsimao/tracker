// Author: Josemar Simão
// Reference:
//              https://www.learnopencv.com/object-tracking-using-opencv-cpp-python/
///             https://docs.opencv.org/4.3.0/d2/d0a/tutorial_introduction_to_tracker.html




#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/highgui/highgui_c.h>


#include "kbhit.h"


using namespace std;
using namespace cv;

char ctrlr = '.';
char fn[1024];

/// TRACKER TYPES  ---------------------------------
//   1 - cv::TrackerBoosting
//   2 - cv::TrackerMIL
//   3 - cv::TrackerKCF
//   4 - cv::TrackerTLD
//   5 - cv::TrackerMedianFlow
//   6 - cv::TrackerGOTURN
//   7 - cv::TrackerMOSSE
//   8 - cv::TrackerCSRT
/// ---------------------------------------------------

#define TRACKER_TYPE   2

static Ptr<Tracker> CreateTracker(){
#if(TRACKER_TYPE == 1)
    return cv::TrackerBoosting::create();
#elif(TRACKER_TYPE == 2)
    return cv::TrackerMIL::create();                /// good
#elif(TRACKER_TYPE == 3)
    return cv::TrackerKCF::create();
#elif(TRACKER_TYPE == 4)
    return cv::TrackerTLD::create();                /// detection error
#elif(TRACKER_TYPE == 5)
    return cv::TrackerMedianFlow::create();         /// reference error
#elif(TRACKER_TYPE == 6)
    return cv::TrackerGOTURN::create();             /// doesnt work
#elif(TRACKER_TYPE == 7)
    return cv::TrackerMOSSE::create();              /// reference error
#elif(TRACKER_TYPE == 8)
    return cv::TrackerCSRT::create();               /// good
#endif
}

int main(){


    Mat frame;
    Mat img_matches;
    Mat imCrop1;
    Mat imCrop2;
    Mat imRef;

    Rect2d rbox;

    namedWindow("Tracking", CV_WINDOW_AUTOSIZE); // Create a window
    startWindowThread();

    FILE *f = popen("zenity --file-selection", "r"); // popen starts a new process. zenity is a process that creates a dialog box
    fgets(fn, 1024, f);
    /// There is a '/n' character in the end of string
    int sz = strlen(fn);
    if(sz == 0){
        return -1;
    }
    fn[sz-1] = 0;

    VideoCapture cap(fn);

    if(!cap.open(fn)){
        return 0;
    }

    /// Select ROI from first image
    cap >> frame;
    /// Select ROI
    rbox = selectROI(frame);
    /// Display bounding box.
    rectangle(frame, rbox, Scalar( 255, 0, 0 ), 2, 1 );
    /// Show image
    imshow("Tracking", frame);
    waitKey(1);




    /// Create Tracker
    Ptr<Tracker> tracker = CreateTracker();

    tracker->init(frame, rbox);

    while (ctrlr != 'q'){
        while(_kbhit()){
            ctrlr = getch();
        }
        cap >> frame;
        if(frame.empty()){
            break;
        }

        int ok = tracker->update(frame, rbox);
        if(ok){
            // Tracking success : Draw the tracked object
	        rectangle(frame, rbox, Scalar( 255, 0, 0 ), 2, 1 );
        }else{
            // Tracking failure detected.
            putText(frame, "Tracking failure detected", Point(100,80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
        }

        // Display tracker type on frame
        ///putText(frame, " Tracker", Point(100,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50),2);

        // Display FPS on frame
        ///putText(frame, "FPS : " + SSTR(int(fps)), Point(100,50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);

        imshow("Tracking", frame);
        waitKey(1);

    }


    cap.release();
    destroyAllWindows();

    return 0;
}
