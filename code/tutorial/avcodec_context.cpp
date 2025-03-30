#include <stdio.h>


extern "C" 
{
	// include format and codec headers   //format(格式) 
#include <libavformat\avformat.h>  
#include <libavcodec\avcodec.h>   

}


int main()
{
	// declare format and codec contexts, also codec for decoding   //declare(声明),context(上下文),codec(编解码器),decoding(解码)
	AVFormatContext* fmt_ctx = NULL;   //AVFormatContext(格式上下文)
	AVCodecContext* CodecCtx = NULL;   //AVCodecContext(编解码器上下文)
	const AVCodec* Codec = NULL;    //const(常量),AVCodec(编解码器)
	int ret;
	const char* filename = "D:\\Wildlife.wmv";   //filename(文件名)
	int VideoStreamID = -1;   //VideoStreamID(视频流ID)


	//av_register_all();    //register(注册)

	// open input file     
	if (ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot open input file\n");	   //log(日志)
		goto end;    //goto(转到)
	}
		
	//get stream info  
	if (ret = avformat_find_stream_info(fmt_ctx, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot get stream info\n"); 
		goto end;
	}
		
	// get video stream index   //index(索引)
	for (int i = 0; i < fmt_ctx->nb_streams; i++)
	{
		if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)   //par(标准),type(类型),media(媒体),video(视频),type(类型)
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
	av_dump_format(fmt_ctx, VideoStreamID, filename, false);    //dump(转储),format(格式),false(假)

	//alloc memory for codec context    //alloc(分配),memory(内存)
	CodecCtx = avcodec_alloc_context3(NULL);

	// retrieve codec params from format context   //retrieve(检索),params(参数),format(格式),context(上下文),codec(编解码器)
	if (ret = avcodec_parameters_to_context(CodecCtx, fmt_ctx->streams[VideoStreamID]->codecpar) < 0)   //parameters(参数),context(上下文),codec(编解码器)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot get codec parameters\n");   //	log(日志)
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

	printf("\nDecoding codec is : %s\n", Codec->name);   //printf(打印)


	// clear and out
	end:
	if (CodecCtx)
		avcodec_close(CodecCtx);
	if (fmt_ctx)
		avformat_close_input(&fmt_ctx);	
	system("PAUSE");  //system(系统)

	return 0;
}