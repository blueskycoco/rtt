/**
  ******************************************************************************
  * @file    stm32_eval_sdio_sd.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file contains all the functions prototypes for the SD Card 
  *          stm32_eval_sdio_sd driver firmware library.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_EVAL_SDIO_SD_H
#define __STM32_EVAL_SDIO_SD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include	"sdio_defs.h"
#include	"sdio_error.h"
#include	"sdio_spec.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup Common
  * @{
  */
  
/** @addtogroup STM32_EVAL_SDIO_SD
  * @{
  */  

/** @defgroup STM32_EVAL_SDIO_SD_Exported_Types
  * @{
  */ 
typedef enum
{
/** 
  * @brief  SDIO specific error defines  
  */   
  SD_CMD_CRC_FAIL                    = (1), /*!< Command response received (but CRC check failed) */
  SD_DATA_CRC_FAIL                   = (2), /*!< Data bock sent/received (CRC check Failed) */
  SD_CMD_RSP_TIMEOUT                 = (3), /*!< Command response timeout */
  SD_DATA_TIMEOUT                    = (4), /*!< Data time out */
  SD_TX_UNDERRUN                     = (5), /*!< Transmit FIFO under-run */
  SD_RX_OVERRUN                      = (6), /*!< Receive FIFO over-run */
  SD_START_BIT_ERR                   = (7), /*!< Start bit not detected on all data signals in widE bus mode */
  SD_CMD_OUT_OF_RANGE                = (8), /*!< CMD's argument was out of range.*/
  SD_ADDR_MISALIGNED                 = (9), /*!< Misaligned address */
  SD_BLOCK_LEN_ERR                   = (10), /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
  SD_ERASE_SEQ_ERR                   = (11), /*!< An error in the sequence of erase command occurs.*/
  SD_BAD_ERASE_PARAM                 = (12), /*!< An Invalid selection for erase groups */
  SD_WRITE_PROT_VIOLATION            = (13), /*!< Attempt to program a write protect block */
  SD_LOCK_UNLOCK_FAILED              = (14), /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
  SD_COM_CRC_FAILED                  = (15), /*!< CRC check of the previous command failed */
  SD_ILLEGAL_CMD                     = (16), /*!< Command is not legal for the card state */
  SD_CARD_ECC_FAILED                 = (17), /*!< Card internal ECC was applied but failed to correct the data */
  SD_CC_ERROR                        = (18), /*!< Internal card controller error */
  SD_GENERAL_UNKNOWN_ERROR           = (19), /*!< General or Unknown error */
  SD_STREAM_READ_UNDERRUN            = (20), /*!< The card could not sustain data transfer in stream read operation. */
  SD_STREAM_WRITE_OVERRUN            = (21), /*!< The card could not sustain data programming in stream mode */
  SD_CID_CSD_OVERWRITE               = (22), /*!< CID/CSD overwrite error */
  SD_WP_ERASE_SKIP                   = (23), /*!< only partial address space was erased */
  SD_CARD_ECC_DISABLED               = (24), /*!< Command has been executed without using internal ECC */
  SD_ERASE_RESET                     = (25), /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
  SD_AKE_SEQ_ERROR                   = (26), /*!< Error in sequence of authentication. */
  SD_INVALID_VOLTRANGE               = (27),
  SD_ADDR_OUT_OF_RANGE               = (28),
  SD_SWITCH_ERROR                    = (29),
  SD_SDIO_DISABLED                   = (30),
  SD_SDIO_FUNCTION_BUSY              = (31),
  SD_SDIO_FUNCTION_FAILED            = (32),
  SD_SDIO_UNKNOWN_FUNCTION           = (33),

/** 
  * @brief  Standard error defines   
  */ 
  SD_INTERNAL_ERROR, 
  SD_NOT_CONFIGURED,
  SD_REQUEST_PENDING, 
  SD_REQUEST_NOT_APPLICABLE, 
  SD_INVALID_PARAMETER,  
  SD_UNSUPPORTED_FEATURE,  
  SD_UNSUPPORTED_HW,  
  SD_ERROR,  
  SD_OK = 0 
} SD_Error;

/** 
  * @brief  SDIO Transfer state  
  */   
typedef enum
{
  SD_TRANSFER_OK  = 0,
  SD_TRANSFER_BUSY = 1,
  SD_TRANSFER_ERROR
} SDTransferState;

/** 
  * @brief  SD Card States 
  */   
typedef enum
{
  SD_CARD_READY                  = ((uint32_t)0x00000001),
  SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
  SD_CARD_STANDBY                = ((uint32_t)0x00000003),
  SD_CARD_TRANSFER               = ((uint32_t)0x00000004),
  SD_CARD_SENDING                = ((uint32_t)0x00000005),
  SD_CARD_RECEIVING              = ((uint32_t)0x00000006),
  SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
  SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
  SD_CARD_ERROR                  = ((uint32_t)0x000000FF)
}SDCardState;


/** 
  * @brief  Card Specific Data: CSD Register   
  */ 
typedef struct
{
  __IO uint8_t  CSDStruct;            /*!< CSD structure */
  __IO uint8_t  SysSpecVersion;       /*!< System specification version */
  __IO uint8_t  Reserved1;            /*!< Reserved */
  __IO uint8_t  TAAC;                 /*!< Data read access-time 1 */
  __IO uint8_t  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
  __IO uint8_t  MaxBusClkFrec;        /*!< Max. bus clock frequency */
  __IO uint16_t CardComdClasses;      /*!< Card command classes */
  __IO uint8_t  RdBlockLen;           /*!< Max. read data block length */
  __IO uint8_t  PartBlockRead;        /*!< Partial blocks for read allowed */
  __IO uint8_t  WrBlockMisalign;      /*!< Write block misalignment */
  __IO uint8_t  RdBlockMisalign;      /*!< Read block misalignment */
  __IO uint8_t  DSRImpl;              /*!< DSR implemented */
  __IO uint8_t  Reserved2;            /*!< Reserved */
  __IO uint32_t DeviceSize;           /*!< Device Size */
  __IO uint8_t  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
  __IO uint8_t  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
  __IO uint8_t  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
  __IO uint8_t  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
  __IO uint8_t  DeviceSizeMul;        /*!< Device size multiplier */
  __IO uint8_t  EraseGrSize;          /*!< Erase group size */
  __IO uint8_t  EraseGrMul;           /*!< Erase group size multiplier */
  __IO uint8_t  WrProtectGrSize;      /*!< Write protect group size */
  __IO uint8_t  WrProtectGrEnable;    /*!< Write protect group enable */
  __IO uint8_t  ManDeflECC;           /*!< Manufacturer default ECC */
  __IO uint8_t  WrSpeedFact;          /*!< Write speed factor */
  __IO uint8_t  MaxWrBlockLen;        /*!< Max. write data block length */
  __IO uint8_t  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
  __IO uint8_t  Reserved3;            /*!< Reserded */
  __IO uint8_t  ContentProtectAppli;  /*!< Content protection application */
  __IO uint8_t  FileFormatGrouop;     /*!< File format group */
  __IO uint8_t  CopyFlag;             /*!< Copy flag (OTP) */
  __IO uint8_t  PermWrProtect;        /*!< Permanent write protection */
  __IO uint8_t  TempWrProtect;        /*!< Temporary write protection */
  __IO uint8_t  FileFormat;           /*!< File Format */
  __IO uint8_t  ECC;                  /*!< ECC code */
  __IO uint8_t  CSD_CRC;              /*!< CSD CRC */
  __IO uint8_t  Reserved4;            /*!< always 1*/
} SD_CSD;

/** 
  * @brief  Card Identification Data: CID Register   
  */
typedef struct
{
  __IO uint8_t  ManufacturerID;       /*!< ManufacturerID */
  __IO uint16_t OEM_AppliID;          /*!< OEM/Application ID */
  __IO uint32_t ProdName1;            /*!< Product Name part1 */
  __IO uint8_t  ProdName2;            /*!< Product Name part2*/
  __IO uint8_t  ProdRev;              /*!< Product Revision */
  __IO uint32_t ProdSN;               /*!< Product Serial Number */
  __IO uint8_t  Reserved1;            /*!< Reserved1 */
  __IO uint16_t ManufactDate;         /*!< Manufacturing Date */
  __IO uint8_t  CID_CRC;              /*!< CID CRC */
  __IO uint8_t  Reserved2;            /*!< always 1 */
} SD_CID;

/** 
  * @brief SD Card Status 
  */
typedef struct
{
  __IO uint8_t DAT_BUS_WIDTH;
  __IO uint8_t SECURED_MODE;
  __IO uint16_t SD_CARD_TYPE;
  __IO uint32_t SIZE_OF_PROTECTED_AREA;
  __IO uint8_t SPEED_CLASS;
  __IO uint8_t PERFORMANCE_MOVE;
  __IO uint8_t AU_SIZE;
  __IO uint16_t ERASE_SIZE;
  __IO uint8_t ERASE_TIMEOUT;
  __IO uint8_t ERASE_OFFSET;
} SD_CardStatus;


/** 
  * @brief SD Card information 
  */
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  uint32_t CardCapacity;  /*!< Card Capacity */
  uint32_t CardBlockSize; /*!< Card Block Size */
  uint16_t RCA;
  uint8_t CardType;
} SD_CardInfo;
/***********from marvell sdio driver */

#define SD_BUS_WIDTH_1			0x00
#define SD_BUS_WIDTH_4			0x02
#define SD_BUS_WIDTH_MASK		0x03
#define ASYNC_INT_MODE			0x20

/* Host Control Registers */
#define IO_PORT_0_REG			0x00
#define IO_PORT_1_REG			0x01
#define IO_PORT_2_REG			0x02
#define CONFIGURATION_REG		0x03
#define HOST_WO_CMD53_FINISH_HOST	(0x1U << 2)
#define HOST_POWER_UP			(0x1U << 1)
#define HOST_POWER_DOWN			(0x1U << 0)
#define HOST_INT_MASK_REG		0x04
#define UP_LD_HOST_INT_MASK		(0x1U)
#define DN_LD_HOST_INT_MASK		(0x2U)
#define HOST_INTSTATUS_REG		0x05
#define UP_LD_HOST_INT_STATUS		(0x1U)
#define DN_LD_HOST_INT_STATUS		(0x2U)
#define HOST_INT_RSR_REG		0x06
#define UP_LD_HOST_INT_RSR		(0x1U)
#define HOST_INT_STATUS_REG		0x07
#define UP_LD_CRC_ERR			(0x1U << 2)
#define UP_LD_RESTART              	(0x1U << 1)
#define DN_LD_RESTART              	(0x1U << 0)

/* Card Control Registers */
#define SQ_READ_BASE_ADDRESS_A0_REG  	0x10
#define SQ_READ_BASE_ADDRESS_A1_REG  	0x11
#define SQ_READ_BASE_ADDRESS_A2_REG  	0x12
#define SQ_READ_BASE_ADDRESS_A3_REG  	0x13
#define SQ_READ_BASE_ADDRESS_B0_REG  	0x14
#define SQ_READ_BASE_ADDRESS_B1_REG  	0x15
#define SQ_READ_BASE_ADDRESS_B2_REG  	0x16
#define SQ_READ_BASE_ADDRESS_B3_REG  	0x17
#define CARD_STATUS_REG              	0x20
#define CARD_IO_READY              	(0x1U << 3)
#define CIS_CARD_RDY                 	(0x1U << 2)
#define UP_LD_CARD_RDY               	(0x1U << 1)
#define DN_LD_CARD_RDY               	(0x1U << 0)
#define HOST_INTERRUPT_MASK_REG      	0x24
#define HOST_POWER_INT_MASK          	(0x1U << 3)
#define ABORT_CARD_INT_MASK          	(0x1U << 2)
#define UP_LD_CARD_INT_MASK          	(0x1U << 1)
#define DN_LD_CARD_INT_MASK          	(0x1U << 0)
#define CARD_INTERRUPT_STATUS_REG    	0x28
#define POWER_UP_INT                 	(0x1U << 4)
#define POWER_DOWN_INT               	(0x1U << 3)
#define CARD_INTERRUPT_RSR_REG       	0x2c
#define POWER_UP_RSR                 	(0x1U << 4)
#define POWER_DOWN_RSR               	(0x1U << 3)
#define DEBUG_0_REG                  	0x30
#define SD_TESTBUS0                  	(0x1U)
#define DEBUG_1_REG                  	0x31
#define SD_TESTBUS1                  	(0x1U)
#define DEBUG_2_REG                  	0x32
#define SD_TESTBUS2                  	(0x1U)
#define DEBUG_3_REG                  	0x33
#define SD_TESTBUS3                  	(0x1U)
#define CARD_OCR_0_REG               	0x34
#define CARD_OCR_1_REG               	0x35
#define CARD_OCR_3_REG               	0x36
#define CARD_CONFIG_REG              	0x38
#define CARD_REVISION_REG            	0x3c
#define CMD53_FINISH_GBUS            	(0x1U << 1)
#define SD_NEG_EDGE                  	(0x1U << 0)

/* Special registers in function 0 of the SDxx card */
#define	SCRATCH_0_REG			0x80fe
#define	SCRATCH_1_REG			0x80ff
#define HOST_F1_RD_BASE_0		0x0010
#define HOST_F1_RD_BASE_1		0x0011
#define HOST_F1_CARD_RDY		0x0020

typedef struct _card_capability
{
    uint8_t num_of_io_funcs;         /* Number of i/o functions */
    uint8_t memory_yes;              /* Memory present ? */
    uint16_t rca;                    /* Relative Card Address */
    uint32_t ocr;                    /* Operation Condition register */
    uint16_t fnblksz[8];
    uint32_t cisptr[8];
} card_capability;

typedef struct _dummy_tmpl
{
    int irq_line;
} dummy_tmpl;

typedef struct _mmc_card_rec
{
    uint8_t chiprev;
    uint8_t block_size_512;
    uint8_t async_int_mode;
    card_capability info;
    struct rt_semaphore sem_lock;
        uint16_t manf_id;
    uint16_t dev_id;

//    struct _sdio_host *ctrlr;
} mmc_card_rec;

typedef struct _mmc_card_rec *mmc_card_t;

typedef struct _sdio_host *mmc_controller_t;

typedef enum _sdio_fsm
{
    SDIO_FSM_IDLE = 1,
    SDIO_FSM_CLK_OFF,
    SDIO_FSM_END_CMD,
    SDIO_FSM_BUFFER_IN_TRANSIT,
    SDIO_FSM_END_BUFFER,
    SDIO_FSM_END_IO,
    SDIO_FSM_END_PRG,
    SDIO_FSM_ERROR
} sdio_fsm_state;
#if 0
typedef struct _sdio_host
{
    int usage;
    int slot;
    uint16_t manf_id;
    uint16_t dev_id;
    int bus_width;
    int dma_init;
    int irq_line;
    int card_int_ready;
    uint32_t num_ofcmd52;
    uint32_t num_ofcmd53;

#ifdef IRQ_DEBUG
    int irqcnt;
    int timeo;
#endif
    int busy;                   /* atomic busy flag               */
    uint32_t mmc_i_reg;              /* interrupt last requested       */
    uint32_t mmc_i_mask;             /* mask to be set by intr handler */
    uint32_t mmc_stat;               /* status register at last intr   */
    uint32_t mmc_cmdat;              /* MMC_CMDAT at the last inr      */
    uint32_t saved_mmc_clkrt;
    uint32_t saved_mmc_i_mask;
    uint32_t saved_mmc_resto;
    uint32_t saved_mmc_spi;
    uint32_t saved_drcmrrxmmc;
    uint32_t saved_drcmrtxmmc;
    uint32_t suspended;
    uint32_t clkrt;                  /* current bus clock rate         */
    uint8_t mmc_res[8];              /* Allignment                     */

    /*
     * DMA Related 
     */
    ssize_t blksz;              /* current block size in bytes     */
    ssize_t bufsz;              /* buffer size for each transfer   */
    ssize_t nob;                /* number of blocks pers buffer    */

    int chan;                   /* dma channel no                  */

    dma_addr_t phys_addr;       /* iodata physical address         */

    pxa_dma_desc *read_desc;    /* input descriptor array         */

    pxa_dma_desc *write_desc;   /* output descriptor 
                                   array virtual address */
    dma_addr_t read_desc_phys_addr;     /* descriptor array 
                                           physical address      */
    dma_addr_t write_desc_phys_addr;    /* descriptor array 
                                           physical address      */
    pxa_dma_desc *last_read_desc;       /* last input descriptor 
                                           used by the previous transfer 
                                         */
    pxa_dma_desc *last_write_desc;      /* last output descriptor
                                           used by the previous 
                                           transfer              */

    sdio_fsm_state state;

    card_capability card_capability;
    char *iodata;               /* I/O data buffer           */
    struct _dummy_tmpl *tmpl;
    struct completion completion;       /* completion                */
    struct semaphore io_sem;
    struct rw_semaphore rw_semaphore;
    mmc_card_t card;
} __attribute__ ((aligned)) sdio_ctrller;
#endif
typedef struct _sdio_operations
{
    char name[16];
} sdio_operations;

typedef struct _iorw_extended_t
{
    uint8_t rw_flag;          /** If 0 command is READ; else if 1 command is WRITE */
    uint8_t func_num;
    uint8_t blkmode;
    uint8_t op_code;
    uint32_t reg_addr;
    uint32_t byte_cnt;
    uint32_t blk_size;
    uint8_t *buf;
} iorw_extended_t;

typedef struct _ioreg
{
    uint8_t read_or_write;
    uint8_t function_num;
    uint32_t reg_addr;
    uint8_t dat;
} ioreg_t;

typedef struct _mmc_notifier_rec mmc_notifier_rec_t;
typedef struct _mmc_notifier_rec *mmc_notifier_t;

typedef int (*mmc_notifier_fn_t) (mmc_card_t);

struct _mmc_notifier_rec
{
    mmc_notifier_fn_t add;
    mmc_notifier_fn_t remove;
};

typedef enum _mmc_response
{
    MMC_NORESPONSE = 1,
    MMC_R1,
    MMC_R2,
    MMC_R3,
    MMC_R4,
    MMC_R5,
    MMC_R6
} mmc_response;

enum _cmd53_rw
{
    IOMEM_READ = 0,
    IOMEM_WRITE = 1
};

enum _cmd53_mode
{
    BLOCK_MODE = 1,
    BYTE_MODE = 0
};

enum _cmd53_opcode
{
    FIXED_ADDRESS = 0,
    INCREMENTAL_ADDRESS = 1
};

#define SDIO_BUSWIDTH_1_BIT 1
#define SDIO_BUSWIDTH_4_BIT 4

#define ECSI_BIT	    0x20
#define BUSWIDTH_1_BIT	    0x00
#define BUSWIDTH_4_BIT	    0x02

#define	MMC_TYPE_HOST		1
#define	MMC_TYPE_CARD		2
#define	MMC_REG_TYPE_USER	3

#define	SDIO_READ		0
#define	SDIO_WRITE		1
#define	MRVL_MANFID		0x2df
#define	MRVL_DEVID		0x9103
#define	NO			0
#define	YES			1
#define	TRUE			1
#define	FALSE			0
#define	ENABLED 		1
#define	DISABLED 		0
#define	SIZE_OF_TUPLE 		255

/***********from marvell sdio driver */

/**
  * @}
  */
  
/** @defgroup STM32_EVAL_SDIO_SD_Exported_Constants
  * @{
  */ 

/** 
  * @brief SDIO Commands  Index 
  */
#define SD_CMD_GO_IDLE_STATE                       ((uint8_t)0)
#define SD_CMD_SEND_OP_COND                        ((uint8_t)1)
#define SD_CMD_ALL_SEND_CID                        ((uint8_t)2)
#define SD_CMD_SET_REL_ADDR                        ((uint8_t)3) /*!< SDIO_SEND_REL_ADDR for SD Card */
#define SD_CMD_SET_DSR                             ((uint8_t)4)
#define SD_CMD_SDIO_SEN_OP_COND                    ((uint8_t)5)
#define SD_CMD_HS_SWITCH                           ((uint8_t)6)
#define SD_CMD_SEL_DESEL_CARD                      ((uint8_t)7)
#define SD_CMD_HS_SEND_EXT_CSD                     ((uint8_t)8)
#define SD_CMD_SEND_CSD                            ((uint8_t)9)
#define SD_CMD_SEND_CID                            ((uint8_t)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((uint8_t)11) /*!< SD Card doesn't support it */
#define SD_CMD_STOP_TRANSMISSION                   ((uint8_t)12)
#define SD_CMD_SEND_STATUS                         ((uint8_t)13)
#define SD_CMD_HS_BUSTEST_READ                     ((uint8_t)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((uint8_t)15)
#define SD_CMD_SET_BLOCKLEN                        ((uint8_t)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((uint8_t)17)
#define SD_CMD_READ_MULT_BLOCK                     ((uint8_t)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((uint8_t)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((uint8_t)20) /*!< SD Card doesn't support it */
#define SD_CMD_SET_BLOCK_COUNT                     ((uint8_t)23) /*!< SD Card doesn't support it */
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((uint8_t)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((uint8_t)25)
#define SD_CMD_PROG_CID                            ((uint8_t)26) /*!< reserved for manufacturers */
#define SD_CMD_PROG_CSD                            ((uint8_t)27)
#define SD_CMD_SET_WRITE_PROT                      ((uint8_t)28)
#define SD_CMD_CLR_WRITE_PROT                      ((uint8_t)29)
#define SD_CMD_SEND_WRITE_PROT                     ((uint8_t)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((uint8_t)32) /*!< To set the address of the first write
                                                                  block to be erased. (For SD card only) */
#define SD_CMD_SD_ERASE_GRP_END                    ((uint8_t)33) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For SD card only) */
#define SD_CMD_ERASE_GRP_START                     ((uint8_t)35) /*!< To set the address of the first write block to be erased.
                                                                  (For MMC card only spec 3.31) */

#define SD_CMD_ERASE_GRP_END                       ((uint8_t)36) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For MMC card only spec 3.31) */

#define SD_CMD_ERASE                               ((uint8_t)38)
#define SD_CMD_FAST_IO                             ((uint8_t)39) /*!< SD Card doesn't support it */
#define SD_CMD_GO_IRQ_STATE                        ((uint8_t)40) /*!< SD Card doesn't support it */
#define SD_CMD_LOCK_UNLOCK                         ((uint8_t)42)
#define SD_CMD_APP_CMD                             ((uint8_t)55)
#define SD_CMD_GEN_CMD                             ((uint8_t)56)
#define SD_CMD_NO_CMD                              ((uint8_t)64)

/** 
  * @brief Following commands are SD Card Specific commands.
  *        SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((uint8_t)6)  /*!< For SD Card only */
#define SD_CMD_SD_APP_STAUS                        ((uint8_t)13) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((uint8_t)22) /*!< For SD Card only */
#define SD_CMD_SD_APP_OP_COND                      ((uint8_t)41) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((uint8_t)42) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_SCR                     ((uint8_t)51) /*!< For SD Card only */
#define SD_CMD_SDIO_RW_DIRECT                      ((uint8_t)52) /*!< For SD I/O Card only */
#define SD_CMD_SDIO_RW_EXTENDED                    ((uint8_t)53) /*!< For SD I/O Card only */

/** 
  * @brief Following commands are SD Card Specific security commands.
  *        SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SD_CMD_SD_APP_GET_MKB                      ((uint8_t)43) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_MID                      ((uint8_t)44) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RN1                  ((uint8_t)45) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RN2                  ((uint8_t)46) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RES2                 ((uint8_t)47) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RES1                 ((uint8_t)48) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((uint8_t)18) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((uint8_t)25) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_ERASE                 ((uint8_t)38) /*!< For SD Card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((uint8_t)49) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((uint8_t)48) /*!< For SD Card only */
  
/* Uncomment the following line to select the SDIO Data transfer mode */  
#define SD_DMA_MODE                                ((uint32_t)0x00000000)
/*#define SD_POLLING_MODE                            ((uint32_t)0x00000002)*/

/**
  * @brief  SD detection on its memory slot
  */
#define SD_PRESENT                                 ((uint8_t)0x01)
#define SD_NOT_PRESENT                             ((uint8_t)0x00)

/** 
  * @brief Supported SD Memory Cards 
  */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((uint32_t)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((uint32_t)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((uint32_t)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((uint32_t)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((uint32_t)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((uint32_t)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((uint32_t)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((uint32_t)0x00000007)

/**
  * @}
  */ 
  
/** @defgroup STM32_EVAL_SDIO_SD_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32_EVAL_SDIO_SD_Exported_Functions
  * @{
  */ 
void SD_DeInit(void);
SD_Error SD_Init(void);
SDTransferState SD_GetStatus(void);
SDCardState SD_GetState(void);
uint8_t SD_Detect(void);
SD_Error SD_PowerON(void);
SD_Error SD_PowerOFF(void);
SD_Error SD_InitializeCards(void);
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);
SD_Error SD_GetCardStatus(SD_CardStatus *cardstatus);
SD_Error SD_EnableWideBusOperation(uint32_t WideMode);
SD_Error SD_SelectDeselect(uint32_t addr);
SD_Error SD_ReadBlock(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize);
SD_Error SD_ReadMultiBlocks(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks);
SD_Error SD_WriteBlock(uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize);
SD_Error SD_WriteMultiBlocks(uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks);
SDTransferState SD_GetTransferState(void);
SD_Error SD_StopTransfer(void);
SD_Error SD_Erase(uint32_t startaddr, uint32_t endaddr);
SD_Error SD_SendStatus(uint32_t *pcardstatus);
SD_Error SD_SendSDStatus(uint32_t *psdstatus);
SD_Error SD_ProcessIRQSrc(void);
SD_Error SD_WaitReadOperation(void);
SD_Error SD_WaitWriteOperation(void);
#ifdef __cplusplus
}
#endif

#endif /* __STM32_EVAL_SDIO_SD_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
