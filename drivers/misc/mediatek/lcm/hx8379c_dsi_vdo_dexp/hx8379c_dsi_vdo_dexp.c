//mediatek blablabla

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#else
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
    #include <linux/string.h>
   
#endif
#include "lcm_drv.h"

//  Local Constants


#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(854)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER

////#define LCM_DSI_CMD_MODE									0

#define LCM_ID 0x79
static unsigned int lcm_compare_id(void);
//-----------------------------------------------------------------------

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
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

static struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {

      {0xB9,3, {0xFF,0x83,0x79}},
	
      {0xB1,20,{0x64,0x18,0x18,0x31,0x91,0x90,0xD0,0xEC,0xC8,0xC7,0x38,0x38,0xF8,0x44,0x41,0x14,0x00,0x80,0x30,0x00}},
          
      {0xB2,9, {0x80,0xFE,0x0B,0x04,0x30,0x50,0x11,0x42,0x1D}},

      //{0xB4,10,{0x00,0x80,0x00,0x86,0x00,0x86,0x08,0x86,0x08,0x86}},
      {0xB4,10,{0x00,0x88,0x00,0x88,0x00,0x88,0x08,0x88,0x08,0x88}},
      
      {0xCC,1,{0x02}},
  
      {0xD3,29,{0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x32,0x10,0x06,0x00,0x06,0x03,0x70,0x03,0x70,0x00,0x08,0x00,0x08,0x11,0x11,0x06,0x06,0x13,0x06,0x06,0x13,0x09}},
          

      {0xD5,32,{0x18,0x18,0x19,0x19,0x01,0x00,0x03,0x02,0x21,0x20,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},
                
      
   
      {0xD6,32,{0x18,0x18,0x18,0x18,0x18,0x18,0x26,0x27,0x24,0x25,0x02,0x03,0x00,0x01,0x06,0x07,0x04,0x05,0x22,0x23,0x20,0x21,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},                

      {0xE0,42,{0x00,0x1F,0x23,0x2E,0x34,0x3F,0x29,0x40,0x07,0x0B,0x0D,0x17,0x0F,0x12,0x14,0x12,0x14,0x06,0x11,0x12,0x17,0x00,0x20,0x24,0x2E,0x34,0x3F,0x2A,0x3F,0x07,0x09,0x0B,0x17,0x0D,0x12,0x14,0x12,0x13,0x06,0x11,0x13,0x18}},

      {0xB6,2,{0x3D,0x3D}},

      {0x35,1,{0x00}},
         
      {0x11,1,{0x00}},		// Sleep-Out
      {REGFLAG_DELAY, 150,  {}},  
      {0x29,1,{0x00}},
      {REGFLAG_DELAY, 50,  {}},
{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void RX_LCM_INIT(void)
{
	 unsigned int data_array[16];
	  data_array[0]= 0x00043902;
      data_array[1]= 0x7983FFB9;
      dsi_set_cmdq(&data_array, 2, 1);
      MDELAY(1);

      data_array[0]= 0x00063902;
      data_array[1]= 0x009351ba;
      dsi_set_cmdq(&data_array, 2, 1);
      MDELAY(1);

      data_array[0]= 0x00143902;
      data_array[1]= 0x445000b1;
      data_array[2]= 0x11086FE3;
      data_array[3]= 0x38361111;
      data_array[4]= 0x0B4229A9;
      data_array[5]= 0xE600F166;
      dsi_set_cmdq(&data_array, 6, 1);
      MDELAY(1);

      data_array[0]= 0x000E3902;
      data_array[1]= 0x3C0000b2;
      data_array[2]= 0x2218040C;
      data_array[3]= 0x040CFF00;
      data_array[4]= 0x00002018;
      dsi_set_cmdq(&data_array, 5, 1);
      MDELAY(1);

      /*data_array[0]= 0x00203902;
      data_array[1]= 0x000880b4;
      data_array[2]= 0x32051032;
      data_array[3]= 0x10327013;
      data_array[4]= 0x40003708;
      data_array[5]= 0x3E081306;//34081306
      data_array[6]= 0x04304008;//04303808
      data_array[7]= 0x28084000;
      data_array[8]= 0x04303008;
      dsi_set_cmdq(&data_array, 9, 1);
      MDELAY(1);*/

	  data_array[0]= 0x00203902;
      data_array[1]= 0x000c80b4;
      data_array[2]= 0x00061030;
      data_array[3]= 0x00000000;
      data_array[4]= 0x48001100;
      data_array[5]= 0x403c2307;//34081306
      data_array[6]= 0x04303008;//04303808
      data_array[7]= 0x28084000;
      data_array[8]= 0x04303008;
      dsi_set_cmdq(&data_array, 9, 1);
      MDELAY(1);

      data_array[0]= 0x00023902;
      data_array[1]= 0x000002cc;
      dsi_set_cmdq(&data_array, 2, 1);
      MDELAY(1);


	  data_array[0]= 0x00303902;
      data_array[1]= 0x0a0000d5;
      data_array[2]= 0x00000100;
      data_array[3]= 0x99881000;
      data_array[4]= 0x88103210;
      data_array[5]= 0x88888888;
      data_array[6]= 0x88888888;
      data_array[7]= 0x88888888;
      data_array[8]= 0x99010123;
      data_array[9]= 0x88888888;
      data_array[10]= 0x88888888;
      data_array[11]= 0x00048888;
      data_array[12]= 0x00000000;
      dsi_set_cmdq(&data_array, 13, 1);
      MDELAY(1);
     
      data_array[0]= 0x00243902;
      data_array[1]= 0x080079e0;
      data_array[2]= 0x3f3A330E;
      data_array[3]= 0x07014B1E;
      data_array[4]= 0x1817150e;
      data_array[5]= 0x001A1F17;
      data_array[6]= 0x3A330E08;
      data_array[7]= 0x014B1E3f;
      data_array[8]= 0x17150e07;
      data_array[9]= 0x1A1F1718;
      dsi_set_cmdq(&data_array, 10, 1);
      MDELAY(1);

	 

      data_array[0]= 0x00053902;
      data_array[1]= 0x009400B6;//8c  7b
      data_array[2]= 0x00000094;
      dsi_set_cmdq(&data_array, 3, 1);
      MDELAY(1);

	
      data_array[0] = 0x00110500;
      dsi_set_cmdq(&data_array, 1, 1);
      MDELAY(150);

      data_array[0] = 0x00290500;
      dsi_set_cmdq(&data_array, 1, 1);
      MDELAY(20);
}

static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 10, {}},
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	{0x28, 0, {0x00}},

    // Sleep Mode On
	{0x10, 0, {0x00}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
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
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;


		params->dsi.mode   = SYNC_PULSE_VDO_MODE;

	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;


		params->dsi.vertical_sync_active				= 6;//2
		params->dsi.vertical_backporch					= 5;//14
		params->dsi.vertical_frontporch					= 6;//9
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 25;//6
		params->dsi.horizontal_backporch				= 40;//60
		params->dsi.horizontal_frontporch				= 33;//22
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

                    params->dsi.pll_div1=1;//48;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		params->dsi.pll_div2=1; 		// div2=0~15: fout=fvo/(2*div2)
                params->dsi.fbk_div =31;//31

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

    SET_RESET_PIN(1);
MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(5);
    SET_RESET_PIN(1);
    MDELAY(150);//Must over 6 ms,SPEC request

//	RX_LCM_INIT();
	
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
SET_RESET_PIN(1);
MDELAY(5);	
SET_RESET_PIN(0);	
MDELAY(3);	
	SET_RESET_PIN(1);
	MDELAY(150);	

	///push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);

}


static void lcm_resume(void)
{
lcm_compare_id();
#ifdef BUILD_LK
#else
printk("hwj huaxian lcd resume!!!");
#endif
lcm_init();
	///push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(&data_array, 7, 0);

}


static unsigned int lcm_compare_id(void)
{
	unsigned int id=0;
	unsigned char buffer[4];
	unsigned int array[16];  

	#if defined(BUILD_LK)
	upmu_set_rg_vgp2_vosel(5);
	upmu_set_rg_vgp2_en(1);

	upmu_set_rg_vgp3_vosel(3);
	upmu_set_rg_vgp3_en(1);	
#else
	hwPowerOn(MT6323_POWER_LDO_VGP2, VOL_2800, "Lance_LCM");
       hwPowerOn(MT6323_POWER_LDO_VGP3, VOL_1800, "Lance_LCM");
#endif

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);
    SET_RESET_PIN(1);
    MDELAY(10);//Must over 6 ms

	array[0] = 0x00033700;// return byte number
	dsi_set_cmdq(&array, 1, 1);
	MDELAY(10);

	read_reg_v2(0x04, buffer, 3);
	id = buffer[1]; 
	
#if defined(BUILD_LK)
	printf("MYCAT %s, id = 0x%08x,di2=0x%x,di3=0x%x\n", __func__, id,buffer[1],buffer[2]);
#endif

	return 1;

}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hx8379c_dsi_vdo_dexp_lcm_drv = 
{
    .name			= "hx8379c_dsi_vdo_dexp",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .compare_id    = lcm_compare_id,
};
//end driver. Changes by assusdan
