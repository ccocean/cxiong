#include "tch_track.h"
#include "Queue.h"
#include<opencv/cv.h>
#include<opencv/highgui.h>
#include "switch_strategy.h"
#include "Strategy_cameraControl.h"
//#include "client_cameraControl.h"

#define WIDTH 480
#define HEIGHT 264

CvMat MatToCV(Itc_Mat_t *src)
{
	CvMat m;

	m.type = src->type;
	m.cols = src->cols;
	m.rows = src->rows;
	m.step = src->step;
	m.data.ptr = src->data.ptr;
	m.refcount = src->refcount;
	m.hdr_refcount = src->hdr_refcount;
	return m;
}

int main()
{
	Strategy_CamControl_t *cam_tch;
	cam_tch = (Strategy_CamControl_t*)malloc(sizeof(Strategy_CamControl_t));
	init_cam(cam_tch);

	int iResult = 0;
	WSADATA wsaData = { 0 };
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	startControl(cam_tch, "192.168.11.167", 1259);


	Itc_Mat_t *srcMat = itc_create_mat(HEIGHT, WIDTH, ITC_8UC1);//360,640
	//cvNamedWindow("Src", 1);
	//cvNamedWindow("teacher", 1);
	char* g_videoPath;
	char* src=0;
	//int g_count = 0;
	g_videoPath = "video/teacher.mp4";
	/*g_frameSize = itcSize(640, 360);
	g_tchWin = itcRect(0, 100, 640, 200);
	g_blkWin = itcRect(0, 35, 640, 50);*/
	Tch_Size_t _frame = { WIDTH, HEIGHT };
	Tch_Rect_t tch = { 0, 0, WIDTH, 150 };//{0,75,480,150}
	Tch_Rect_t blk = { 0, 26, WIDTH, 37 }; //{0, 35, 640, 50}
	Tch_Threshold_t threshold = { 2, 7200, 95 };//{2, 12000, 130}
	TeaITRACK_Params *argument;
	StuITRACK_OutParams_t *stu;
	Strategy_Result_t *stra;
	stra = (Strategy_Result_t *)malloc(sizeof(Strategy_Result_t));
	init_strategy(stra);
	stu = (StuITRACK_OutParams_t *)malloc(sizeof(StuITRACK_OutParams_t));
	stu->count_trackObj_bigMove = -1;
	stu->count_trackObj_stand = -1;
	stu->result_flag = 0;
	argument = (TeaITRACK_Params *)malloc(sizeof(TeaITRACK_Params));
	argument->frame = _frame;
	argument->tch = tch;
	argument->blk = blk;
	argument->threshold = threshold;
	//argument->isSetParams = 1;
	argument->numOfPos = 10;
	argument->numOfSlide = 5;
	argument->isSetParams = 0;
	Tch_Result_t *res;
	res = (Tch_Result_t*)malloc(sizeof(Tch_Result_t));
	Tch_Data_t *data;
	//data->callbackmsg_func = printf;
	data = (Tch_Data_t*)malloc(sizeof(Tch_Data_t));
	int err = tch_Init(argument, data);
	if (err<0)
	{
		printf("error\n\r");
	}
	

	CvCapture* s_video = 0;
	IplImage *frame = NULL;
	IplImage *imgSrc = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);
	IplImage *grayImg = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);
	IplImage *yuvImg = cvCreateImage(cvSize(WIDTH, HEIGHT*3/2), 8, 1);
	CvMat testMat;
	uchar *uv;
	uv = malloc(sizeof(uchar));

	cvNamedWindow("test", 1);

	s_video = cvCaptureFromFile(g_videoPath);
	int result;
	int last_status=-1;
	int last_pos = -1;
	int g_count = 0;
	clock_t start,end;
	double delta;
	while (1)
	{
		frame = cvQueryFrame(s_video);

		if (!frame)
		{
			return -1; break;
		}

		cvResize(frame, imgSrc, 1);
		//cvShowImage("Src", imgSrc);
		imgSrc->origin = frame->origin;
		cvCvtColor(imgSrc, grayImg, CV_RGB2GRAY);
		cvCvtColor(imgSrc, yuvImg, CV_RGB2YUV_I420);
		//memcpy(srcMat->data.ptr, grayImg->imageData, srcMat->step*srcMat->rows);
		//cvShowImage("test", imgSrc);
		testMat = MatToCV(data->mhiMatTch);
		cvShowImage("teacher", &testMat);
		start = clock();
		result = tch_track(yuvImg->imageData,uv,argument,data,res);
		int strategy = switch_strategy(res, stu, stra,cam_tch);
		end = clock();
		//if (/*last_status != res->status || last_pos != res->pos*/strategy == 1)
		//{
			delta = (end - start);
			if (strategy == 1)
			{
				printf("status:%d, position:%d, time:%f\r\n", stra->tch_status, stra->tch_posIndex, (double)(delta / CLOCKS_PER_SEC));
			}
			
			//printf("status:%d, position:%d, time:%f\r\n", res->status, res->pos, (double)(delta / CLOCKS_PER_SEC));
			/*last_status = res->status;
			last_status = res->status;
			last_pos = res->pos;*/
		//}
		cvCvtColor(yuvImg, imgSrc, CV_YUV2BGR_I420);
		cvShowImage("test", imgSrc);
		
		cvWaitKey(1);
		g_count++;
	}
	tch_trackDestroy(data);

	return 0;
}