#include <stdio.h>


extern "C" 
{
	// include format and codec headers   //format(��ʽ) 
#include <libavformat\avformat.h>  
#include <libavcodec\avcodec.h>   

}


int main()
{
	// declare format and codec contexts, also codec for decoding   //declare(����),context(������),codec(�������),decoding(����)
	AVFormatContext* fmt_ctx = NULL;   //AVFormatContext(��ʽ������)
	AVCodecContext* CodecCtx = NULL;   //AVCodecContext(�������������)
	const AVCodec* Codec = NULL;    //const(����),AVCodec(�������)
	int ret;
	const char* filename = "D:\\Wildlife.wmv";   //filename(�ļ���)
	int VideoStreamID = -1;   //VideoStreamID(��Ƶ��ID)


	//av_register_all();    //register(ע��)

	// open input file     
	if (ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot open input file\n");	   //log(��־)
		goto end;    //goto(ת��)
	}
		
	//get stream info  
	if (ret = avformat_find_stream_info(fmt_ctx, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot get stream info\n"); 
		goto end;
	}
		
	// get video stream index   //index(����)
	for (int i = 0; i < fmt_ctx->nb_streams; i++)
	{
		if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)   //par(��׼),type(����),media(ý��),video(��Ƶ),type(����)
		{
			VideoStreamID = i;      
			break;
		}
	}

	if (VideoStreamID < 0)      
	{
		av_log(NULL, AV_LOG_ERROR, "No video stream\n");
		goto end;
	}

	// dump video stream info
	av_dump_format(fmt_ctx, VideoStreamID, filename, false);    //dump(ת��),format(��ʽ),false(��)

	//alloc memory for codec context    //alloc(����),memory(�ڴ�)
	CodecCtx = avcodec_alloc_context3(NULL);

	// retrieve codec params from format context   //retrieve(����),params(����),format(��ʽ),context(������),codec(�������)
	if (ret = avcodec_parameters_to_context(CodecCtx, fmt_ctx->streams[VideoStreamID]->codecpar) < 0)   //parameters(����),context(������),codec(�������)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot get codec parameters\n");   //	log(��־)
		goto end;
	}
		
	// find decoding codec 
	Codec = avcodec_find_decoder(CodecCtx->codec_id);

	if (Codec == NULL)
	{
		av_log(NULL, AV_LOG_ERROR, "No decoder found\n");     
		goto end;
	}

	// try to open codec
	if (ret = avcodec_open2(CodecCtx, Codec, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n"); 
		goto end;
	}

	printf("\nDecoding codec is : %s\n", Codec->name);   //printf(��ӡ)


	// clear and out
	end:
	if (CodecCtx)
		avcodec_close(CodecCtx);
	if (fmt_ctx)
		avformat_close_input(&fmt_ctx);	
	system("PAUSE");  //system(ϵͳ)

	return 0;
}