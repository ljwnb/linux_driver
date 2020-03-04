#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "jpeglib.h" ///home/gec/arm-libs/include 
#include "disp_manager.h"



/*
	功能：显示一行的jpeg数据 
	参数： x0 表示起点的行坐标
			y0 表示起点的 列坐标 
			wigth 表示一行有多少个像素点 
			depth 表示每个像素点有几字节
			buffer 表示 这一行数据 
				格式由两种 rgb 
						   argb 
				由depth 
*/
static int  show_jpeg_line(int x0,int y0,int wigth,int depth,char* buffer)
{
	int i,color;
	char *p = buffer;
	char a,r,g,b;
    PT_DispOpr pt_DispDev = GetDefaultDispDev();//获取显示设备
    if (pt_DispDev == NULL)
    {
        printf("未选择显示设备！\n");
        return -1;
    }
    
	if(depth == 3)
	{
		for(i=y0;i<y0+wigth;i++)
		{
			a = 0x00;
			r = *p++;
			g = *p++;
			b = *p++;
	
			color = a<<24|r<<16|g<<8|b;
            pt_DispDev->ShowPixel(x0,i,color);
		}
	}
	else if(depth == 4)
	{
		for(i=y0;i<y0+wigth;i++)
		{
			a = *p++;
			r = *p++;
			g = *p++;
			b = *p++;
			color = a<<24|r<<16|g<<8|b;
            pt_DispDev->ShowPixel(x0,i,color);
			//draw_point(x0,i,color);
		}
	}
	return 0;
}


int show_jpeg(char *pathname)
{
        PT_DispOpr pt_DispDev = GetDefaultDispDev();
	//step 1：分配并初始化一个jpeg的解压对象 
		struct jpeg_decompress_struct dinfo; //定义一个jpeg解压对象 
		struct jpeg_error_mgr jerr; //出错信息结构体 
		
		dinfo.err = jpeg_std_error(&jerr); //将出错结构体与 解压对象关联 
		
		jpeg_create_decompress(&dinfo);//初始化解压对象 
	
	//step 2:打开要解压的图像文件  需要使用标准IO 因为jpeglib是使用的C标准实现的 
		FILE* J_fp = fopen(pathname,"r");
		if(J_fp == NULL)
		{
			perror("fopen jpeg error");
			return -1;
		}
	
	//step 3: 指定jpeg 解压对象的输入文件 
		jpeg_stdio_src(&dinfo,J_fp);
		//or 指定解压对象的输入来自于 pbuf指向的内存空间 即将文件中的内容读取出来 然后解压内容 中间只是缺省了一个fread 
			//jpeg_mem_src(&dinfo,pbuf);
		
	//step 4:获取将要解压的图像信息  包括 文件的分辨率 gps信息 压缩比例 
		jpeg_read_header(&dinfo,TRUE);
		
	//step 5：设置jpeg解压对象dinfo中的参数 一般使用默认参数 即不设置 
	
	//step 6: 调用 jpeg_start_decompress()启动解压过程 
		jpeg_start_decompress(&dinfo);
	/*
		调用完以上这句话后 dinfo 结构体将会被填充完整
			其中比较有用的将会是以下几个字段 
			output_width 解压后图像的宽度 
			output_height 解压后图像的高度 
			out_color_components 加压后图像的分量数 即每个像素点所占的字节数 
				3 rgb 4 argb 
			output_scanline 表示已经扫描的行数
	*/
	
	//step 7:在启动解压后 往往会按照行 进行解压 扫描 所以需要为每一行分配空间 
			//一行所占的字节数为 output_width * out_color_components 
	/*
		注意：对扫描线的读取是按照从上倒下的顺序进行的 也就是说 图像的最上方的扫描线是最先被扫描到的 
		接着扫描第二行 。。。。 
		每一行都是从左往右扫描
	*/
        if (dinfo.output_width > pt_DispDev->Xres || dinfo.output_height > pt_DispDev->Yres)
        {
            printf("该文件大小不支持！\n");
            fclose(J_fp);
            return -1;
        }
        if (dinfo.output_width < pt_DispDev->Xres || dinfo.output_height < pt_DispDev->Yres)//如果图片没有铺满屏幕 清屏
        {
            pt_DispDev->CleanScreen(COLOR_BACKGROUND);
        }
        
		unsigned char* buffer = (unsigned char*)malloc(dinfo.output_width*dinfo.out_color_components);
		
		int x,y;
		int x0 = (480-dinfo.output_height)/2;
		int y0 = (800-dinfo.output_width)/2;
		while(dinfo.output_scanline < dinfo.output_height)
		{
			x = dinfo.output_scanline + x0; //表示当前读取到的行号 
			y = y0;
			jpeg_read_scanlines(&dinfo,//解压对象 
								&buffer,//保存解压后数据的缓冲区首地址
								1);//解压多少行 
			//解压后读取到的数据 就是 argb or rgb 由out_color_components来决定 
			
			//处理显示一行的数据 
			//怎么处理一行的数据？
				
				/*
					需要知道 一行的数据 读取到数据的首地址 
					每一个像素点的 字节数 
					一行有多少个像素点 
					起点的坐标 
					类比画块函数 只不过现在画一行而已 
				
				*/
				show_jpeg_line(x,y,dinfo.output_width,dinfo.out_color_components,buffer);
			
		}
		
		
		
	//step 8：释放缓冲区 并 调用 jpeg_finish_decompress()完成加压过程
	
		free(buffer);
		jpeg_finish_decompress(&dinfo);
	
	//step 9: 释放解压对象 并关闭文件
		jpeg_destroy_decompress(&dinfo);
	
		fclose(J_fp);
}

