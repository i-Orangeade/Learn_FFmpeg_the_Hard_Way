#include <stdio.h>    

// libavformat deals with the capsule of video   //library(库),format(格式),capsule(容器)

// if your code c++, you can include c headers with an extern encapsulation  //encapsulation(封装),extern(外部的)
extern "C"    //extern "C" 用于在C++中调用C语言的函数
{
#include <libavformat/avformat.h>  //format(格式)
}

int main()
{	

	AVFormatContext *fmt_ctx = NULL;   //Context(比赛)，null(空值)
	int ret;   //ret(返回值)
	const char* filename = "D:\\Wildlife.wmv";   //const(常数)，char(字符)，filename(文件名)
	

	//av_register_all();  //register(登记),codecs(编解码器)

	//fill format context with file info    //file(文件)
	if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)))
		return ret;	

	// get streams info from format context   
	if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
		return ret;
	}

	// loop streams and dump info     //loop(循环),dump(转储)
	for (int i = 0; i < fmt_ctx->nb_streams; i++)
	{
		av_dump_format(fmt_ctx, i, filename, false);		
	}

	// close format context
	avformat_close_input(&fmt_ctx);

	system("PAUSE");  //pause(暂停)

	return 0;

}