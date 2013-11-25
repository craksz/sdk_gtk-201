#ifndef TT_CONTOURS_H

#define TT_CONTOURS_H
#define useiofiles 1

#include "cv.h"
#include "highgui.h"

#define TEST_MODE_XY            0
#define TEST_MODE_XY_TOP        15
#define TEST_MODE_XY_X          0
#define TEST_MODE_XY_Y          0
#define SAVE_VIDEO              0
#define USE_FRONTAL             0

#define ERR_RESOLUTION          1000000
#define ERR_TOLERANCE           400000
#define MINAREA                 400
#define TESTING                 0
#define HU_MOMENTS_1_SCALE      1.5E3
#define HU_MOMENTS_2_SCALE      2.5E3
#define HU_MOMENTS_3_SCALE      0.3E5
#define HU_MOMENTS_4_SCALE      1.6E5
#define HU_MOMENTS_5_SCALE      2E7
#define HU_MOMENTS_6_SCALE      1E5
#define HU_MOMENTS_7_SCALE      1E8
#define HU_MOMENTS_LIMIT        800

#define VIDEO_VERT_WIDTH        176
#define VIDEO_VERT_HEIGHT       144
#define VIDEO_SAVE              0


typedef struct theTTModels{
	IplImage *model1;
	IplImage *model2;
	IplImage *model3;
	CvSeq* c1;
	CvSeq* c2;
	CvSeq* c3;
	double e1;
	double e2;
	double e3;
} ttModels;


CvSeq* ttFindContours(IplImage * );

CvSeq* ttContours(IplImage *src, int mode, ttModels *theModels, double *theErr);

void ttInit(ttModels *theModels);

void ttSegmenter(IplImage *src, IplImage *dst, int color);

void ttImprover(IplImage *src,IplImage *dst);

CvBox2D ttFindBox(CvSeq* theContour);

void ttCalibration(IplImage *src);

double ttGetClassValue(int H2, int H1);

double ttGetAndDrawClassValue(int theH1,int theH2);

void ttDrawDirections(IplImage * outputFrame,int use_contours);

int ttMain(IplImage * theFrame);
#endif
