#ifndef TT_CONTOURS_H

#define TT_CONTOURS_H
#define useiofiles 1

#include "cv.h"
#include "highgui.h"

#define ERR_RESOLUTION 1000000
#define ERR_TOLERANCE 400000
#define MINAREA 400
#define TESTING 0
#define skale1 1.5E3
#define skale2 2.5E3
#define skale3 0.3E5
#define skale4 1.6E5
#define skale5 2E7
#define skale6 1E5
#define skale7 1E8
#define lim 800


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
#endif
