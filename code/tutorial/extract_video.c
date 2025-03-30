/*
 * copyright (c) 2024 Jack Lau
 * 
 * This file is a tutorial about extrating video stream from a container into a new container through ffmpeg API
 *                                //提取                                                                //应用程序接口
 * FFmpeg version 5.0.3  
 * Tested on MacOS 14.1.2, compiled with clang 14.0.3  //compiled(编译)
 */

#include <libavformat/avformat.h>    
#include <libavutil/avutil.h>   //util(工具)，avutil是一个工具库，包含了很多工具函数

int main(int argc, char* argv[])  //argc是参数个数，argv是参数数组
{
	int ret = -1;  //integer(整数)
	int idx = -1;  //index(索引)
    //deal with arguments
	char* src;     //source(源)
	char* dst;	 //destination(目的地)

	AVFormatContext *pFmtCtx = NULL;  
    AVFormatContext *oFmtCtx = NULL;  

    const AVOutputFormat *outFmt = NULL; 
    AVStream *inStream = NULL;
    AVStream *outStream = NULL;

    AVPacket pkt;

    av_log_set_level(AV_LOG_DEBUG);    
	if (argc < 3) {     //argc是参数个数，argv是参数数组, vector(向量)
        av_log(NULL, AV_LOG_ERROR, "the arguments must be more than 3!\n");
    }

	src = argv[1];   //source(源)
	dst = argv[2];   //destination(目的地)
    
    //open the multimedia file
    if( (ret = avformat_open_input(&pFmtCtx, src, NULL, NULL)) < 0 ){
        av_log(NULL, AV_LOG_ERROR, " %s \n", av_err2str(ret));
        exit(-1);
    }

    //find the video stream from container
    if((idx = av_find_best_stream(pFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0)) < 0){ 
        av_log(pFmtCtx, AV_LOG_ERROR, "There is no audio stream!\n");    
        goto end;
    }

    //open the context of destination file 
	if ((oFmtCtx = avformat_alloc_context()) < 0) {    //allocate(分配)
        av_log(NULL, AV_LOG_ERROR, "No Memory!\n");
        goto end;
    }

	outFmt = av_guess_format(NULL, dst, NULL);    //destination(目的地)
	oFmtCtx->oformat = outFmt;     //output format(输出格式)

    //create a video stream  
    outStream = avformat_new_stream(oFmtCtx, NULL);

	//set the arguments of output Stream   //arguments(参数)
	inStream = pFmtCtx->streams[idx];   //idx是索引
	avcodec_parameters_copy(outStream->codecpar, inStream->codecpar);    //parameters(参数) 
	outStream->codecpar->codec_tag = 0;     //tag(标签)

    //binding
	ret = avio_open2(&oFmtCtx->pb, dst, AVIO_FLAG_WRITE, NULL, NULL);   //flag(标志)
    if(ret < 0){
        av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret)); 
        goto end;
    }

	//write the head file of multimedia to destination file    //destination(目的地)
    ret = avformat_write_header(oFmtCtx, NULL);
    if(ret < 0){                            
        av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
        goto end;
    }

    //read audio data from multimedia files to write into destination file
    while(av_read_frame(pFmtCtx, &pkt) >= 0){
		if (pkt.stream_index == idx) {   //index(索引)
            pkt.pts = av_rescale_q_rnd(pkt.pts, inStream->time_base, outStream->time_base, (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            //the dts data maybe different with the the pts if the file is video
            pkt.dts = av_rescale_q_rnd(pkt.dts, inStream->time_base, outStream->time_base, (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.duration = av_rescale_q(pkt.duration, inStream->time_base, outStream->time_base);
            pkt.stream_index = 0;
            pkt.pos = -1;
            av_interleaved_write_frame(oFmtCtx, &pkt);
			av_packet_unref(&pkt);  //unreferenced(未引用)
        }
    }

    //write end of file
	av_write_trailer(oFmtCtx);   //trailer(尾部)

    //free memory
end:
    if(pFmtCtx){       
        avformat_close_input(&pFmtCtx);
        pFmtCtx = NULL;
    }
    if(oFmtCtx->pb){
        avio_close(oFmtCtx->pb);
    }
    if(oFmtCtx){
        avformat_close_input(&oFmtCtx);
        oFmtCtx = NULL;
    }

    return 0;
}