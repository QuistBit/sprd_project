#include <common.h>
#include <asm/io.h>
#include <asm/arch/ldo.h>
#include <asm/arch/sprd_reg_ahb.h>
#include <asm/arch/regs_ahb.h>
#include <asm/arch/common.h>
#include <asm/arch/adi_hal_internal.h>
#include <asm/u-boot.h>
#include <part.h>
#include <sdhci.h>
#include <asm/arch/mfp.h>
#include <linux/gpio.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pinmap.h>
DECLARE_GLOBAL_DATA_PTR;

extern void sprd_gpio_init(void);
extern void ADI_init (void);
extern int LDO_Init(void);
extern void ADC_Init(void);
extern int sound_init(void);
extern void init_ldo_sleep_gr(void);

#define GPIO_CP2_RFCTL      73

#ifdef CONFIG_GENERIC_MMC
int mv_sdh_init(u32 regbase, u32 max_clk, u32 min_clk, u32 quirks);
int mmc_sdcard_init();

int board_mmc_init(bd_t *bd)
{
	mmc_sdcard_init();

	mv_sdh_init(CONFIG_SYS_SD_BASE, SDIO_BASE_CLK_192M,
			SDIO_CLK_250K, 0);

	return 0;
}
#endif

extern struct eic_gpio_resource sprd_gpio_resource[];

static void cp2_rfctl_init(void)
{
	sprd_gpio_request(NULL,GPIO_CP2_RFCTL);
	sprd_gpio_direction_output(NULL, GPIO_CP2_RFCTL, 1);
	sprd_gpio_set(NULL, GPIO_CP2_RFCTL, 1);
}

typedef enum
{
	TD_MODE,
	W_MODE
}COMM_STD_MODE_E;
static void TD_comm_misc_init(COMM_STD_MODE_E csm)
{
	if(TD_MODE == csm){
		REG32(REG_AON_APB_WTG_TEST) |= BIT_TG_DAC_OUT_SEL;
	}
	REG32(REG_AON_APB_BB_BG_CTRL) |= (BIT_BB_REF_AUTO_PD_EN|BIT_BB_LDO_FORCE_ON|BIT_BB_BG_FORCE_ON);
	REG32(REG_AON_APB_RES_REG0) |= BIT_BB_LDO_AUTO_EN;
}
int board_init()
{
	gd->bd->bi_arch_number = MACH_TYPE_OPENPHONE;
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;
	ADI_init();
	misc_init();
	LDO_Init();
	ADC_Init();
	pin_init();
	sprd_eic_init();
	sprd_gpio_init();
	sound_init();
	init_ldo_sleep_gr();
	if(ANA_GET_CHIP_ID() != 0x2711a000){
		TDPllRefConfig(1);
	}
	cp2_rfctl_init();
	TD_comm_misc_init(TD_MODE);
	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((volatile void *)PHYS_SDRAM_1,
			PHYS_SDRAM_1_SIZE);
	return 0;
}
