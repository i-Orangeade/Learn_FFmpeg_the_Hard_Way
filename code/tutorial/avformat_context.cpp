#include <stdio.h>    

// libavformat deals with the capsule of video   //library(��),format(��ʽ),capsule(����)

// if your code c++, you can include c headers with an extern encapsulation  //encapsulation(��װ),extern(�ⲿ��)
extern "C"    //extern "C" ������C++�е���C���Եĺ���
{
#include <libavformat/avformat.h>  //format(��ʽ)
}

int main()
{	

	AVFormatContext *fmt_ctx = NULL;   //Context(����)��null(��ֵ)
	int ret;   //ret(����ֵ)
	const char* filename = "D:\\Wildlife.wmv";   //const(����)��char(�ַ�)��filename(�ļ���)
	

	//av_register_all();  //register(�Ǽ�),codecs(�������)

	//fill format context with file info    //file(�ļ�)
	if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)))
		return ret;	

	// get streams info from format context   
	if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
		return ret;
	}

	// loop streams and dump info     //loop(ѭ��),dump(ת��)
	for (int i = 0; i < fmt_ctx->nb_streams; i++)
	{
		av_dump_format(fmt_ctx, i, filename, false);		
	}

	// close format context
	avformat_close_input(&fmt_ctx);

	system("PAUSE");  //pause(��ͣ)

	return 0;

}