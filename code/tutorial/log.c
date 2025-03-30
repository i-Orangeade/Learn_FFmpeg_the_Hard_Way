/*
 * copyright (c) 2024 Jack Lau
 * 
 * This file is a tutorial about using the log system in ffmpeg 
 * 
 * FFmpeg version 5.0
 * Tested on Ubuntu 22.04, compiled with GCC 11.4.0   //compile(编译)
 */
#include <stdio.h>
#include <libavutil/log.h>     //utility(工具)
//use the log system of ffmpeg 

int main(int argc, char* argv[])   //argument count(整形变量), argument vector（向量）
{
	//set the log level  //level(级别), set(设置)
	av_log_set_level(AV_LOG_DEBUG); 

	//print the log   //print(打印)
    av_log(NULL,AV_LOG_INFO,"Hello world! %s \n","test");

    return 0;
}