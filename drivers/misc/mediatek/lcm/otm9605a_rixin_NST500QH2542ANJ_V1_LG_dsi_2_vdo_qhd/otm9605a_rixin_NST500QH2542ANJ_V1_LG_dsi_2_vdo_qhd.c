
#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>

#define Lcd_Log printf
#else
#include <linux/string.h>
#include <linux/kernel.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>

#define Lcd_Log printk
#endif

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(540)
#define FRAME_HEIGHT 										(960)

#define REGFLAG_DELAY             							0XFFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER

#define LCM_ID	0x9605

#define LCM_DSI_CMD_MODE									0
#define GPIO_LCD_ID_PIN 97

#ifndef TRUE
#define   TRUE     1
#endif

#ifndef FALSE
#define   FALSE    0
#endif

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {

            /*
            Note :

            Data ID will depends on the following rule.

            	count of parameters > 1	=> Data ID = 0x39
            	count of parameters = 1	=> Data ID = 0x15
            	count of parameters = 0	=> Data ID = 0x05

            Structure Format :

            {DCS command, count of parameters, {parameter list}}
            {REGFLAG_DELAY, milliseconds of time, {}},

            ...

            Setting ending by predefined flag

            {REGFLAG_END_OF_TABLE, 0x00, {}}
            */
{0x00,1,{0x00}}, 
{0xFF,3,{0x96,0x05,0x01}}, 
 
{0x00,1,{0x80}}, 
{0xFF,2,{0x96,0x05}}, 
 
 
{0x00,1,{0x92}}, // mipi 2 lane
{0xFF,2,{0x10,0x02}},
 
////auo request
{0x00,1,{0xB4}},
{0xC0,1,{0x11}},//0x11=1+2 dot  // 0x55 column
 
{0x00,1,{0x80}},
{0xC1,2,{0x36,0x77}}, //65Hz
{0x00,1,{0x89}},
{0xC0,1,{0x01}},// TCON OSC turbo mode
 
{0x00,1,{0xA0}},
{0xC1,1,{0x00}},
 
//Address Shift//Power Control Setting 2 for Normal Mode
{0x00,1,{0x80}},
{0xC5,4,{0x08,0x00,0xa0,0x11}},
 
//Address Shift//Power Control Setting 2 for Normal Mode
{0x00,1,{0x90}},
{0xC5,3,{0xD6,0x57,0x01}},//VGH VHL
 
{0x00,1,{0xB0}},
{0xC5,2,{0x05,0xac}},//VDD18=1.9V
 
{0x00,1,{0x00}},
{0xD8,2,{0xB7,0xB7}},//gvdd=4.4V 67
 
{0x00,1,{0x00}},
{0xD9,1,{0x4B}},//VCOM 6e
 
{0x00,1,{0xa6}},
{0xc1,1,{0x01}},
 
{0x00,1,{0xa2}},
{0xc0,3,{0x0c,0x05,0x02}},
 
//GOA
{0x00,1,{0x80}},
{0xCB,10,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
 
{0x00,1,{0x90}},
{0xCB,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
 
{0x00,1,{0xA0}},
{0xCB,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
 
{0x00,1,{0xB0}},
{0xCB,10,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
 
{0x00,1,{0xC0}},
{0xCB,15,{0x04,0x04,0x04,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x04,0x04,0x08}},
 
{0x00,1,{0xD0}},
{0xCB,15,{0x08,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x08,0x04,0x08,0x04,0x08,0x04}},
 
{0x00,1,{0xE0}},
{0xCB,10,{0x08,0x04,0x04,0x04,0x08,0x08,0x00,0x00,0x00,0x00}},
 
{0x00,1,{0xF0}},
{0xCB,10,{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},
 
 
{0x00,1,{0x80}},
{0xCC,10,{0x26,0x25,0x21,0x22,0x00,0x0F,0x00,0x0D,0x00,0x0B}},
 
{0x00,1,{0x90}},
{0xCC,15,{0x00,0x09,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x25,0x21,0x22,0x00}},
 
{0x00,1,{0xA0}},
{0xCC,15,{0x10,0x00,0x0E,0x00,0x0C,0x00,0x0A,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00}},
 
 
{0x00,1,{0x80}},
{0xCE,12,{0x8B,0x03,0x06,0x8A,0x03,0x06,0x89,0x03,0x06,0x88,0x03,0x06}},
{0x00,1,{0x90}},
{0xCE,14,{0xF0,0x00,0x00,0xF0,0x00,0x00,0xF0,0x00,0x00,0xF0,0x00,0x00,0x00,0x00}},
{0x00,1,{0xA0}},
{0xCE,14,{0x38,0x03,0x03,0xC0,0x00,0x06,0x00,0x38,0x02,0x03,0xC1,0x00,0x06,0x00}},
{0x00,1,{0xB0}},
{0xCE,14,{0x38,0x01,0x03,0xC2,0x00,0x06,0x00,0x38,0x00,0x03,0xC3,0x00,0x06,0x00}},
{0x00,1,{0xC0}},
{0xCE,14,{0x38,0x07,0x03,0xBC,0x00,0x06,0x00,0x38,0x06,0x03,0xBD,0x00,0x06,0x00}},
{0x00,1,{0xD0}},
{0xCE,14,{0x38,0x05,0x03,0xBE,0x00,0x06,0x00,0x38,0x04,0x03,0xBF,0x00,0x06,0x00}},
 
 
{0x00,1,{0x80}},
{0xCF,14,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},
 
{0x00,1,{0x90}},
{0xCF,14,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},
 
{0x00,1,{0xa0}},
{0xCF,14,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},
 
{0x00,1,{0xb0}},
{0xCF,14,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},
 
{0x00,1,{0xC0}},
{0xCF,10,{0x02,0x02,0x20,0x20,0x00,0x00,0x01,0x00,0x00,0x02}},
 
 
{0x00,1,{0x80}},
{0xC4,1,{0x9c}},
 
{0x00,1,{0x87}},
{0xC4,1,{0x40}},
{0x00,1,{0xA6}},//zigzag off
{0xB3,1,{0x27}},
 
{0x00,1,{0xA0}},//panel mode
{0xB3,1,{0x10}},
 
 
//GAMMA 2.2
{0x00,1,{0x00}},
{0xE1,16,{0x02,0x0B,0x11,0x0E,0x06,0x10,0x0A,0x09,0x04,0x08,0x0B,0x08,0x0F,0x12,0x0E,0x08}},
 
{0x00,1,{0x00}},
{0xE2,16,{0x02,0x0B,0x11,0x0E,0x06,0x10,0x0A,0x09,0x04,0x08,0x0B,0x08,0x0F,0x12,0x0E,0x08}},
 
{0x00,1,{0x00}}, 
{0xFF,3,{0xFF,0xFF,0xFF}}, 
 
{0x11,1,{0x00}},//SLEEP OUT
{REGFLAG_DELAY,200,{}},
                                                                                                                 
{0x29,1,{0x00}},//Display ON 
{REGFLAG_DELAY,20,{}}, 
{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 0, {0x00}},
        {REGFLAG_DELAY, 120, {}},
    // Sleep Mode On
	{0x10, 0, {0x00}},
        {REGFLAG_DELAY, 120, {}},//20
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for (i = 0; i < count; i++) {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

        case REGFLAG_DELAY :
            MDELAY(table[i].count);
            break;

        case REGFLAG_END_OF_TABLE :
            break;

        default:
            dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }

}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));	
	params->type   = LCM_TYPE_DSI;
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
    params->dbi.te_mode = LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity = LCM_POLARITY_RISING;
#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;  //SYNC_EVENT_VDO_MODE; //CMD_MODE; //SYNC_PULSE_VDO_MODE; //BURST_VDO_MODE;
#else
    params->dsi.mode = SYNC_PULSE_VDO_MODE;
#endif
	// DSI  
	//Command mode setting
#if (FRAME_WIDTH < 480) 
	params->dsi.LANE_NUM = LCM_ONE_LANE;  // LCM_TWO_LANE;
#else
	params->dsi.LANE_NUM = LCM_TWO_LANE;  // LCM_ONE_LANE;
#endif

	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Video mode setting		
    params->dsi.intermediat_buffer_num = 2;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    
    params->dsi.word_count=FRAME_WIDTH*3;	//DSI CMD mode need set these two bellow params, different to 6577
    params->dsi.vertical_active_line=FRAME_HEIGHT;

    params->dsi.vertical_sync_active				= 6;
    params->dsi.vertical_backporch					= 16;
    params->dsi.vertical_frontporch					= 15;	
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active				= 6;
    params->dsi.horizontal_backporch				= 32;
    params->dsi.horizontal_frontporch				= 32;
    //params->dsi.horizontal_blanking_pixel			= 60;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
   // params->dsi.compatibility_for_nvk = 0;		// this parameter would be set to 1 if DriverIC is NTK's and when force match DSI clock for NTK's

    // Bit rate calculation
#ifdef CONFIG_MT6589_FPGA
    params->dsi.pll_div1=2;		// div1=0,1,2,3;div1_real=1,2,4,4
    params->dsi.pll_div2=2;		// div2=0,1,2,3;div1_real=1,2,4,4
    params->dsi.fbk_div =8;		// fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)
#else
	// Bit rate calculation
    params->dsi.pll_div1=1;		// div1=0,1,2,3;div1_real=1,2,4,4
    params->dsi.pll_div2=1;		// div2=0,1,2,3;div2_real=1,2,4,4
 //   params->dsi.fbk_sel=1;		 // fbk_sel=0,1,2,3;fbk_sel_real=1,2,4,4
    params->dsi.fbk_div =31;		// fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)
#endif
}

static void lcm_init(void)
{
	#if defined(BUILD_LK)
	upmu_set_rg_vgp2_vosel(5);
	upmu_set_rg_vgp2_en(1);

	upmu_set_rg_vgp3_vosel(3);
	upmu_set_rg_vgp3_en(1);	
#else
	hwPowerOn(MT6323_POWER_LDO_VGP2, VOL_2800, "Lance_LCM");
  hwPowerOn(MT6323_POWER_LDO_VGP3, VOL_1800, "Lance_LCM");
#endif
    MDELAY(100);


    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(10);//10
    SET_RESET_PIN(1);
    MDELAY(50);//50

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

 SET_RESET_PIN(0);
   MDELAY(1);
   SET_RESET_PIN(1);



}


static void lcm_resume(void)
{
//    Lcd_Log("\n %s \n",__func__);
//		lcm_initialization_setting[30].para_list[0] += 2;
//		Lcd_Log("mycat dxc vcom = 0x%02x\n", lcm_initialization_setting[30].para_list[0]);
    lcm_init();

//	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

static int get_lcd_id(void)
{
    mt_set_gpio_mode(GPIO_LCD_ID_PIN,0);
    mt_set_gpio_dir(GPIO_LCD_ID_PIN,0);
    mt_set_gpio_pull_enable(GPIO_LCD_ID_PIN,1);
    mt_set_gpio_pull_select(GPIO_LCD_ID_PIN,0);
    MDELAY(10);

    return mt_get_gpio_in(GPIO_LCD_ID_PIN);
}


static unsigned int lcm_compare_id(void)
{
	int   array[4];
		char  buffer[5];
		unsigned int id=0;
	#if defined(BUILD_LK)
	upmu_set_rg_vgp2_vosel(5);
	upmu_set_rg_vgp2_en(1);

	upmu_set_rg_vgp3_vosel(3);
	upmu_set_rg_vgp3_en(1);	
#else
	hwPowerOn(MT6323_POWER_LDO_VGP2, VOL_2800, "Lance_LCM");
  hwPowerOn(MT6323_POWER_LDO_VGP3, VOL_1800, "Lance_LCM");
#endif
    MDELAY(100);	
		SET_RESET_PIN(0);
		MDELAY(200);
		SET_RESET_PIN(1);
		MDELAY(200);
		
	array[0] = 0x00083700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xA1,buffer,4);
	
	id=(buffer[2]<<8)+buffer[3];

	return (LCM_ID == id)?1:0;
}

LCM_DRIVER otm9605a_rixin_NST500QH2542ANJ_V1_LG_dsi_2_vdo_qhd_lcm_drv =
    {
        .name			= "otm9605a_rixin_NST500QH2542ANJ_V1_LG_dsi_2_vdo_qhd",
        .set_util_funcs = lcm_set_util_funcs,
        .get_params     = lcm_get_params,
        .init           = lcm_init,
        .suspend        = lcm_suspend,
        .resume         = lcm_resume,
        .compare_id    = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
        .set_backlight	= lcm_setbacklight,
        .update         = lcm_update,
#endif
    };

