/*
 * copyright (c) 2024 Jack Lau   
 * 
 * This file is a tutorial about listing files through ffmpeg API  //tutorial(�̳�)��list(�г�)
 * 
 * FFmpeg version 4.1.7 
 * Tested on Ubuntu 22.04, compiled with GCC 11.4.0   //compiled(����)
 */
#include <libavformat/avformat.h>

int main(int argc, char *argv[])
{
    int ret; 
    //init
	AVIODirContext* ctx = NULL;   //avoidircontext(����Ŀ¼������)
	AVIODirEntry* entry = NULL;	 //avoid(����)��entry(��Ŀ)��directory(Ŀ¼)
	av_log_set_level(AV_LOG_INFO);  //log(��־)��set(����)��level(����)
    //open the directory
    ret = avio_open_dir(&ctx, "./", NULL);  //
    if(ret < 0){
        av_log(NULL, AV_LOG_ERROR, "failed to open the directory:%s\n",av_err2str(ret));
        goto end;
    }
    //read the files
    while(1){
		ret = avio_read_dir(ctx, &entry);   //ctx(������)��entry(��Ŀ)
        if(ret < 0){
            av_log(NULL, AV_LOG_ERROR, "failed to read the directory:%s\n",av_err2str(ret));
            goto end;
        }
		//if all of the files listed, then break the loop  //listed(�г�),break(�ж�),loop(ѭ��)
        if(!entry){
            break;
        }
		//print the info of file's size and name   print(��ӡ)
        av_log(NULL, AV_LOG_INFO, "%12"PRId64" %s \n",  
                entry->size,
                entry->name);
		//free the memory of entry   //free(�ͷ�),memory(�ڴ�),entry(��Ŀ) 
		avio_free_directory_entry(&entry);   //free(�ͷ�),directory(Ŀ¼),entry(��Ŀ)
    }
    
end:
    //free the memory of ctx
    avio_close_dir(&ctx);

    return 0;
}