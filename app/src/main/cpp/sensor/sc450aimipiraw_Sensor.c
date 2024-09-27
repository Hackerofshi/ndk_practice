// /*
//  * Copyright (C) 2015 MediaTek Inc.
//  *
//  * This program is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License version 2 as
//  * published by the Free Software Foundation.
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  * GNU General Public License for more details.
//  */

// /*****************************************************************************
//  *
//  * Filename:
//  * ---------
//  *	 SC450AImipi_Sensor.c
//  *
//  * Project:
//  * --------
//  *	 ALPS
//  *
//  * Description:
//  * ------------
//  *	 Source code of Sensor driver
//  *
//  *
//  *------------------------------------------------------------------------------
//  * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
//  *============================================================================
//  ****************************************************************************/

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
//#include <asm/system.h>
/*#include <linux/xlog.h>*/

//#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_camera_typedef.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"

#include "sc450aimipiraw_Sensor.h"


#define PFX "SC450AI_camera_sensor"
#define LOG_INF(format, args...)	printk(PFX "[%s] " format, __FUNCTION__, ##args)

static DEFINE_SPINLOCK(imgsensor_drv_lock);

#define OTP_SIZE 452
u8 OTPData[OTP_SIZE];
extern  int iReadData(unsigned int  ui4_offset, unsigned int  ui4_length, unsigned char * pinputdata);


static imgsensor_info_struct imgsensor_info = {
	.sensor_id = SC450AI_SENSOR_ID,

	.checksum_value =0xc15d2913,

	.pre = {
        .pclk = 88000000,//320000000,              //record different mode's pclk
		.linelength = 0x3a8,//5008,				//record different mode's linelength
        .framelength = 0x618,//3160,//2110,         //record different mode's framelength
		.startx = 0,					//record different mode's startx of grabwindow
		.starty = 0,					//record different mode's starty of grabwindow
		.grabwindow_width = 2688,//4208,//2104,		//record different mode's width of grabwindow
		.grabwindow_height = 1520,//3120,//1560,		//record different mode's height of grabwindow
		/*	 following for MIPIDataLowPwr2HighSpeedSettleDelayCount by different scenario	*/
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.max_framerate = 600,
		},
	.cap = {
        .pclk = 88000000,//320000000,              //record different mode's pclk
		.linelength = 0x3a8,//5008,				//record different mode's linelength
        .framelength = 0x618,//3160,//2110,         //record different mode's framelength
		.startx = 0,					//record different mode's startx of grabwindow
		.starty = 0,					//record different mode's starty of grabwindow
		.grabwindow_width = 2688,//4208,//2104,		//record different mode's width of grabwindow
		.grabwindow_height = 1520,//3120,//1560,		//record different mode's height of grabwindow
		/*	 following for MIPIDataLowPwr2HighSpeedSettleDelayCount by different scenario	*/
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.max_framerate = 600,
		},
	.cap1 = {
        .pclk = 88000000,//320000000,              //record different mode's pclk
		.linelength = 0x3a8,//5008,				//record different mode's linelength
        .framelength = 0x618,//3160,//2110,         //record different mode's framelength
		.startx = 0,					//record different mode's startx of grabwindow
		.starty = 0,					//record different mode's starty of grabwindow
		.grabwindow_width = 2688,//4208,//2104,		//record different mode's width of grabwindow
		.grabwindow_height = 1520,//3120,//1560,		//record different mode's height of grabwindow
		/*	 following for MIPIDataLowPwr2HighSpeedSettleDelayCount by different scenario	*/
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.max_framerate = 600,
		},
	.cap2 = {
        .pclk = 88000000,//320000000,              //record different mode's pclk
		.linelength = 0x3a8,//5008,				//record different mode's linelength
        .framelength = 0x618,//3160,//2110,         //record different mode's framelength
		.startx = 0,					//record different mode's startx of grabwindow
		.starty = 0,					//record different mode's starty of grabwindow
		.grabwindow_width = 2688,//4208,//2104,		//record different mode's width of grabwindow
		.grabwindow_height = 1520,//3120,//1560,		//record different mode's height of grabwindow
		/*	 following for MIPIDataLowPwr2HighSpeedSettleDelayCount by different scenario	*/
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.max_framerate = 600,
		},
	.normal_video = {
        .pclk = 88000000,//320000000,              //record different mode's pclk
		.linelength = 0x3a8,//5008,				//record different mode's linelength
        .framelength = 0x618,//3160,//2110,         //record different mode's framelength
		.startx = 0,					//record different mode's startx of grabwindow
		.starty = 0,					//record different mode's starty of grabwindow
		.grabwindow_width = 2688,//4208,//2104,		//record different mode's width of grabwindow
		.grabwindow_height = 1520,//3120,//1560,		//record different mode's height of grabwindow
		/*	 following for MIPIDataLowPwr2HighSpeedSettleDelayCount by different scenario	*/
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.max_framerate = 600,
		},
	.hs_video = {
        .pclk = 88000000,//320000000,              //record different mode's pclk
		.linelength = 0x3a8,//5008,				//record different mode's linelength
        .framelength = 0x618,//3160,//2110,         //record different mode's framelength
		.startx = 0,					//record different mode's startx of grabwindow
		.starty = 0,					//record different mode's starty of grabwindow
		.grabwindow_width = 2688,//4208,//2104,		//record different mode's width of grabwindow
		.grabwindow_height = 1520,//3120,//1560,		//record different mode's height of grabwindow
		/*	 following for MIPIDataLowPwr2HighSpeedSettleDelayCount by different scenario	*/
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.max_framerate = 600,
		},
	.slim_video = {
        .pclk = 88000000,//320000000,              //record different mode's pclk
		.linelength = 0x3a8,//5008,				//record different mode's linelength
        .framelength = 0x618,//3160,//2110,         //record different mode's framelength
		.startx = 0,					//record different mode's startx of grabwindow
		.starty = 0,					//record different mode's starty of grabwindow
		.grabwindow_width = 2688,//4208,//2104,		//record different mode's width of grabwindow
		.grabwindow_height = 1520,//3120,//1560,		//record different mode's height of grabwindow
		/*	 following for MIPIDataLowPwr2HighSpeedSettleDelayCount by different scenario	*/
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.max_framerate = 600,
		},
	.margin = 10,
	.min_shutter = 1,
	.max_frame_length = 0xffff,
	.ae_shut_delay_frame = 0,
	.ae_sensor_gain_delay_frame =1,
	.ae_ispGain_delay_frame = 2,
	.ihdr_support = 0,	  //1, support; 0,not support
	.ihdr_le_firstline = 0,  //1,le first ; 0, se first
	.sensor_mode_num = 5,	  //support sensor mode num

	.cap_delay_frame = 6,
	.pre_delay_frame = 4,
	.video_delay_frame = 3,
	.hs_video_delay_frame = 3,
	.slim_video_delay_frame = 3,

	.isp_driving_current = ISP_DRIVING_4MA,
	.sensor_interface_type = SENSOR_INTERFACE_TYPE_MIPI,
	.mipi_sensor_type = MIPI_OPHY_NCSI2, //0,MIPI_OPHY_NCSI2;  1,MIPI_OPHY_CSI2
	.mipi_settle_delay_mode = MIPI_SETTLEDELAY_AUTO, //0,MIPI_SETTLEDELAY_AUTO; 1,MIPI_SETTLEDELAY_MANNUAL
	.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_B,
	.mclk = 24,
	.mipi_lane_num = SENSOR_MIPI_4_LANE,
	//.i2c_addr_table = {0x30,0x32,0xff},
	.i2c_addr_table = {0x60,0x62,0x64,0x66,0xff},
};


static imgsensor_struct imgsensor = {
	.mirror = IMAGE_NORMAL,				//mirrorflip information
	.sensor_mode = IMGSENSOR_MODE_INIT, //IMGSENSOR_MODE enum value,record current sensor mode,such as: INIT, Preview, Capture, Video,High Speed Video, Slim Video
	.shutter = 0x3D0,					//current shutter
	.gain = 0x100,						//current gain
	.dummy_pixel = 0,					//current dummypixel
	.dummy_line = 0,					//current dummyline
	.current_fps = 0,  //full size current fps : 24fps for PIP, 30fps for Normal or ZSD
	.autoflicker_en = KAL_FALSE_C,  //auto flicker enable: KAL_FALSE_C for disable auto flicker, KAL_TRUE_C for enable auto flicker
	.test_pattern = KAL_FALSE_C,		//test pattern mode or not. KAL_FALSE_C for in test pattern mode, KAL_TRUE_C for normal output
	.current_scenario_id = MSDK_SCENARIO_ID_CAMERA_PREVIEW,//current scenario id
	.ihdr_mode = 0, //sensor need support LE, SE with HDR feature
	.i2c_write_id = 0x30,
};


/* Sensor output window information */
static struct SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[5] =
{{ 2688, 1520,	  0,	0, 2688, 1520, 2688,  1520, 0000, 0000, 2688,  1520,	  0,	0, 2688, 1520}, // Preview
 { 2688, 1520,	  0,	0, 2688, 1520, 2688,  1520, 0000, 0000, 2688,  1520,	  0,	0, 2688, 1520}, // capture
 { 2688, 1520,	  0,	0, 2688, 1520, 2688,  1520, 0000, 0000, 2688,  1520,	  0,	0, 2688, 1520}, // video
 { 2688, 1520,	  0,	0, 2688, 1520, 2688,  1520, 0000, 0000, 2688,  1520,	  0,	0, 2688, 1520}, //hight speed video
 { 2688, 1520,	  0,	0, 2688, 1520, 2688,  1520, 0000, 0000, 2688,  1520,	  0,	0, 2688, 1520}};// slim video

static struct SENSOR_VC_INFO_STRUCT SENSOR_VC_INFO[3]=
{/* Preview mode setting */
 {0x02, 0x0a,   0x00,   0x08, 0x40, 0x00,
  0x00, 0x2b, 0x0838, 0x0618, 0x01, 0x35, 0x0200, 0x0001,
  0x02, 0x00, 0x0000, 0x0000, 0x03, 0x00, 0x0000, 0x0000},
  /* Video mode setting */
 {0x02, 0x0a,   0x00,   0x08, 0x40, 0x00,
  0x00, 0x2b, 0x1070, 0x0c30, 0x01, 0x35, 0x0200, 0x0001,
  0x02, 0x00, 0x0000, 0x0000, 0x03, 0x00, 0x0000, 0x0000},
  /* Capture mode setting */
 {0x02, 0x0a,   0x00,   0x08, 0x40, 0x00,
  0x00, 0x2b, 0x1070, 0x0c30, 0x01, 0x35, 0x0200, 0x0001,
  0x02, 0x00, 0x0000, 0x0000, 0x03, 0x00, 0x0000, 0x0000}
};

typedef struct
{
    MUINT16 DarkLimit_H;
    MUINT16 DarkLimit_L;
    MUINT16 OverExp_Min_H;
    MUINT16 OverExp_Min_L;
    MUINT16 OverExp_Max_H;
    MUINT16 OverExp_Max_L;
}SENSOR_ATR_INFO, *pSENSOR_ATR_INFO;

// SENSOR_ATR_INFO sensorATR_Info[4]=
// {/* Strength Range Min */
//     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//     /* Strength Range Std */
//     {0x00, 0x32, 0x00, 0x3c, 0x03, 0xff},
//     /* Strength Range Max */
//     {0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xff},
//     /* Strength Range Custom */
//     {0x3F, 0xFF, 0x00, 0x0, 0x3F, 0xFF}
// };

#define MIPI_MaxGainIndex 159

static kal_uint16 sensorGainMapping[MIPI_MaxGainIndex][2] = {

	{64 , 1  },
	{65 , 8  },
	{66 , 13 },
	{67 , 23 },
	{68 , 27 },
	{69 , 36 },
	{70 , 41 },
	{71 , 49 },
	{72 , 53 },
	{73 , 61 },
	{74 , 69 },
	{75 , 73 },
	{76 , 80 },
	{77 , 88 },
	{78 , 91 },
	{79 , 98 },
	{80 , 101},
	{81 , 108},
	{82 , 111},
	{83 , 117},
	{84 , 120},
	{85 , 126},
	{86 , 132},
	{87 , 135},
	{88 , 140},
	{89 , 143},
	{90 , 148},
	{91 , 151},
	{92 , 156},
	{93 , 161},
	{94 , 163},
	{95 , 168},
	{96 , 170},
	{97 , 175},
	{98 , 177},
	{99 , 181},
	{100, 185},
	{101, 187},
	{102, 191},
	{103, 193},
	{104, 197},
	{105, 199},
	{106, 203},
	{107, 205},
	{108, 207},
	{109, 212},
	{110, 214},
	{111, 217},
	{112, 219},
	{113, 222},
	{114, 224},
	{115, 227},
	{116, 230},
	{117, 232},
	{118, 234},
	{119, 236},
	{120, 239},
	{122, 244},
	{123, 245},
	{124, 248},
	{125, 249},
	{126, 252},
	{127, 253},
	{128, 256},
	{129, 258},
	{130, 260},
	{131, 262},
	{132, 263},
	{133, 266},
	{134, 268},
	{136, 272},
	{138, 274},
	{139, 276},
	{140, 278},
	{141, 280},
	{143, 282},
	{144, 284},
	{145, 286},
	{147, 288},
	{148, 290},
	{149, 292},
	{150, 294},
	{152, 296},
	{153, 298},
	{155, 300},
	{156, 302},
	{157, 304},
	{159, 306},
	{161, 308},
	{162, 310},
	{164, 312},
	{166, 314},
	{167, 316},
	{169, 318},
	{171, 320},
	{172, 322},
	{174, 324},
	{176, 326},
	{178, 328},
	{180, 330},
	{182, 332},
	{184, 334},
	{186, 336},
	{188, 338},
	{191, 340},
	{193, 342},
	{195, 344},
	{197, 346},
	{200, 348},
	{202, 350},
	{205, 352},
	{207, 354},
	{210, 356},
	{212, 358},
	{216, 360},
	{218, 362},
	{221, 364},
	{225, 366},
	{228, 368},
	{231, 370},
	{234, 372},
	{237, 374},
	{241, 376},
	{244, 378},
	{248, 380},
	{252, 382},
	{256, 384},
	{260, 386},
	{264, 388},
	{269, 390},
	{273, 392},
	{278, 394},
	{282, 396},
	{287, 398},
	{292, 400},
	{298, 402},
	{303, 404},
	{309, 406},
	{315, 408},
	{321, 410},
	{328, 412},
	{334, 414},
	{341, 416},
	{349, 418},
	{356, 420},
	{364, 422},
	{372, 424},
	{381, 426},
	{390, 428},
	{399, 430},
	{410, 432},
	{420, 434},
	{431, 436},
	{443, 438},
	{455, 440},
	{468, 442},
	{482, 444},
	{497, 446},
	{512, 448}
};

#if 0
extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);
static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte=0;
      iReadReg((u16) addr ,(u8*)&get_byte, imgsensor.i2c_write_id);
      return get_byte;
}

#define write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para , 1,  imgsensor.i2c_write_id)
#endif
static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;
	char pu_send_cmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };

    iReadRegI2C(pu_send_cmd, 2, (u8*)&get_byte, 1, imgsensor.i2c_write_id);

    return get_byte;
}

static void write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
    char pu_send_cmd[3] = {(char)(addr >> 8), (char)(addr & 0xFF), (char)(para & 0xFF)};

    iWriteRegI2C(pu_send_cmd, 3, imgsensor.i2c_write_id);
}



// static kal_uint32 sc450ai_ATR(UINT16 DarkLimit, UINT16 OverExp)
// {

// #if 0
//     write_cmos_sensor(0x6e50,sensorATR_Info[DarkLimit].DarkLimit_H);
//     write_cmos_sensor(0x6e51,sensorATR_Info[DarkLimit].DarkLimit_L);
//     write_cmos_sensor(0x9340,sensorATR_Info[OverExp].OverExp_Min_H);
//     write_cmos_sensor(0x9341,sensorATR_Info[OverExp].OverExp_Min_L);
//     write_cmos_sensor(0x9342,sensorATR_Info[OverExp].OverExp_Max_H);
//     write_cmos_sensor(0x9343,sensorATR_Info[OverExp].OverExp_Max_L);
//     write_cmos_sensor(0x9706,0x10);
//     write_cmos_sensor(0x9707,0x03);
//     write_cmos_sensor(0x9708,0x03);
//     write_cmos_sensor(0x9e24,0x00);
//     write_cmos_sensor(0x9e25,0x8c);
//     write_cmos_sensor(0x9e26,0x00);
//     write_cmos_sensor(0x9e27,0x94);
//     write_cmos_sensor(0x9e28,0x00);
//     write_cmos_sensor(0x9e29,0x96);
//     LOG_INF("DarkLimit 0x6e50(0x%x), 0x6e51(0x%x)\n",sensorATR_Info[DarkLimit].DarkLimit_H,
//                                                      sensorATR_Info[DarkLimit].DarkLimit_L);
//     LOG_INF("OverExpMin 0x9340(0x%x), 0x9341(0x%x)\n",sensorATR_Info[OverExp].OverExp_Min_H,
//                                                      sensorATR_Info[OverExp].OverExp_Min_L);
//     LOG_INF("OverExpMin 0x9342(0x%x), 0x9343(0x%x)\n",sensorATR_Info[OverExp].OverExp_Max_H,
//                                                      sensorATR_Info[OverExp].OverExp_Max_L);
// #endif
//     return ERROR_NONE;
// }
static void set_dummy(void)
{
	LOG_INF("dummyline = %d, dummypixels = %d \n", imgsensor.dummy_line, imgsensor.dummy_pixel);


	   #if 0
    write_cmos_sensor(0x0104, 1);

	write_cmos_sensor(0x0340, imgsensor.frame_length >> 8);
	write_cmos_sensor(0x0341, imgsensor.frame_length & 0xFF);
	write_cmos_sensor(0x0342, imgsensor.line_length >> 8);
	write_cmos_sensor(0x0343, imgsensor.line_length & 0xFF);

	write_cmos_sensor(0x0104, 0);
	#endif

}	/*	set_dummy  */


static void set_max_framerate(UINT16 framerate,kal_bool_c min_framelength_en)
{
	//kal_int16 dummy_line;
	kal_uint32 frame_length = imgsensor.frame_length;
	//unsigned long flags;

	LOG_INF("framerate = %d, min framelength should enable = %d\n", framerate,min_framelength_en);

	frame_length = imgsensor.pclk / framerate * 10 / imgsensor.line_length;
	spin_lock(&imgsensor_drv_lock);
	if (frame_length >= imgsensor.min_frame_length)
		imgsensor.frame_length = frame_length;
	else
		imgsensor.frame_length = imgsensor.min_frame_length;
	imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
		//imgsensor.dummy_line = dummy_line;
	//imgsensor.frame_length = frame_length + imgsensor.dummy_line;
	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
	{
		imgsensor.frame_length = imgsensor_info.max_frame_length;
		imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
	}
	if (min_framelength_en)
		imgsensor.min_frame_length = imgsensor.frame_length;
	spin_unlock(&imgsensor_drv_lock);
	set_dummy();
}	/*	set_max_framerate  */


static void write_shutter(kal_uint16 shutter)
{
	kal_uint16 realtime_fps = 0;
	//kal_uint32 frame_length = 0;

	spin_lock(&imgsensor_drv_lock);
	if (shutter > imgsensor.min_frame_length - imgsensor_info.margin)
		imgsensor.frame_length = shutter + imgsensor_info.margin;
	else
		imgsensor.frame_length = imgsensor.min_frame_length;
	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		imgsensor.frame_length = imgsensor_info.max_frame_length;
	spin_unlock(&imgsensor_drv_lock);
	if (shutter < imgsensor_info.min_shutter) shutter = imgsensor_info.min_shutter;

	if (imgsensor.autoflicker_en) {
		realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
		if(realtime_fps >= 297 && realtime_fps <= 305)
			set_max_framerate(296,0);
		else if(realtime_fps >= 147 && realtime_fps <= 150)
			set_max_framerate(146,0);
		else {
		// Extend frame length
		//write_cmos_sensor(0x0104, 1);
		write_cmos_sensor(0x320e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x320f, imgsensor.frame_length & 0xFF);
		//write_cmos_sensor(0x0104, 0);
	    }
	} else {
		// Extend frame length
		//write_cmos_sensor(0x0104, 1);
		write_cmos_sensor(0x320e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x320f, imgsensor.frame_length & 0xFF);
		//write_cmos_sensor(0x0104, 0);
	}

	// Update Shutter
		//write_cmos_sensor(0x0104, 1);
		     write_cmos_sensor(0x3e00, (shutter >> 12) & 0xFF);
		     write_cmos_sensor(0x3e01, (shutter >> 4) & 0xFF);
             write_cmos_sensor(0x3e02, (shutter << 4) & 0xFF);
            // write_cmos_sensor(0x0202, (shutter >> 8) & 0xFF);
            // write_cmos_sensor(0x0203, shutter  & 0xFF);
            // write_cmos_sensor(0x0104, 0);

	printk("shutter =%d, framelength =%d\n", shutter,imgsensor.frame_length);

	//LOG_INF("frame_length = %d ", frame_length);

}	/*	write_shutter  */



/*************************************************************************
* FUNCTION
*	set_shutter
*
* DESCRIPTION
*	This function set e-shutter of sensor to change exposure time.
*
* PARAMETERS
*	iShutter : exposured lines
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void set_shutter(kal_uint16 shutter)
{
	unsigned long flags;
	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);

	write_shutter(shutter);
}	/*	set_shutter */



static kal_uint16 gain2reg(const kal_uint16 gain)
{
	kal_uint8 iI;

	for (iI = 0; iI < (MIPI_MaxGainIndex-1); iI++) {
			if(gain <= sensorGainMapping[iI][0]){
				break;
			}
		}

	return sensorGainMapping[iI][1];
}

/*************************************************************************
* FUNCTION
*	set_gain
*
* DESCRIPTION
*	This function is to set global gain to sensor.
*
* PARAMETERS
*	iGain : sensor global gain(base: 0x40)
*
* RETURNS
*	the actually gain set to sensor.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint16 set_gain(kal_uint16 gain)
{
    kal_uint16 reg_gain;

    if (gain < BASEGAIN || gain > 32 * BASEGAIN) {
        LOG_INF("Error gain setting");

        if (gain < BASEGAIN)
            gain = BASEGAIN;
        else if (gain > 32 * BASEGAIN)
            gain = 32 * BASEGAIN;
    }

    reg_gain = gain2reg(gain);
    spin_lock(&imgsensor_drv_lock);
    imgsensor.gain = reg_gain;
    spin_unlock(&imgsensor_drv_lock);
    LOG_INF("gain = %d , reg_gain = 0x%x\n ", gain, reg_gain);



   write_cmos_sensor(0x3e08, reg_gain & 0xFF);

   // write_cmos_sensor(0x0104, 1);
    /* Global analog Gain for Long expo*/
   // write_cmos_sensor(0x0204, (reg_gain>>8)& 0xFF);
  //  write_cmos_sensor(0x0205, reg_gain & 0xFF);
    /* Global analog Gain for Short expo*/
   // write_cmos_sensor(0x0216, (reg_gain>>8)& 0xFF);
   // write_cmos_sensor(0x0217, reg_gain & 0xFF);
    //write_cmos_sensor(0x0104, 0);

    return gain;
}	/*	set_gain  */

#if 0
static void ihdr_write_shutter_gain(kal_uint16 le, kal_uint16 se, kal_uint16 gain)
{
	//LOG_INF("le:0x%x, se:0x%x, gain:0x%x\n",le,se,gain);
    kal_uint16 realtime_fps = 0;
   // kal_uint32 frame_length = 0;
    kal_uint16 reg_gain;
    spin_lock(&imgsensor_drv_lock);
    if (le > imgsensor.min_frame_length - imgsensor_info.margin)
        imgsensor.frame_length = le + imgsensor_info.margin;
    else
        imgsensor.frame_length = imgsensor.min_frame_length;
    if (imgsensor.frame_length > imgsensor_info.max_frame_length)
        imgsensor.frame_length = imgsensor_info.max_frame_length;
    spin_unlock(&imgsensor_drv_lock);
    if (le < imgsensor_info.min_shutter) le = imgsensor_info.min_shutter;
    if (imgsensor.autoflicker_en) {
        realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
        if(realtime_fps >= 297 && realtime_fps <= 305)
            set_max_framerate(296,0);
        else if(realtime_fps >= 147 && realtime_fps <= 150)
            set_max_framerate(146,0);
        else {
        write_cmos_sensor(0x0104, 1);
        write_cmos_sensor(0x0340, imgsensor.frame_length >> 8);
        write_cmos_sensor(0x0341, imgsensor.frame_length & 0xFF);
        write_cmos_sensor(0x0104, 0);
        }
    } else {
        write_cmos_sensor(0x0104, 1);
        write_cmos_sensor(0x0340, imgsensor.frame_length >> 8);
        write_cmos_sensor(0x0341, imgsensor.frame_length & 0xFF);
        write_cmos_sensor(0x0104, 0);
    }
    write_cmos_sensor(0x0104, 1);
    /* Long exposure */
    write_cmos_sensor(0x0202, (le >> 8) & 0xFF);
    write_cmos_sensor(0x0203, le  & 0xFF);
    /* Short exposure */
    write_cmos_sensor(0x0224, (se >> 8) & 0xFF);
    write_cmos_sensor(0x0225, se  & 0xFF);
    reg_gain = gain2reg(gain);
    spin_lock(&imgsensor_drv_lock);
    imgsensor.gain = reg_gain;
    spin_unlock(&imgsensor_drv_lock);
    /* Global analog Gain for Long expo*/
    write_cmos_sensor(0x0204, (reg_gain>>8)& 0xFF);
    write_cmos_sensor(0x0205, reg_gain & 0xFF);
    /* Global analog Gain for Short expo*/
    write_cmos_sensor(0x0216, (reg_gain>>8)& 0xFF);
    write_cmos_sensor(0x0217, reg_gain & 0xFF);
    write_cmos_sensor(0x0104, 0);

}
#endif

#if 0
static void ihdr_write_shutter(kal_uint16 le, kal_uint16 se)
{
	//LOG_INF("le:0x%x, se:0x%x\n",le,se);
    kal_uint16 realtime_fps = 0;
   // kal_uint32 frame_length = 0;
   // kal_uint16 reg_gain;
    spin_lock(&imgsensor_drv_lock);
    if (le > imgsensor.min_frame_length - imgsensor_info.margin)
        imgsensor.frame_length = le + imgsensor_info.margin;
    else
        imgsensor.frame_length = imgsensor.min_frame_length;
    if (imgsensor.frame_length > imgsensor_info.max_frame_length)
        imgsensor.frame_length = imgsensor_info.max_frame_length;
    spin_unlock(&imgsensor_drv_lock);
    if (le < imgsensor_info.min_shutter) le = imgsensor_info.min_shutter;
    if (imgsensor.autoflicker_en) {
        realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
        if(realtime_fps >= 297 && realtime_fps <= 305)
            set_max_framerate(296,0);
        else if(realtime_fps >= 147 && realtime_fps <= 150)
            set_max_framerate(146,0);
        else {
        write_cmos_sensor(0x0104, 1);
        write_cmos_sensor(0x0340, imgsensor.frame_length >> 8);
        write_cmos_sensor(0x0341, imgsensor.frame_length & 0xFF);
        write_cmos_sensor(0x0104, 0);
        }
    } else {
        write_cmos_sensor(0x0104, 1);
        write_cmos_sensor(0x0340, imgsensor.frame_length >> 8);
        write_cmos_sensor(0x0341, imgsensor.frame_length & 0xFF);
        write_cmos_sensor(0x0104, 0);
    }
    write_cmos_sensor(0x0104, 1);
    /* Long exposure */
    write_cmos_sensor(0x0202, (le >> 8) & 0xFF);
    write_cmos_sensor(0x0203, le  & 0xFF);
    /* Short exposure */
    write_cmos_sensor(0x0224, (se >> 8) & 0xFF);
    write_cmos_sensor(0x0225, se  & 0xFF);

    write_cmos_sensor(0x0104, 0);

}
#endif


#if 0
static void set_mirror_flip(kal_uint8 image_mirror)
{
	LOG_INF("image_mirror = %d\n", image_mirror);

	kal_uint8 itemp;

	itemp=read_cmos_sensor(0x0101);
	itemp &= ~0x03;

	switch(image_mirror)
		{

		   case IMAGE_NORMAL:
		   	     write_cmos_sensor(0x0101, itemp);
			      break;

		   case IMAGE_V_MIRROR:
			     write_cmos_sensor(0x0101, itemp | 0x02);
			     break;

		   case IMAGE_H_MIRROR:
			     write_cmos_sensor(0x0101, itemp | 0x01);
			     break;

		   case IMAGE_HV_MIRROR:
			     write_cmos_sensor(0x0101, itemp | 0x03);
			     break;
		}
}
#endif
/*************************************************************************
* FUNCTION
*	night_mode
*
* DESCRIPTION
*	This function night mode of sensor.
*
* PARAMETERS
*	bEnable: KAL_TRUE_C -> enable night mode, otherwise, disable night mode
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void night_mode(kal_bool_c enable)
{
/*No Need to implement this function*/
}	/*	night_mode	*/

static void sensor_init(void)
{
	LOG_INF("E\n");
    //init setting



	write_cmos_sensor(0x0103, 0x01);
		write_cmos_sensor(0x0100, 0x00);
		write_cmos_sensor(0x36e9, 0x80);
		write_cmos_sensor(0x36f9, 0x80);
		write_cmos_sensor(0x301c, 0x78);
		write_cmos_sensor(0x301f, 0x02);
		write_cmos_sensor(0x302e, 0x00);
		write_cmos_sensor(0x3208, 0x0a);
		write_cmos_sensor(0x3209, 0x80);
		write_cmos_sensor(0x320a, 0x05);
		write_cmos_sensor(0x320b, 0xf0);
		write_cmos_sensor(0x320c, 0x03);
		write_cmos_sensor(0x320d, 0xa8);
		write_cmos_sensor(0x320e, 0x06);
		write_cmos_sensor(0x320f, 0x18);
		write_cmos_sensor(0x3214, 0x11);
		write_cmos_sensor(0x3215, 0x11);
		write_cmos_sensor(0x3220, 0x00);
		write_cmos_sensor(0x3223, 0xc0);
		write_cmos_sensor(0x3253, 0x10);
		write_cmos_sensor(0x325f, 0x44);
		write_cmos_sensor(0x3274, 0x09);
		write_cmos_sensor(0x3280, 0x01);
		write_cmos_sensor(0x3301, 0x08);
		write_cmos_sensor(0x3306, 0x24);
		write_cmos_sensor(0x3309, 0x60);
		write_cmos_sensor(0x330b, 0x64);
		write_cmos_sensor(0x330d, 0x30);
		write_cmos_sensor(0x3315, 0x00);
		write_cmos_sensor(0x331f, 0x59);
		write_cmos_sensor(0x335d, 0x60);
		write_cmos_sensor(0x3364, 0x56);
		write_cmos_sensor(0x338f, 0x80);
		write_cmos_sensor(0x3390, 0x08);
		write_cmos_sensor(0x3391, 0x18);
		write_cmos_sensor(0x3392, 0x38);
		write_cmos_sensor(0x3393, 0x0a);
		write_cmos_sensor(0x3394, 0x10);
		write_cmos_sensor(0x3395, 0x18);
		write_cmos_sensor(0x3396, 0x08);
		write_cmos_sensor(0x3397, 0x18);
		write_cmos_sensor(0x3398, 0x38);
		write_cmos_sensor(0x3399, 0x0f);
		write_cmos_sensor(0x339a, 0x12);
		write_cmos_sensor(0x339b, 0x14);
		write_cmos_sensor(0x339c, 0x18);
		write_cmos_sensor(0x33af, 0x18);
		write_cmos_sensor(0x360f, 0x13);
		write_cmos_sensor(0x3621, 0xec);
		write_cmos_sensor(0x3627, 0xa0);
		write_cmos_sensor(0x3630, 0x90);
		write_cmos_sensor(0x3633, 0x56);
		write_cmos_sensor(0x3637, 0x1d);
		write_cmos_sensor(0x3638, 0x0a);
		write_cmos_sensor(0x363c, 0x0f);
		write_cmos_sensor(0x363d, 0x0f);
		write_cmos_sensor(0x363e, 0x08);
		write_cmos_sensor(0x3670, 0x4a);
		write_cmos_sensor(0x3671, 0xe0);
		write_cmos_sensor(0x3672, 0xe0);
		write_cmos_sensor(0x3673, 0xe0);
		write_cmos_sensor(0x3674, 0xb0);
		write_cmos_sensor(0x3675, 0x88);
		write_cmos_sensor(0x3676, 0x8c);
		write_cmos_sensor(0x367a, 0x48);
		write_cmos_sensor(0x367b, 0x58);
		write_cmos_sensor(0x367c, 0x48);
		write_cmos_sensor(0x367d, 0x58);
		write_cmos_sensor(0x3690, 0x34);
		write_cmos_sensor(0x3691, 0x43);
		write_cmos_sensor(0x3692, 0x44);
		write_cmos_sensor(0x3699, 0x03);
		write_cmos_sensor(0x369a, 0x0f);
		write_cmos_sensor(0x369b, 0x1f);
		write_cmos_sensor(0x369c, 0x40);
		write_cmos_sensor(0x369d, 0x48);
		write_cmos_sensor(0x36a2, 0x48);
		write_cmos_sensor(0x36a3, 0x78);
		write_cmos_sensor(0x36b0, 0x54);
		write_cmos_sensor(0x36b1, 0x75);
		write_cmos_sensor(0x36b2, 0x35);
		write_cmos_sensor(0x36b3, 0x48);
		write_cmos_sensor(0x36b4, 0x78);
		write_cmos_sensor(0x36b7, 0xa0);
		write_cmos_sensor(0x36b8, 0xa0);
		write_cmos_sensor(0x36b9, 0x20);
		write_cmos_sensor(0x36bd, 0x40);
		write_cmos_sensor(0x36be, 0x48);
		write_cmos_sensor(0x36d0, 0x20);
		write_cmos_sensor(0x36e0, 0x08);
		write_cmos_sensor(0x36e1, 0x08);
		write_cmos_sensor(0x36e2, 0x12);
		write_cmos_sensor(0x36e3, 0x48);
		write_cmos_sensor(0x36e4, 0x78);
		write_cmos_sensor(0x36fa, 0x0d);
		write_cmos_sensor(0x36fb, 0xa4);
		write_cmos_sensor(0x36fc, 0x00);
		write_cmos_sensor(0x36fd, 0x24);
		write_cmos_sensor(0x3907, 0x00);
		write_cmos_sensor(0x3908, 0x41);
		write_cmos_sensor(0x391e, 0x01);
		write_cmos_sensor(0x391f, 0x11);
		write_cmos_sensor(0x3933, 0x82);
		write_cmos_sensor(0x3934, 0x0b);
		write_cmos_sensor(0x3935, 0x02);
		write_cmos_sensor(0x3936, 0x5e);
		write_cmos_sensor(0x3937, 0x76);
		write_cmos_sensor(0x3938, 0x78);
		write_cmos_sensor(0x3939, 0x00);
		write_cmos_sensor(0x393a, 0x28);
		write_cmos_sensor(0x393b, 0x00);
		write_cmos_sensor(0x393c, 0x1d);
		write_cmos_sensor(0x3e01, 0xc2);
		write_cmos_sensor(0x3e02, 0x60);
		write_cmos_sensor(0x3e03, 0x0b);
		write_cmos_sensor(0x3e08, 0x03);
		write_cmos_sensor(0x3e1b, 0x2a);
		write_cmos_sensor(0x440e, 0x02);
		write_cmos_sensor(0x4509, 0x20);
		write_cmos_sensor(0x4837, 0x16);
		write_cmos_sensor(0x5000, 0x0e);
		write_cmos_sensor(0x5001, 0x44);
		write_cmos_sensor(0x5780, 0x76);
		write_cmos_sensor(0x5784, 0x08);
		write_cmos_sensor(0x5785, 0x04);
		write_cmos_sensor(0x5787, 0x0a);
		write_cmos_sensor(0x5788, 0x0a);
		write_cmos_sensor(0x5789, 0x0a);
		write_cmos_sensor(0x578a, 0x0a);
		write_cmos_sensor(0x578b, 0x0a);
		write_cmos_sensor(0x578c, 0x0a);
		write_cmos_sensor(0x578d, 0x40);
		write_cmos_sensor(0x5790, 0x08);
		write_cmos_sensor(0x5791, 0x04);
		write_cmos_sensor(0x5792, 0x04);
		write_cmos_sensor(0x5793, 0x08);
		write_cmos_sensor(0x5794, 0x04);
		write_cmos_sensor(0x5795, 0x04);
		write_cmos_sensor(0x5799, 0x46);
		write_cmos_sensor(0x579a, 0x77);
		write_cmos_sensor(0x57a1, 0x04);
		write_cmos_sensor(0x57a8, 0xd0);
		write_cmos_sensor(0x57aa, 0x2a);
		write_cmos_sensor(0x57ab, 0x7f);
		write_cmos_sensor(0x57ac, 0x00);
		write_cmos_sensor(0x57ad, 0x00);
		write_cmos_sensor(0x59e0, 0xfe);
		write_cmos_sensor(0x59e1, 0x40);
		write_cmos_sensor(0x59e2, 0x3f);
		write_cmos_sensor(0x59e3, 0x38);
		write_cmos_sensor(0x59e4, 0x30);
		write_cmos_sensor(0x59e5, 0x3f);
		write_cmos_sensor(0x59e6, 0x38);
		write_cmos_sensor(0x59e7, 0x30);
		write_cmos_sensor(0x59e8, 0x3f);
		write_cmos_sensor(0x59e9, 0x3c);
		write_cmos_sensor(0x59ea, 0x38);
		write_cmos_sensor(0x59eb, 0x3f);
		write_cmos_sensor(0x59ec, 0x3c);
		write_cmos_sensor(0x59ed, 0x38);
		write_cmos_sensor(0x59ee, 0xfe);
		write_cmos_sensor(0x59ef, 0x40);
		write_cmos_sensor(0x59f4, 0x3f);
		write_cmos_sensor(0x59f5, 0x38);
		write_cmos_sensor(0x59f6, 0x30);
		write_cmos_sensor(0x59f7, 0x3f);
		write_cmos_sensor(0x59f8, 0x38);
		write_cmos_sensor(0x59f9, 0x30);
		write_cmos_sensor(0x59fa, 0x3f);
		write_cmos_sensor(0x59fb, 0x3c);
		write_cmos_sensor(0x59fc, 0x38);
		write_cmos_sensor(0x59fd, 0x3f);
		write_cmos_sensor(0x59fe, 0x3c);
		write_cmos_sensor(0x59ff, 0x38);
		write_cmos_sensor(0x36e9, 0x44);
		write_cmos_sensor(0x36f9, 0x20);
		// write_cmos_sensor(0x0100, 0x01);

    msleep(20);
}	/*	sensor_init  */


static void preview_setting_HDR(void)
{}	/*	preview_setting  */

static void capture_setting(kal_uint16 currefps)
{}

static void normal_video_setting(kal_uint16 currefps)
{}

static void fullsize_setting_HDR(kal_uint16 currefps)
{}

static void hs_video_setting(void)
{}

static void slim_video_setting(void)
{
	LOG_INF("E\n");
	hs_video_setting();
}
#if 0
static void vhdr_setting(void)
{
     	LOG_INF("E\n");
write_cmos_sensor(0x0114,0x03);
write_cmos_sensor(0x0220,0x01);
write_cmos_sensor(0x0221,0x22);
write_cmos_sensor(0x0222,0x10);
write_cmos_sensor(0x0340,0x06);
write_cmos_sensor(0x0341,0x68);
write_cmos_sensor(0x0342,0x13);
write_cmos_sensor(0x0343,0x90);
write_cmos_sensor(0x0344,0x00);
write_cmos_sensor(0x0345,0x00);
write_cmos_sensor(0x0346,0x01);
write_cmos_sensor(0x0347,0x78);
write_cmos_sensor(0x0348,0x10);
write_cmos_sensor(0x0349,0x6F);
write_cmos_sensor(0x034A,0x0A);
write_cmos_sensor(0x034B,0xB7);
write_cmos_sensor(0x0381,0x01);
write_cmos_sensor(0x0383,0x01);
write_cmos_sensor(0x0385,0x01);
write_cmos_sensor(0x0387,0x01);
write_cmos_sensor(0x0900,0x00);
write_cmos_sensor(0x0901,0x00);
write_cmos_sensor(0x0902,0x00);
write_cmos_sensor(0x3000,0x35);
write_cmos_sensor(0x3054,0x01);
write_cmos_sensor(0x305C,0x11);

write_cmos_sensor(0x0112,0x0A);
write_cmos_sensor(0x0113,0x0A);
write_cmos_sensor(0x034C,0x08);
write_cmos_sensor(0x034D,0x38);
write_cmos_sensor(0x034E,0x04);
write_cmos_sensor(0x034F,0xA0);
write_cmos_sensor(0x0401,0x00);
write_cmos_sensor(0x0404,0x00);
write_cmos_sensor(0x0405,0x10);
write_cmos_sensor(0x0408,0x00);
write_cmos_sensor(0x0409,0x00);
write_cmos_sensor(0x040A,0x00);
write_cmos_sensor(0x040B,0x00);
write_cmos_sensor(0x040C,0x08);
write_cmos_sensor(0x040D,0x38);
write_cmos_sensor(0x040E,0x04);
write_cmos_sensor(0x040F,0xA0);

write_cmos_sensor(0x0301,0x05);
write_cmos_sensor(0x0303,0x02);
write_cmos_sensor(0x0305,0x03);
write_cmos_sensor(0x0306,0x00);
write_cmos_sensor(0x0307,0x4D);
write_cmos_sensor(0x0309,0x0A);
write_cmos_sensor(0x030B,0x01);
write_cmos_sensor(0x0310,0x00);
write_cmos_sensor(0x0820,0x09);
write_cmos_sensor(0x0821,0xA0);
write_cmos_sensor(0x0822,0x00);
write_cmos_sensor(0x0823,0x00);
write_cmos_sensor(0x3A03,0x06);
write_cmos_sensor(0x3A04,0xE8);
write_cmos_sensor(0x3A05,0x01);
write_cmos_sensor(0x0B06,0x01);
write_cmos_sensor(0x30A2,0x00);
write_cmos_sensor(0x30B4,0x00);
write_cmos_sensor(0x3A02,0x06);
write_cmos_sensor(0x3013,0x01);
write_cmos_sensor(0x0202,0x06);
write_cmos_sensor(0x0203,0x5E);
write_cmos_sensor(0x0224,0x00);
write_cmos_sensor(0x0225,0xCB);
write_cmos_sensor(0x0204,0x00);
write_cmos_sensor(0x0205,0x00);
write_cmos_sensor(0x020E,0x01);
write_cmos_sensor(0x020F,0x00);
write_cmos_sensor(0x0210,0x01);
write_cmos_sensor(0x0211,0x00);
write_cmos_sensor(0x0212,0x01);
write_cmos_sensor(0x0213,0x00);
write_cmos_sensor(0x0214,0x01);
write_cmos_sensor(0x0215,0x00);
write_cmos_sensor(0x0216,0x00);
write_cmos_sensor(0x0217,0x00);
write_cmos_sensor(0x4170,0x00);
write_cmos_sensor(0x4171,0x10);
write_cmos_sensor(0x4176,0x00);
write_cmos_sensor(0x4177,0x3C);
write_cmos_sensor(0xAE20,0x04);
write_cmos_sensor(0xAE21,0x5C);

write_cmos_sensor(0x0138,0x01);
write_cmos_sensor(0x0100,0x01);

}
#endif
static kal_uint32 return_sensor_id(void)
{   
	kal_uint32 sensorid = (read_cmos_sensor(0x3107) << 8) | read_cmos_sensor(0x3108);
	//xc7022_bypass_on();
    return sensorid;
	//xc7022_bypass_off();
}

/*************************************************************************
* FUNCTION
*	get_imgsensor_id
*
* DESCRIPTION
*	This function get the sensor ID
*
* PARAMETERS
*	*sensorID : return the sensor ID
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/

static kal_uint32 get_imgsensor_id(UINT32 *sensor_id)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
	//sensor have two i2c address 0x6c 0x6d & 0x21 0x20, we should detect the module used i2c address
	printk("sc450ai lrz get_imgsensor_id\n");
	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
			msleep(10);   ///sc450ai id =0xbd2f;
			*sensor_id = return_sensor_id();
			if (*sensor_id == imgsensor_info.sensor_id) {
				printk("sc450ai lrz i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,*sensor_id);
				//iReadData(0x00,452,OTPData);
				return ERROR_NONE;
			}
			printk("sc450ai lrz Read sensor id fail, write id:0x%x id: 0x%x\n", imgsensor.i2c_write_id,*sensor_id);
			retry--;
		} while(retry > 0);
		i++;
		retry = 2;
	}
	if (*sensor_id != imgsensor_info.sensor_id) {
		// if Sensor ID is not correct, Must set *sensor_id to 0xFFFFFFFF
		*sensor_id = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	return ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*	open
*
* DESCRIPTION
*	This function initialize the registers of CMOS sensor
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 open(void)
{
	//const kal_uint8 i2c_addr[] = {IMGSENSOR_WRITE_ID_1, IMGSENSOR_WRITE_ID_2};
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
	kal_uint16 sensor_id = 0;
	LOG_INF("PLATFORM:MT6595,MIPI 2LANE\n");
	LOG_INF("preview 1280*960@30fps,864Mbps/lane; video 1280*960@30fps,864Mbps/lane; capture 5M@30fps,864Mbps/lane\n");

	//sensor have two i2c address 0x6c 0x6d & 0x21 0x20, we should detect the module used i2c address
	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
			msleep(10);
			sensor_id = return_sensor_id();
			if (sensor_id == imgsensor_info.sensor_id) {
				printk("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,sensor_id);
				break;
			}
			printk("Read sensor id fail, write id:0x%x id: 0x%x\n", imgsensor.i2c_write_id,sensor_id);
			retry--;
		} while(retry > 0);
		i++;
		if (sensor_id == imgsensor_info.sensor_id)
			break;
		retry = 2;
	}
	if (imgsensor_info.sensor_id != sensor_id)
		return ERROR_SENSOR_CONNECT_FAIL;

	/* initail sequence write in  */
	sensor_init();

	spin_lock(&imgsensor_drv_lock);

	imgsensor.autoflicker_en= KAL_FALSE_C;
	imgsensor.sensor_mode = IMGSENSOR_MODE_INIT;
	imgsensor.shutter = 0x3D0;
	imgsensor.gain = 0x100;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.dummy_pixel = 0;
	imgsensor.dummy_line = 0;
	imgsensor.ihdr_mode = 0;
	imgsensor.test_pattern = KAL_FALSE_C;
	imgsensor.current_fps = imgsensor_info.pre.max_framerate;
	spin_unlock(&imgsensor_drv_lock);
	LOG_INF("open call finished\n");

	return ERROR_NONE;
}	/*	open  */



/*************************************************************************
* FUNCTION
*	close
*
* DESCRIPTION
*
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 close(void)
{
	LOG_INF("E\n");

	/*No Need to implement this function*/

	return ERROR_NONE;
}	/*	close  */


/*************************************************************************
* FUNCTION
* preview
*
* DESCRIPTION
*	This function start the sensor preview.
*
* PARAMETERS
*	*image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_PREVIEW;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	//imgsensor.video_mode = KAL_FALSE_C;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.autoflicker_en = KAL_FALSE_C;
	spin_unlock(&imgsensor_drv_lock);
    if((imgsensor.ihdr_mode == 2) || (imgsensor.ihdr_mode == 9))
        preview_setting_HDR();
    else
	    //preview_setting();
	    capture_setting(imgsensor.current_fps);
	//hs_video_setting();

	return ERROR_NONE;
}	/*	preview   */

/*************************************************************************
* FUNCTION
*	capture
*
* DESCRIPTION
*	This function setup the CMOS sensor in capture MY_OUTPUT mode
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
						  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CAPTURE;
	if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {//PIP capture: 24fps for less than 13M, 20fps for 16M,15fps for 20M
		imgsensor.pclk = imgsensor_info.cap1.pclk;
		imgsensor.line_length = imgsensor_info.cap1.linelength;
		imgsensor.frame_length = imgsensor_info.cap1.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap1.framelength;
		imgsensor.autoflicker_en = KAL_FALSE_C;
	}  else if(imgsensor.current_fps == imgsensor_info.cap2.max_framerate){
		if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
			LOG_INF("Warning: current_fps %d fps is not support, so use cap1's setting: %d fps!\n",imgsensor.current_fps,imgsensor_info.cap1.max_framerate/10);
		imgsensor.pclk = imgsensor_info.cap2.pclk;
		imgsensor.line_length = imgsensor_info.cap2.linelength;
		imgsensor.frame_length = imgsensor_info.cap2.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap2.framelength;
		imgsensor.autoflicker_en = KAL_FALSE_C;
	}else {
		if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
			LOG_INF("Warning: current_fps %d fps is not support, so use cap1's setting: %d fps!\n",imgsensor.current_fps,imgsensor_info.cap1.max_framerate/10);
		imgsensor.pclk = imgsensor_info.cap.pclk;
		imgsensor.line_length = imgsensor_info.cap.linelength;
		imgsensor.frame_length = imgsensor_info.cap.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap.framelength;
		imgsensor.autoflicker_en = KAL_FALSE_C;
	}
	spin_unlock(&imgsensor_drv_lock);
    //
    if((imgsensor.ihdr_mode == 2) || (imgsensor.ihdr_mode == 9))
        fullsize_setting_HDR(imgsensor.current_fps);
    else
		sensor_init();
		msleep(10);
		capture_setting(imgsensor.current_fps);
		msleep(500);

	return ERROR_NONE;
}	/* capture() */
static kal_uint32 normal_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_VIDEO;
	imgsensor.pclk = imgsensor_info.normal_video.pclk;
	imgsensor.line_length = imgsensor_info.normal_video.linelength;
	imgsensor.frame_length = imgsensor_info.normal_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.normal_video.framelength;
	//imgsensor.current_fps = 300;
	imgsensor.autoflicker_en = KAL_FALSE_C;
	spin_unlock(&imgsensor_drv_lock);
    if(imgsensor.ihdr_mode == 2)
	    fullsize_setting_HDR(imgsensor.current_fps);
	else
        normal_video_setting(imgsensor.current_fps);

	return ERROR_NONE;
}	/*	normal_video   */

static kal_uint32 hs_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_HIGH_SPEED_VIDEO;
	imgsensor.pclk = imgsensor_info.hs_video.pclk;
	//imgsensor.video_mode = KAL_TRUE_C;
	imgsensor.line_length = imgsensor_info.hs_video.linelength;
	imgsensor.frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	//imgsensor.current_fps = 300;
	imgsensor.autoflicker_en = KAL_FALSE_C;
	spin_unlock(&imgsensor_drv_lock);
	hs_video_setting();

	return ERROR_NONE;
}	/*	hs_video   */

static kal_uint32 slim_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_SLIM_VIDEO;
	imgsensor.pclk = imgsensor_info.slim_video.pclk;
	//imgsensor.video_mode = KAL_TRUE_C;
	imgsensor.line_length = imgsensor_info.slim_video.linelength;
	imgsensor.frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	//imgsensor.current_fps = 300;
	imgsensor.autoflicker_en = KAL_FALSE_C;
	spin_unlock(&imgsensor_drv_lock);
	slim_video_setting();

	return ERROR_NONE;
}	/*	slim_video	 */



static kal_uint32 get_resolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *sensor_resolution)
{
	LOG_INF("E\n");
	sensor_resolution->SensorFullWidth = imgsensor_info.cap.grabwindow_width;
	sensor_resolution->SensorFullHeight = imgsensor_info.cap.grabwindow_height;

	sensor_resolution->SensorPreviewWidth = imgsensor_info.pre.grabwindow_width;
	sensor_resolution->SensorPreviewHeight = imgsensor_info.pre.grabwindow_height;

	sensor_resolution->SensorVideoWidth = imgsensor_info.normal_video.grabwindow_width;
	sensor_resolution->SensorVideoHeight = imgsensor_info.normal_video.grabwindow_height;


	sensor_resolution->SensorHighSpeedVideoWidth	 = imgsensor_info.hs_video.grabwindow_width;
	sensor_resolution->SensorHighSpeedVideoHeight	 = imgsensor_info.hs_video.grabwindow_height;

	sensor_resolution->SensorSlimVideoWidth	 = imgsensor_info.slim_video.grabwindow_width;
	sensor_resolution->SensorSlimVideoHeight	 = imgsensor_info.slim_video.grabwindow_height;
	return ERROR_NONE;
}	/*	get_resolution	*/

static kal_uint32 get_info(enum MSDK_SCENARIO_ID_ENUM scenario_id,
					  MSDK_SENSOR_INFO_STRUCT *sensor_info,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("scenario_id = %d\n", scenario_id);


	//sensor_info->SensorVideoFrameRate = imgsensor_info.normal_video.max_framerate/10; /* not use */
	//sensor_info->SensorStillCaptureFrameRate= imgsensor_info.cap.max_framerate/10; /* not use */
	//imgsensor_info->SensorWebCamCaptureFrameRate= imgsensor_info.v.max_framerate; /* not use */

	sensor_info->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW; /* not use */
	sensor_info->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW; // inverse with datasheet
	sensor_info->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorInterruptDelayLines = 4; /* not use */
	sensor_info->SensorResetActiveHigh = FALSE; /* not use */
	sensor_info->SensorResetDelayCount = 5; /* not use */

	sensor_info->SensroInterfaceType = imgsensor_info.sensor_interface_type;
	//sensor_info->MIPIsensorType = imgsensor_info.mipi_sensor_type;
	//sensor_info->SettleDelayMode = imgsensor_info.mipi_settle_delay_mode;
	sensor_info->SensorOutputDataFormat = imgsensor_info.sensor_output_dataformat;

	sensor_info->CaptureDelayFrame = imgsensor_info.cap_delay_frame;
	sensor_info->PreviewDelayFrame = imgsensor_info.pre_delay_frame;
	sensor_info->VideoDelayFrame = imgsensor_info.video_delay_frame;
	sensor_info->HighSpeedVideoDelayFrame = imgsensor_info.hs_video_delay_frame;
	sensor_info->SlimVideoDelayFrame = imgsensor_info.slim_video_delay_frame;

	sensor_info->SensorMasterClockSwitch = 0; /* not use */
	sensor_info->SensorDrivingCurrent = imgsensor_info.isp_driving_current;

	sensor_info->AEShutDelayFrame = imgsensor_info.ae_shut_delay_frame; 		 /* The frame of setting shutter default 0 for TG int */
	sensor_info->AESensorGainDelayFrame = imgsensor_info.ae_sensor_gain_delay_frame;	/* The frame of setting sensor gain */
	sensor_info->AEISPGainDelayFrame = imgsensor_info.ae_ispGain_delay_frame;
	sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
	sensor_info->IHDR_LE_FirstLine = imgsensor_info.ihdr_le_firstline;
	sensor_info->SensorModeNum = imgsensor_info.sensor_mode_num;

	sensor_info->SensorMIPILaneNumber = imgsensor_info.mipi_lane_num;
	sensor_info->SensorClockFreq = imgsensor_info.mclk;
	sensor_info->SensorClockDividCount = 3; /* not use */
	sensor_info->SensorClockRisingCount = 0;
	sensor_info->SensorClockFallingCount = 2; /* not use */
	sensor_info->SensorPixelClockCount = 3; /* not use */
	sensor_info->SensorDataLatchCount = 2; /* not use */

	sensor_info->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->SensorWidthSampling = 0;  // 0 is default 1x
	sensor_info->SensorHightSampling = 0;	// 0 is default 1x
	sensor_info->SensorPacketECCOrder = 1;

	switch (scenario_id) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;

			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			sensor_info->SensorGrabStartX = imgsensor_info.cap.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.cap.starty;

			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.cap.mipi_data_lp2hs_settle_dc;

			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:

			sensor_info->SensorGrabStartX = imgsensor_info.normal_video.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.normal_video.starty;

			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.normal_video.mipi_data_lp2hs_settle_dc;

			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			sensor_info->SensorGrabStartX = imgsensor_info.hs_video.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.hs_video.starty;

			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.hs_video.mipi_data_lp2hs_settle_dc;

			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			sensor_info->SensorGrabStartX = imgsensor_info.slim_video.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.slim_video.starty;

			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.slim_video.mipi_data_lp2hs_settle_dc;

			break;
		default:
			sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
			break;
	}

	return ERROR_NONE;
}	/*	get_info  */


static kal_uint32 control(enum MSDK_SCENARIO_ID_ENUM scenario_id, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("scenario_id = %d\n", scenario_id);
	spin_lock(&imgsensor_drv_lock);
	imgsensor.current_scenario_id = scenario_id;
	spin_unlock(&imgsensor_drv_lock);
	switch (scenario_id) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			preview(image_window, sensor_config_data);
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			capture(image_window, sensor_config_data);
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			normal_video(image_window, sensor_config_data);
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			hs_video(image_window, sensor_config_data);
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			slim_video(image_window, sensor_config_data);
			break;
		default:
			LOG_INF("Error ScenarioId setting");
			preview(image_window, sensor_config_data);
			return ERROR_INVALID_SCENARIO_ID;
	}
	return ERROR_NONE;
}	/* control() */



static kal_uint32 set_video_mode(UINT16 framerate)
{
	LOG_INF("framerate = %d\n ", framerate);
	// SetVideoMode Function should fix framerate
	if (framerate == 0)
		// Dynamic frame rate
		return ERROR_NONE;
	spin_lock(&imgsensor_drv_lock);
	if ((framerate == 300) && (imgsensor.autoflicker_en == KAL_TRUE_C))
		imgsensor.current_fps = 296;
	else if ((framerate == 150) && (imgsensor.autoflicker_en == KAL_TRUE_C))
		imgsensor.current_fps = 146;
	else
		imgsensor.current_fps = framerate;
	spin_unlock(&imgsensor_drv_lock);
	set_max_framerate(imgsensor.current_fps,1);

	return ERROR_NONE;
}

static kal_uint32 set_auto_flicker_mode(kal_bool_c enable, UINT16 framerate)
{
	LOG_INF("enable = %d, framerate = %d \n", enable, framerate);
	spin_lock(&imgsensor_drv_lock);
	if (enable) //enable auto flicker
		imgsensor.autoflicker_en = KAL_TRUE_C;
	else //Cancel Auto flick
		imgsensor.autoflicker_en = KAL_FALSE_C;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}


static kal_uint32 set_max_framerate_by_scenario( enum MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 framerate)
{
	kal_uint32 frame_length;

	LOG_INF("scenario_id = %d, framerate = %d\n", scenario_id, framerate);

	switch (scenario_id) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			frame_length = imgsensor_info.pre.pclk / framerate * 10 / imgsensor_info.pre.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.pre.framelength) ? (frame_length - imgsensor_info.pre.framelength) : 0;
			imgsensor.frame_length = imgsensor_info.pre.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			//set_dummy();
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			if(framerate == 0)
				return ERROR_NONE;
			frame_length = imgsensor_info.normal_video.pclk / framerate * 10 / imgsensor_info.normal_video.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.normal_video.framelength) ? (frame_length - imgsensor_info.normal_video.framelength) : 0;
			imgsensor.frame_length = imgsensor_info.normal_video.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			//set_dummy();
			break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        	  if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {
                frame_length = imgsensor_info.cap1.pclk / framerate * 10 / imgsensor_info.cap1.linelength;
                spin_lock(&imgsensor_drv_lock);
		            imgsensor.dummy_line = (frame_length > imgsensor_info.cap1.framelength) ? (frame_length - imgsensor_info.cap1.framelength) : 0;
		            imgsensor.frame_length = imgsensor_info.cap1.framelength + imgsensor.dummy_line;
		            imgsensor.min_frame_length = imgsensor.frame_length;
		            spin_unlock(&imgsensor_drv_lock);
            } else if (imgsensor.current_fps == imgsensor_info.cap2.max_framerate) {
                frame_length = imgsensor_info.cap2.pclk / framerate * 10 / imgsensor_info.cap2.linelength;
                spin_lock(&imgsensor_drv_lock);
		            imgsensor.dummy_line = (frame_length > imgsensor_info.cap2.framelength) ? (frame_length - imgsensor_info.cap2.framelength) : 0;
		            imgsensor.frame_length = imgsensor_info.cap2.framelength + imgsensor.dummy_line;
		            imgsensor.min_frame_length = imgsensor.frame_length;
		            spin_unlock(&imgsensor_drv_lock);
            } else {
        		    if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
                    LOG_INF("Warning: current_fps %d fps is not support, so use cap's setting: %d fps!\n",framerate,imgsensor_info.cap.max_framerate/10);
                frame_length = imgsensor_info.cap.pclk / framerate * 10 / imgsensor_info.cap.linelength;
                spin_lock(&imgsensor_drv_lock);
		            imgsensor.dummy_line = (frame_length > imgsensor_info.cap.framelength) ? (frame_length - imgsensor_info.cap.framelength) : 0;
		            imgsensor.frame_length = imgsensor_info.cap.framelength + imgsensor.dummy_line;
		            imgsensor.min_frame_length = imgsensor.frame_length;
		            spin_unlock(&imgsensor_drv_lock);
            }
            //set_dummy();
            break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			frame_length = imgsensor_info.hs_video.pclk / framerate * 10 / imgsensor_info.hs_video.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.hs_video.framelength) ? (frame_length - imgsensor_info.hs_video.framelength) : 0;
			imgsensor.frame_length = imgsensor_info.hs_video.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			//set_dummy();
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			frame_length = imgsensor_info.slim_video.pclk / framerate * 10 / imgsensor_info.slim_video.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.slim_video.framelength) ? (frame_length - imgsensor_info.slim_video.framelength): 0;
			imgsensor.frame_length = imgsensor_info.slim_video.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			//set_dummy();
			break;
		default:  //coding with  preview scenario by default
			frame_length = imgsensor_info.pre.pclk / framerate * 10 / imgsensor_info.pre.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.pre.framelength) ? (frame_length - imgsensor_info.pre.framelength) : 0;
			imgsensor.frame_length = imgsensor_info.pre.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			//set_dummy();
			LOG_INF("error scenario_id = %d, we use preview scenario \n", scenario_id);
			break;
	}
	return ERROR_NONE;
}


static kal_uint32 get_default_framerate_by_scenario(enum MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 *framerate)
{
	LOG_INF("scenario_id = %d\n", scenario_id);

	switch (scenario_id) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			*framerate = imgsensor_info.pre.max_framerate;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*framerate = imgsensor_info.normal_video.max_framerate;
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*framerate = imgsensor_info.cap.max_framerate;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*framerate = imgsensor_info.hs_video.max_framerate;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*framerate = imgsensor_info.slim_video.max_framerate;
			break;
		default:
			break;
	}

	return ERROR_NONE;
}

static kal_uint32 set_test_pattern_mode(kal_bool_c enable)
{
	LOG_INF("enable: %d\n", enable);

	if (enable) {
		// 0x5E00[8]: 1 enable,  0 disable
		// 0x5E00[1:0]; 00 Color bar, 01 Random Data, 10 Square, 11 BLACK
		//write_cmos_sensor(0x0601,0x0002);
	} else {
		// 0x5E00[8]: 1 enable,  0 disable
		// 0x5E00[1:0]; 00 Color bar, 01 Random Data, 10 Square, 11 BLACK
		//write_cmos_sensor(0x0601,0x0000);
	}
	spin_lock(&imgsensor_drv_lock);
	imgsensor.test_pattern = enable;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}

static kal_uint32 sc450ai_awb_gain(struct SET_SENSOR_AWB_GAIN *pSetSensorAWB)
{
  //  LOG_INF("sc450ai_awb_gain\n");
    //UINT32 rgain_32, grgain_32, gbgain_32, bgain_32;
#if 0
    grgain_32 = (pSetSensorAWB->ABS_GAIN_GR << 8) >> 9;
    rgain_32 = (pSetSensorAWB->ABS_GAIN_R << 8) >> 9;
    bgain_32 = (pSetSensorAWB->ABS_GAIN_B << 8) >> 9;
    gbgain_32 = (pSetSensorAWB->ABS_GAIN_GB << 8) >> 9;

    LOG_INF("[sc450ai_awb_gain] ABS_GAIN_GR:%d, grgain_32:%d\n", pSetSensorAWB->ABS_GAIN_GR, grgain_32);
    LOG_INF("[sc450ai_awb_gain] ABS_GAIN_R:%d, rgain_32:%d\n", pSetSensorAWB->ABS_GAIN_R, rgain_32);
    LOG_INF("[sc450ai_awb_gain] ABS_GAIN_B:%d, bgain_32:%d\n", pSetSensorAWB->ABS_GAIN_B, bgain_32);
    LOG_INF("[sc450ai_awb_gain] ABS_GAIN_GB:%d, gbgain_32:%d\n", pSetSensorAWB->ABS_GAIN_GB, gbgain_32);

    write_cmos_sensor(0x0b8e, (grgain_32 >> 8) & 0xFF);
    write_cmos_sensor(0x0b8f, grgain_32 & 0xFF);
    write_cmos_sensor(0x0b90, (rgain_32 >> 8) & 0xFF);
    write_cmos_sensor(0x0b91, rgain_32 & 0xFF);
    write_cmos_sensor(0x0b92, (bgain_32 >> 8) & 0xFF);
    write_cmos_sensor(0x0b93, bgain_32 & 0xFF);
    write_cmos_sensor(0x0b94, (gbgain_32 >> 8) & 0xFF);
    write_cmos_sensor(0x0b95, gbgain_32 & 0xFF);

	#endif
	
    return ERROR_NONE;
}
static kal_uint32 feature_control(MSDK_SENSOR_FEATURE_ENUM feature_id,
							 UINT8 *feature_para,UINT32 *feature_para_len)
{
	UINT16 *feature_return_para_16=(UINT16 *) feature_para;
	UINT16 *feature_data_16=(UINT16 *) feature_para;
	UINT32 *feature_return_para_32=(UINT32 *) feature_para;
	UINT32 *feature_data_32=(UINT32 *) feature_para;
    unsigned long long *feature_data=(unsigned long long *) feature_para;
//    unsigned long long *feature_return_para=(unsigned long long *) feature_para;

	struct SENSOR_WINSIZE_INFO_STRUCT *wininfo;
    struct SENSOR_VC_INFO_STRUCT *pvcinfo;
    struct SET_SENSOR_AWB_GAIN *pSetSensorAWB=(struct SET_SENSOR_AWB_GAIN *)feature_para;
	MSDK_SENSOR_REG_INFO_STRUCT *sensor_reg_data=(MSDK_SENSOR_REG_INFO_STRUCT *) feature_para;

	LOG_INF("feature_id = %d\n", feature_id);
	switch (feature_id) {
		case SENSOR_FEATURE_GET_PERIOD:
			*feature_return_para_16++ = imgsensor.line_length;
			*feature_return_para_16 = imgsensor.frame_length;
			*feature_para_len=4;
			break;
		case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
            LOG_INF("feature_Control imgsensor.pclk = %d,imgsensor.current_fps = %d\n", imgsensor.pclk,imgsensor.current_fps);
			*feature_return_para_32 = imgsensor.pclk;
			*feature_para_len=4;
			break;
		case SENSOR_FEATURE_SET_ESHUTTER:
            set_shutter(*feature_data);
			break;
		case SENSOR_FEATURE_SET_NIGHTMODE:
            night_mode((kal_bool_c) *feature_data);
			break;
		case SENSOR_FEATURE_SET_GAIN:
            set_gain((UINT16) *feature_data);
			break;
		case SENSOR_FEATURE_SET_FLASHLIGHT:
			break;
		case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
			break;
		case SENSOR_FEATURE_SET_REGISTER:
			write_cmos_sensor(sensor_reg_data->RegAddr, sensor_reg_data->RegData);
			break;
		case SENSOR_FEATURE_GET_REGISTER:
			sensor_reg_data->RegData = read_cmos_sensor(sensor_reg_data->RegAddr);
			break;
		case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
			// get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
			// if EEPROM does not exist in camera module.
			*feature_return_para_32=LENS_DRIVER_ID_DO_NOT_CARE;
			*feature_para_len=4;
			break;
		case SENSOR_FEATURE_SET_VIDEO_MODE:
            set_video_mode(*feature_data);
			break;
		case SENSOR_FEATURE_CHECK_SENSOR_ID:
			get_imgsensor_id(feature_return_para_32);
			break;
		case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
			set_auto_flicker_mode((kal_bool_c)*feature_data_16,*(feature_data_16+1));
			break;
		case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
            set_max_framerate_by_scenario((enum MSDK_SCENARIO_ID_ENUM)*feature_data, *(feature_data+1));
			break;
		case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
            get_default_framerate_by_scenario((enum MSDK_SCENARIO_ID_ENUM)*(feature_data), (MUINT32 *)(uintptr_t)(*(feature_data+1)));
			break;
		case SENSOR_FEATURE_SET_TEST_PATTERN:
            set_test_pattern_mode((kal_bool_c)*feature_data);
			break;
		case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE: //for factory mode auto testing
			*feature_return_para_32 = imgsensor_info.checksum_value;
			*feature_para_len=4;
			break;
		case SENSOR_FEATURE_SET_FRAMERATE:
            LOG_INF("current fps :%d\n", (UINT32)*feature_data);
			spin_lock(&imgsensor_drv_lock);
            imgsensor.current_fps = *feature_data;
			spin_unlock(&imgsensor_drv_lock);
			break;
		case SENSOR_FEATURE_SET_HDR:
            LOG_INF("ihdr enable :%d\n", (kal_bool_c)*feature_data);
			spin_lock(&imgsensor_drv_lock);
			imgsensor.ihdr_mode = *feature_data;
			spin_unlock(&imgsensor_drv_lock);
			break;
		case SENSOR_FEATURE_GET_CROP_INFO:
            LOG_INF("SENSOR_FEATURE_GET_CROP_INFO scenarioId:%d\n", (UINT32)*feature_data);
            wininfo = (struct SENSOR_WINSIZE_INFO_STRUCT *)(uintptr_t)(*(feature_data+1));

			switch (*feature_data_32) {
				case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
					memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[1],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
					break;
				case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
					memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[2],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
					break;
				case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
					memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[3],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
					break;
				case MSDK_SCENARIO_ID_SLIM_VIDEO:
					memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[4],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
					break;
				case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
				default:
					memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[0],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
					break;
			}
			break;
		case SENSOR_FEATURE_SET_IHDR_SHUTTER_GAIN:
            LOG_INF("SENSOR_SET_SENSOR_IHDR LE=%d, SE=%d, Gain=%d\n",(UINT16)*feature_data,(UINT16)*(feature_data+1),(UINT16)*(feature_data+2));
            //ihdr_write_shutter_gain((UINT16)*feature_data,(UINT16)*(feature_data+1),(UINT16)*(feature_data+2));
			break;
        case SENSOR_FEATURE_GET_VC_INFO:
            LOG_INF("SENSOR_FEATURE_GET_VC_INFO %d\n", (UINT16)*feature_data);
            pvcinfo = (struct SENSOR_VC_INFO_STRUCT *)(uintptr_t)(*(feature_data+1));
            switch (*feature_data_32) {
            case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
                memcpy((void *)pvcinfo,(void *)&SENSOR_VC_INFO[1],sizeof(struct SENSOR_VC_INFO_STRUCT));
                break;
            case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
                memcpy((void *)pvcinfo,(void *)&SENSOR_VC_INFO[2],sizeof(struct SENSOR_VC_INFO_STRUCT));
                break;
            case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
            default:
                memcpy((void *)pvcinfo,(void *)&SENSOR_VC_INFO[0],sizeof(struct SENSOR_VC_INFO_STRUCT));
                break;
            }
            break;
        case SENSOR_FEATURE_SET_AWB_GAIN:
            sc450ai_awb_gain(pSetSensorAWB);
            break;
        case SENSOR_FEATURE_SET_HDR_SHUTTER:
            LOG_INF("SENSOR_FEATURE_SET_HDR_SHUTTER LE=%d, SE=%d\n",(UINT16)*feature_data,(UINT16)*(feature_data+1));
           // ihdr_write_shutter((UINT16)*feature_data,(UINT16)*(feature_data+1));
            break;
		//danny add 
		case SENSOR_FEATURE_GET_PIXEL_RATE:
			switch (*feature_data) {
				case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
				*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				(imgsensor_info.cap.pclk /
				(imgsensor_info.cap.linelength - 80))*
				imgsensor_info.cap.grabwindow_width;
				break;

				case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
				*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				(imgsensor_info.normal_video.pclk /
				(imgsensor_info.normal_video.linelength - 80))*
				imgsensor_info.normal_video.grabwindow_width;
				break;

				case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
				*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				(imgsensor_info.hs_video.pclk /
				(imgsensor_info.hs_video.linelength - 80))*
				imgsensor_info.hs_video.grabwindow_width;
				break;

				case MSDK_SCENARIO_ID_SLIM_VIDEO:
				*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				(imgsensor_info.slim_video.pclk /
				(imgsensor_info.slim_video.linelength - 80))*
				imgsensor_info.slim_video.grabwindow_width;
				break;

				case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
				default:
				*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				(imgsensor_info.pre.pclk /
				(imgsensor_info.pre.linelength - 80))*
				imgsensor_info.pre.grabwindow_width;
				break;
			}
		break;	
        default:
            break;
	}

	return ERROR_NONE;
}	/*	feature_control()  */

static struct  SENSOR_FUNCTION_STRUCT sensor_func = {
	open,
	get_info,
	get_resolution,
	feature_control,
	control,
	close
};

//kin0603
UINT32 SC450AI_MIPI_RAW_SensorInit(struct SENSOR_FUNCTION_STRUCT  **pfFunc)
//UINT32 SC450AI_MIPI_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/* To Do : Check Sensor status here */
	printk("SC450AI_MIPI_RAW_SensorInit\n");
	if (pfFunc!=NULL)
		*pfFunc=&sensor_func;
	return ERROR_NONE;
}	/*	OV5693_MIPI_RAW_SensorInit	*/
