/*
 * File      : ls1c_spi.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-10-23     ��Ϊ��       first version
 */

// Ӳ��spi�ӿڵ�ͷ�ļ�

#ifndef __OPENLOONGSON_SPI_H
#define __OPENLOONGSON_SPI_H


// SPIģ����
typedef enum
{
    LS1C_SPI_0 = 0,
    LS1C_SPI_1,
}ls1c_spi_t;


// Ƭѡ
#define LS1C_SPI_INVALID_CS             (-1)
#define LS1C_SPI_CS_0                   (0)
#define LS1C_SPI_CS_1                   (1)
#define LS1C_SPI_CS_2                   (2)
#define LS1C_SPI_CS_3                   (3)

// ʱ�Ӽ��Ժ���λ
#define SPI_CPOL_1                      (1)
#define SPI_CPOL_0                      (0)
#define SPI_CPHA_1                      (1)
#define SPI_CPHA_0                      (0)


// Ӳ��SPI��Ϣ
typedef struct
{
    ls1c_spi_t SPIx;                    // SPIģ����
    unsigned long max_speed_hz;         // ���ͨ���ٶȣ���λhz
    unsigned char cs;                   // Ƭѡ
    unsigned char cpol;                 // ʱ�Ӽ���
    unsigned char cpha;                 // ʱ����λ
}ls1c_spi_info_t;



/*
 * ��ʼ��ָ��SPIģ��
 * @spi_info_p SPIģ����Ϣ
 */
void spi_init(ls1c_spi_info_t *spi_info_p);


/*
 * ����ָ��ƬѡΪָ��״̬
 * @spi_info_p SPIģ����Ϣ
 * @new_status Ƭѡ���ŵ���״̬��ȡֵΪ0��1�����ߵ�ƽ��͵�ƽ
 */
void spi_set_cs(ls1c_spi_info_t *spi_info_p, int new_status);


/*
 * ͨ��ָ��SPI���ͽ���һ���ֽ�
 * ע�⣬�ڶ������ϵͳ�У��˺�����Ҫ���⡣
 * ����֤�ں�ĳ�����豸�շ�ĳ���ֽڵĹ����У����ܱ��л�����������ͬʱ���������ͬһ��SPI�����ϵĴ��豸ͨ��
 * ��Ϊ��о1c��ÿ·SPI�Ͽ��ܽ��в�ͬ�Ĵ��豸��ͨ��Ƶ�ʡ�ģʽ�ȿ��ܲ�ͬ
 * @spi_info_p SPI�ӿ�
 * @tx_ch �����͵�����
 * @ret �յ�������
 */
unsigned char spi_txrx_byte(ls1c_spi_info_t *spi_info_p, unsigned char tx_ch);


/*
 * ��ӡָ��SPIģ������мĴ�����ֵ
 * @spi_info_p SPIģ����Ϣ
 */
void spi_print_all_regs_info(ls1c_spi_info_t *spi_info_p);


#endif

