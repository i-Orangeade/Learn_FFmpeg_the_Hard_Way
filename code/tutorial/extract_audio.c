/*
 * copyright (c) 2024 Jack Lau
 * 
 * This file is a tutorial about extrating audio stream from a container into a new container through ffmpeg API
 *                               //提取                         //容器              //新容器
 * FFmpeg version 5.0.3 
 * Tested on Ubuntu 22.04, compiled with GCC 11.4.0
 */                         //编写 

#include <libavformat/avformat.h> 
#include <libavutil/avutil.h>   //

int main(int argc, char *argv[])
{
    int ret = -1;
    int idx = -1;
	//deal with arguments   //arguments(参数)
    char *src;
    char *dst;

    AVFormatContext *pFmtCtx = NULL; //point(指针)
	AVFormatContext* oFmtCtx = NULL;  //output(输出)

	const AVOutputFormat* outFmt = NULL;   //const(常量)
    AVStream *inStream = NULL;
	AVStream* outStream = NULL;   

    AVPacket pkt;

	av_log_set_level(AV_LOG_DEBUG);   //set(设置)  //level(级别)
    if(argc < 3){
		av_log(NULL, AV_LOG_ERROR, "the arguments must be more than 3!\n");   //arguments(参数)
    }

	src = argv[1];   //source(源)
	dst = argv[2];   //destination(目的)
    
    //open the multimedia file    //mult(多重)
    if( (ret = avformat_open_input(&pFmtCtx, src, NULL, NULL)) < 0 ){ 
        av_log(NULL, AV_LOG_ERROR, " %s \n", av_err2str(ret));
        exit(-1);
    }

	//find the audio stream from container    //container(容器)
	if ((idx = av_find_best_stream(pFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0)) < 0) {  //index(索引),ponit(指针),audio(音频)
        av_log(pFmtCtx, AV_LOG_ERROR, "There is no audio stream!\n");
        goto end;
    }

	//open the context of destination file   //destination(目的)
	if ((oFmtCtx = avformat_alloc_context()) < 0) {    //allocate(分配)
		av_log(NULL, AV_LOG_ERROR, "No Memory!\n");    //memory(内存)
        goto end;
    }

	outFmt = av_guess_format(NULL, dst, NULL);    //destination(目的)
    oFmtCtx->oformat = outFmt; 

    //create a audio stream   
    outStream = avformat_new_stream(oFmtCtx, NULL);

    //set the arguments of output Stream 
	inStream = pFmtCtx->streams[idx];  //input(输入)
    avcodec_parameters_copy(outStream->codecpar, inStream->codecpar); 
	outStream->codecpar->codec_tag = 0;   //tag(标签)

    //binding
	ret = avio_open2(&oFmtCtx->pb, dst, AVIO_FLAG_WRITE, NULL, NULL);   //pb(点播)
    if(ret < 0){
		av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));  //string(字符串)
        goto end;
    }

	//write the head file of multimedia to destination file   //destination(目的)
    ret = avformat_write_header(oFmtCtx, NULL);
    if(ret < 0){
        av_log(oFmtCtx, AV_LOG_ERROR, "%s", av_err2str(ret));
        goto end;
    }

    //read audio data from multimedia files to write into destination file  
	while (av_read_frame(pFmtCtx, &pkt) >= 0) {    //frame(帧)
		if (pkt.stream_index == idx) {   //index(索引)
            pkt.pts = av_rescale_q_rnd(pkt.pts, inStream->time_base, outStream->time_base, (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			//pts(时间戳),presentation(显示)，time(时间)，stamp(戳)  //time_base(时间基准)，round(圆),near(近),inf(无穷),pass(通过),minmax(最小最大)
			//the dts data is same as the the pts if the file is audio   //destination(目的)
			pkt.dts = pkt.pts;    //decoding(解码),time(时间),stamp(戳)
			pkt.duration = av_rescale_q(pkt.duration, inStream->time_base, outStream->time_base);
			//packet(包),duration(持续时间),rescale(重新缩放)
			pkt.stream_index = 0;  //index(索引)
			pkt.pos = -1;   //position(位置)  
			av_interleaved_write_frame(oFmtCtx, &pkt);  //interleaved(交叉),write(写)
			av_packet_unref(&pkt);  //packet(包),unreferenced(未引用)
        }
    }

    //write end of file
	av_write_trailer(oFmtCtx);   //trailer(拖尾部分)

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
		oFmtCtx = NULL;    //output(输出)
    }

    return 0;
}