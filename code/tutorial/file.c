/*
 * copyright (c) 2024 Jack Lau        //copy right(��Ȩ)
 * 
 * This file is a tutorial about moving or deleting a file through ffmpeg API   //tutorial(�̳�)
 * 
 * FFmpeg version 4.1.7 
 * Tested on Ubuntu 22.04, compiled with GCC 11.4.0    //tested on(����)  compiled with(����)
 */
#include <libavformat/avformat.h>       

int main(int argc, char* argv[]) //arguement(����) count, arguement vector��������
{
    int ret;
	//rename a file  //rename(������)
	ret = avpriv_io_move("111.txt", "222.txt");   //private(˽�е�) io_move(�ƶ�)
    if(ret<0){
        av_log(NULL, AV_LOG_ERROR, "Failed to rename the file \n");
        return -1;
    }
    av_log(NULL, AV_LOG_INFO, "Success to rename\n");

	//delete a file through url  //url(ͳһ��Դ��λ��)
	ret = avpriv_io_delete("./mytestfile.txt");   //private(˽�е�)
    if(ret<0){
		av_log(NULL, AV_LOG_ERROR, "Failed to delete the file %s\n", "mytestfile.txt"); 
        return -1;
    }
    av_log(NULL, AV_LOG_INFO, "Success to delete mytestfile.txt\n");
    return 0;
}