/*
 * copyright (c) 2024 Jack Lau
 * 
 * This file is a tutorial about using the log system in ffmpeg 
 * 
 * FFmpeg version 5.0
 * Tested on Ubuntu 22.04, compiled with GCC 11.4.0   //compile(����)
 */
#include <stdio.h>
#include <libavutil/log.h>     //utility(����)
//use the log system of ffmpeg 

int main(int argc, char* argv[])   //argument count(���α���), argument vector��������
{
	//set the log level  //level(����), set(����)
	av_log_set_level(AV_LOG_DEBUG); 

	//print the log   //print(��ӡ)
    av_log(NULL,AV_LOG_INFO,"Hello world! %s \n","test");

    return 0;
}