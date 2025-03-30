#include <stdio.h>


extern "C"
{
	// include format and codec headers
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

}

void pgm_save(unsigned char* buf, int wrap, int xsize, int ysize, FILE* f) //PGM: 图像格式，unsigned char: 无符号字符，wrap: 一行的字节数，xsize: 图像宽度，ysize: 图像高度，f: 文件指针		
{
	// write header
	fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);  //P5: 图像格式，255: 最大灰度值,fprintf: 将格式化的数据写入文件
	// loop until all rows are written to file    //loop: 循环，row: 行,file: 文件
	for (int i = 0; i < ysize; i++)
		fwrite(buf + i * wrap, 1, xsize, f);	//fwrite: 将数据写入文件
}

void decode(AVCodecContext* dec_ctx, AVFrame* frame, AVPacket* pkt, FILE* f)  //AVCodecContext: 编解码器上下文，AVFrame: 帧，AVPacket: 包，f: 文件指针
{
	char buf[1024];  //buf: 缓冲区
	int ret;

	//send packet to decoder  
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) {
		fprintf(stderr, "Error sending a packet for decoding\n");  //fprintf: 将格式化的数据写入文件
		exit(1);  
	}
	while (ret >= 0) {
		// receive frame from decoder  //receive:接收,frame:帧,decoder:解码器
		// we may receive multiple frames or we may consume all data from decoder, then return to main loop   //multiple:多个,consume:消耗,loop:循环
		ret = avcodec_receive_frame(dec_ctx, frame);     
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)  
			return;
		else if (ret < 0) {
			// something wrong, quit program  
			fprintf(stderr, "Error during decoding\n");
			exit(1);
		}
		//printf("saving frame %3d\n", dec_ctx->pkt_serial);    //serial:序列号
		printf("saving frame %lld\n", frame->pts);   //pts(presentation time stamp):显示时间戳
		fflush(stdout);  //flush:刷新	
		// send frame info to writing function  //info:信息,function:函数
		pgm_save(frame->data[0], frame->linesize[0],frame->width, frame->height, f);  
	}
}


int main()
{

	// declare format and codec contexts, also codec for decoding   //declare:声明,format:格式,codec:编解码器,context:上下文,decoding:解码	
	AVFormatContext *fmt_ctx = NULL;  
	AVCodecContext *codec_ctx = NULL;
	const AVCodec *Codec = NULL; 
	int ret;
	const char *infilename = "/Users/jacklau/Movies/ffmpeg_test/input/out_1.mov";
	const char *outfilename = "/Users/jacklau/Movies/ffmpeg_test/input/out.yuv";
	int VideoStreamIndex = -1;  //index:索引

	FILE* fin = NULL; 
	FILE* fout = NULL;  

	AVFrame *frame = NULL;
	AVPacket *pkt = NULL;


	//av_register_all();   //register:注册,format:格式,codec:编解码器

	// open input file    
	if ((ret = avformat_open_input(&fmt_ctx, infilename, NULL, NULL)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot open input file\n");
		goto end;
	}

	//get stream info
	if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot get stream info\n");
		goto end;
	}

	// get video stream index
	for (int i = 0; i < fmt_ctx->nb_streams; i++)
	{
		if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)   //par(标准)，type(类型)
		{
			VideoStreamIndex = i;  //index:索引
			break;
		}
	}

	if (VideoStreamIndex < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "No video stream\n");  //log:日志
		goto end;
	}

	// dump video stream info   //dump:转储
	av_dump_format(fmt_ctx, VideoStreamIndex, infilename, false);   

	//alloc memory for codec context  
	codec_ctx = avcodec_alloc_context3(NULL);

	// retrieve codec params from format context  //retrieve:检索,params:参数
	if ((ret = avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[VideoStreamIndex]->codecpar)) < 0) //params(参数)，index(索引)，par(标准)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot get codec parameters\n");     //parameter(参数)
		goto end;
	}

	// find decoding codec   
	Codec = avcodec_find_decoder(codec_ctx->codec_id);

	if (Codec == NULL)  
	{
		av_log(NULL, AV_LOG_ERROR, "No decoder found\n"); 
		goto end;
	}

	// try to open codec
	if ((ret = avcodec_open2(codec_ctx, Codec, NULL)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
		goto end;
	}

	printf("\nDecoding codec is : %s\n", Codec->name);   //printf(格式化输出)

	//init packet  //init(初始化)
	pkt = av_packet_alloc();   //alloc(分配)
	av_packet_alloc(); 
	if (!pkt)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot init packet\n"); 
		goto end;
	}

	// init frame   //frame(框架)
	frame = av_frame_alloc();
	if (!frame)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot init frame\n");
		goto end;
	}

	//open input file   
	fin = fopen(infilename, "rb"); 
	if (!fin)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");   //log(日志)
		goto end;
	}

	// open output file
	fout = fopen(outfilename, "w");
	if (!fout)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open output file\n");
		goto end;
	}

	// main loop   //loop(循环)
	while (1)
	{
		// read an encoded packet from file    //encode(编码)
		if ((ret = av_read_frame(fmt_ctx, pkt)) < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "cannot read frame");  //frame(帧)   
			break;
		}
		// if packet data is video data then send it to decoder   //decoder(解码器)
		if (pkt->stream_index == VideoStreamIndex)   //index(索引)
		{
			decode(codec_ctx, frame, pkt, fout);  
		}

		// release packet buffers to be allocated again  //release(释放)，buffer(缓存区)，allocate(分配)
		av_packet_unref(pkt);  //pkt(数据包)
	}

	//flush decoder    //flush(冲洗)
	decode(codec_ctx, frame, NULL, fout);

	// clear and out
end:
	if (fin)
		fclose(fin);
	if (fout)
		fclose(fout);
	if (codec_ctx)
		avcodec_close(codec_ctx);
	if (fmt_ctx)
		avformat_close_input(&fmt_ctx);
	if (frame)
		av_frame_free(&frame);
	if (pkt)
		av_packet_free(&pkt);


	return 0;
}