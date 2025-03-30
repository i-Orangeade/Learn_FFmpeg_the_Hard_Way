#include <stdio.h>


extern "C"
{
	// include format and codec headers
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

}

void pgm_save(unsigned char* buf, int wrap, int xsize, int ysize, FILE* f) //PGM: ͼ���ʽ��unsigned char: �޷����ַ���wrap: һ�е��ֽ�����xsize: ͼ���ȣ�ysize: ͼ��߶ȣ�f: �ļ�ָ��		
{
	// write header
	fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);  //P5: ͼ���ʽ��255: ���Ҷ�ֵ,fprintf: ����ʽ��������д���ļ�
	// loop until all rows are written to file    //loop: ѭ����row: ��,file: �ļ�
	for (int i = 0; i < ysize; i++)
		fwrite(buf + i * wrap, 1, xsize, f);	//fwrite: ������д���ļ�
}

void decode(AVCodecContext* dec_ctx, AVFrame* frame, AVPacket* pkt, FILE* f)  //AVCodecContext: ������������ģ�AVFrame: ֡��AVPacket: ����f: �ļ�ָ��
{
	char buf[1024];  //buf: ������
	int ret;

	//send packet to decoder  
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) {
		fprintf(stderr, "Error sending a packet for decoding\n");  //fprintf: ����ʽ��������д���ļ�
		exit(1);  
	}
	while (ret >= 0) {
		// receive frame from decoder  //receive:����,frame:֡,decoder:������
		// we may receive multiple frames or we may consume all data from decoder, then return to main loop   //multiple:���,consume:����,loop:ѭ��
		ret = avcodec_receive_frame(dec_ctx, frame);     
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)  
			return;
		else if (ret < 0) {
			// something wrong, quit program  
			fprintf(stderr, "Error during decoding\n");
			exit(1);
		}
		//printf("saving frame %3d\n", dec_ctx->pkt_serial);    //serial:���к�
		printf("saving frame %lld\n", frame->pts);   //pts(presentation time stamp):��ʾʱ���
		fflush(stdout);  //flush:ˢ��	
		// send frame info to writing function  //info:��Ϣ,function:����
		pgm_save(frame->data[0], frame->linesize[0],frame->width, frame->height, f);  
	}
}


int main()
{

	// declare format and codec contexts, also codec for decoding   //declare:����,format:��ʽ,codec:�������,context:������,decoding:����	
	AVFormatContext *fmt_ctx = NULL;  
	AVCodecContext *codec_ctx = NULL;
	const AVCodec *Codec = NULL; 
	int ret;
	const char *infilename = "/Users/jacklau/Movies/ffmpeg_test/input/out_1.mov";
	const char *outfilename = "/Users/jacklau/Movies/ffmpeg_test/input/out.yuv";
	int VideoStreamIndex = -1;  //index:����

	FILE* fin = NULL; 
	FILE* fout = NULL;  

	AVFrame *frame = NULL;
	AVPacket *pkt = NULL;


	//av_register_all();   //register:ע��,format:��ʽ,codec:�������

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
		if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)   //par(��׼)��type(����)
		{
			VideoStreamIndex = i;  //index:����
			break;
		}
	}

	if (VideoStreamIndex < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "No video stream\n");  //log:��־
		goto end;
	}

	// dump video stream info   //dump:ת��
	av_dump_format(fmt_ctx, VideoStreamIndex, infilename, false);   

	//alloc memory for codec context  
	codec_ctx = avcodec_alloc_context3(NULL);

	// retrieve codec params from format context  //retrieve:����,params:����
	if ((ret = avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[VideoStreamIndex]->codecpar)) < 0) //params(����)��index(����)��par(��׼)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot get codec parameters\n");     //parameter(����)
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

	printf("\nDecoding codec is : %s\n", Codec->name);   //printf(��ʽ�����)

	//init packet  //init(��ʼ��)
	pkt = av_packet_alloc();   //alloc(����)
	av_packet_alloc(); 
	if (!pkt)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot init packet\n"); 
		goto end;
	}

	// init frame   //frame(���)
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
		av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");   //log(��־)
		goto end;
	}

	// open output file
	fout = fopen(outfilename, "w");
	if (!fout)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open output file\n");
		goto end;
	}

	// main loop   //loop(ѭ��)
	while (1)
	{
		// read an encoded packet from file    //encode(����)
		if ((ret = av_read_frame(fmt_ctx, pkt)) < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "cannot read frame");  //frame(֡)   
			break;
		}
		// if packet data is video data then send it to decoder   //decoder(������)
		if (pkt->stream_index == VideoStreamIndex)   //index(����)
		{
			decode(codec_ctx, frame, pkt, fout);  
		}

		// release packet buffers to be allocated again  //release(�ͷ�)��buffer(������)��allocate(����)
		av_packet_unref(pkt);  //pkt(���ݰ�)
	}

	//flush decoder    //flush(��ϴ)
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