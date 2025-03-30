/*
 * copyright (c) 2024 Jack Lau   
 * 
 * This file is a tutorial about listing files through ffmpeg API  //tutorial(教程)，list(列出)
 * 
 * FFmpeg version 4.1.7 
 * Tested on Ubuntu 22.04, compiled with GCC 11.4.0   //compiled(编译)
 */
#include <libavformat/avformat.h>

int main(int argc, char *argv[])
{
    int ret; 
    //init
	AVIODirContext* ctx = NULL;   //avoidircontext(避免目录上下文)
	AVIODirEntry* entry = NULL;	 //avoid(避免)，entry(条目)，directory(目录)
	av_log_set_level(AV_LOG_INFO);  //log(日志)，set(设置)，level(级别)
    //open the directory
    ret = avio_open_dir(&ctx, "./", NULL);  //
    if(ret < 0){
        av_log(NULL, AV_LOG_ERROR, "failed to open the directory:%s\n",av_err2str(ret));
        goto end;
    }
    //read the files
    while(1){
		ret = avio_read_dir(ctx, &entry);   //ctx(上下文)，entry(条目)
        if(ret < 0){
            av_log(NULL, AV_LOG_ERROR, "failed to read the directory:%s\n",av_err2str(ret));
            goto end;
        }
		//if all of the files listed, then break the loop  //listed(列出),break(中断),loop(循环)
        if(!entry){
            break;
        }
		//print the info of file's size and name   print(打印)
        av_log(NULL, AV_LOG_INFO, "%12"PRId64" %s \n",  
                entry->size,
                entry->name);
		//free the memory of entry   //free(释放),memory(内存),entry(条目) 
		avio_free_directory_entry(&entry);   //free(释放),directory(目录),entry(条目)
    }
    
end:
    //free the memory of ctx
    avio_close_dir(&ctx);

    return 0;
}