/*
 * copyright (c) 2024 Jack Lau        //copy right(版权)
 * 
 * This file is a tutorial about moving or deleting a file through ffmpeg API   //tutorial(教程)
 * 
 * FFmpeg version 4.1.7 
 * Tested on Ubuntu 22.04, compiled with GCC 11.4.0    //tested on(测试)  compiled with(编译)
 */
#include <libavformat/avformat.h>       

int main(int argc, char* argv[]) //arguement(参数) count, arguement vector（向量）
{
    int ret;
	//rename a file  //rename(重命名)
	ret = avpriv_io_move("111.txt", "222.txt");   //private(私有的) io_move(移动)
    if(ret<0){
        av_log(NULL, AV_LOG_ERROR, "Failed to rename the file \n");
        return -1;
    }
    av_log(NULL, AV_LOG_INFO, "Success to rename\n");

	//delete a file through url  //url(统一资源定位符)
	ret = avpriv_io_delete("./mytestfile.txt");   //private(私有的)
    if(ret<0){
		av_log(NULL, AV_LOG_ERROR, "Failed to delete the file %s\n", "mytestfile.txt"); 
        return -1;
    }
    av_log(NULL, AV_LOG_INFO, "Success to delete mytestfile.txt\n");
    return 0;
}