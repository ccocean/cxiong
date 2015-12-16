#include "decode.h"


//extern CListBox m_listErr;
//extern CMFCTrackToolsDlg *g_pDlg;

static int cur_size;
static int ret;
static int got_picture;
static int y_size;

int H264_init(Tools_decoder_t* decoder)
{
	if (!decoder)
	{
		return -1;
	}
	//avcodec_init();
	avcodec_register_all();

	/* find the h264 video decoder */
	decoder->pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!decoder->pCodec) {
		//g_pDlg->m_listErr.AddString(_T("codec not found!\r\n"));
		//m_listErr.AddString(_T("codec not found!\r\n"));
		MessageBox(NULL, TEXT("解码器未找到。"), TEXT("标题"), MB_OK);
		return -1;
	}

	decoder->pCodecCtx = avcodec_alloc_context3(decoder->pCodec);
	if (!decoder->pCodecCtx)
	{
		//m_listErr.AddString(_T("Could not allocate video codec context\r\n"));
		MessageBox(NULL, TEXT("无法分配视频解码器上下文。"), TEXT("标题"), MB_OK);
		return -1;
	}

	decoder->pCodecParserCtx = av_parser_init(AV_CODEC_ID_H264);
	if (!decoder->pCodecParserCtx){
		//m_listErr.AddString(_T("Could not allocate video parser context\n"));
		MessageBox(NULL, TEXT("无法分配视频分割器上下文。"), TEXT("标题"), MB_OK);
		return -1;
	}

	if (avcodec_open2(decoder->pCodecCtx,decoder->pCodec,NULL)<0)
	{
		//m_listErr.AddString(_T("Could not open codec\r\n"));
		MessageBox(NULL, TEXT("无法打开解码器。"), TEXT("标题"), MB_OK);
		return -1;
	}

	decoder->pFrame = av_frame_alloc();
	if (decoder->pFrame==NULL)
	{
		return -1;
	}

	decoder->pFrameRGB = av_frame_alloc();
	if (decoder->pFrameRGB==NULL)
	{
		return -1;
	}
	av_init_packet(&decoder->packet);
	decoder->firstTime = 1;

	return 0;
}

int H264_To_RGB(unsigned char *inputbuffer, int frame_size, unsigned char *&outputbuffer, Tools_decoder_t *decoder)
{
	if (!inputbuffer||!decoder)
	{
		return -1;
	}
	
	int             decode_size=0;
	int             av_result=0;
	
	decoder->packet.data = inputbuffer;
	decoder->packet.size = frame_size;

	//decoder->cur_ptr = inputbuffer;

	av_result = avcodec_decode_video2(decoder->pCodecCtx, decoder->pFrame, &decode_size, &decoder->packet);

	if (av_result < 0)
	{
		return -1;
	}

	if (decode_size)
	{
		
		if (decoder->firstTime)
		{
			uint8_t         *buffer = NULL;
			int initSize = 0;
			decoder->img_convert_ctx = sws_getContext(decoder->pCodecCtx->width, decoder->pCodecCtx->height, decoder->pCodecCtx->pix_fmt,
				WIDTH, HEIGHT, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
			initSize = av_image_get_buffer_size(AV_PIX_FMT_BGR24, decoder->pCodecCtx->width, decoder->pCodecCtx->height, 16);
			buffer = (uint8_t *)malloc(initSize*sizeof(uint8_t));
			memset(buffer, 0, initSize);
			av_image_fill_arrays(decoder->pFrameRGB->data, decoder->pFrameRGB->linesize, buffer, AV_PIX_FMT_BGR24, decoder->pCodecCtx->width, decoder->pCodecCtx->height, 16);

			decoder->firstTime = 0;
		}
		/*else
		{
			buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24, decoder->pCodecCtx->width, decoder->pCodecCtx->height, 16)*sizeof(uint8_t));
			av_image_fill_arrays(decoder->pFrameRGB->data, decoder->pFrameRGB->linesize, buffer, AV_PIX_FMT_BGR24, decoder->pCodecCtx->width, decoder->pCodecCtx->height, 16);
		}*/
		sws_scale(decoder->img_convert_ctx, (const uint8_t* const*)decoder->pFrame->data, decoder->pFrame->linesize, 0, decoder->pCodecCtx->height,
			decoder->pFrameRGB->data, decoder->pFrameRGB->linesize);


		for (int i = 0; i < HEIGHT;i++)
		{
			memcpy(outputbuffer + WIDTH * 3 * i, (decoder->pFrameRGB->linesize[0])*i + decoder->pFrameRGB->data[0], WIDTH * 3);
		}
	}
	else
	{
		return -1;
	}
	

	return 0;
}