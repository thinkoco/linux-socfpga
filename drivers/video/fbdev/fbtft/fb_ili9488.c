/*
 * FB driver for the ili9488 LCD Controller
 *
 * Copyright (C) 2014 Noralf Tronnes
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include "fbtft.h"

#define DRVNAME		"fb_ili9488"
#define WIDTH		320
#define HEIGHT		480
#define MIPI_DCS_EXIT_SLEEP_MODE	0x11
#define MIPI_DCS_SET_PIXEL_FORMAT	0x3A
#define MIPI_DCS_EXIT_SLEEP_MODE	0x11
#define MIPI_DCS_SET_DISPLAY_ON		0x29
#define BPP				24

static int default_init_sequence[] = {
        /* Interface Mode Control */
        -1, 0xb0, 0x0,
        -1, MIPI_DCS_EXIT_SLEEP_MODE,
        -2, 250,
        /* Interface Pixel Format */
        -1, MIPI_DCS_SET_PIXEL_FORMAT, 0x66,
        /* Power Control 3 */
        -1, 0xC2, 0x44,
        /* VCOM Control 1 */
        -1, 0xC5, 0x00, 0x00, 0x00, 0x00,
        -1, MIPI_DCS_EXIT_SLEEP_MODE,
        -1, MIPI_DCS_SET_DISPLAY_ON,
        /* end marker */
        -3
};

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	fbtft_par_dbg(DEBUG_SET_ADDR_WIN, par,
		"%s(xs=%d, ys=%d, xe=%d, ye=%d)\n", __func__, xs, ys, xe, ye);

	/* Column address */
	write_reg(par, 0x2A, xs >> 8, xs & 0xFF, xe >> 8, xe & 0xFF);

	/* Row adress */
	write_reg(par, 0x2B, ys >> 8, ys & 0xFF, ye >> 8, ye & 0xFF);

	/* Memory write */
	write_reg(par, 0x2C);
}

static int set_var(struct fbtft_par *par)
{
	fbtft_par_dbg(DEBUG_INIT_DISPLAY, par, "%s()\n", __func__);
	/* RGB666 */
	par->info->var.red.offset    = 16;
	par->info->var.red.length    = 8;
	par->info->var.green.offset  = 8;
	par->info->var.green.length  = 8;
	par->info->var.blue.offset   = 0;
	par->info->var.blue.length   = 8;

	switch (par->info->var.rotate) {
	case 0:
		write_reg(par, 0x36, 0x80 | (par->bgr << 3));
		break;
	case 90:
		write_reg(par, 0x36, 0x20 | (par->bgr << 3));
		break;
	case 180:
		write_reg(par, 0x36, 0x40 | (par->bgr << 3));
		break;
	case 270:
		write_reg(par, 0x36, 0xE0 | (par->bgr << 3));
		break;
	default:
		break;
	}

	return 0;
}

/* 18/24 bit pixel over 8-bit databus */
int fbtft_write_vmem24_bus8(struct fbtft_par *par, size_t offset, size_t len)
{
        u8 *vmem8;
        u8 *txbuf8 = par->txbuf.buf;
        size_t remain;
        size_t to_copy;
        size_t tx_array_size;
        int i;
        int ret = 0;
        size_t startbyte_size = 0;
        fbtft_par_dbg(DEBUG_WRITE_VMEM, par, "%s(offset=%zu, len=%zu)\n",
                __func__, offset, len);

        remain = len / 3;
       // vmem8 = (u8 *)(par->info->screen_buffer + offset);
	vmem8 = (u8 *)(par->info->screen_base + offset);
        if (par->gpio.dc != -1)
                gpio_set_value(par->gpio.dc, 1);

        /* non buffered write */
        if (!par->txbuf.buf)
                return par->fbtftops.write(par, vmem8, len);

        /* buffered write, /4*4 to faster */
        tx_array_size = par->txbuf.len / 3 / 4 *4;

        if (par->startbyte) {
                txbuf8 = par->txbuf.buf + 1;
                tx_array_size -= 1;
                *(u8 *)(par->txbuf.buf) = par->startbyte | 0x2;
                startbyte_size = 1;
        }

        while (remain) {
                to_copy = min(tx_array_size, remain);
                dev_dbg(par->info->device, "    to_copy=%zu, remain=%zu\n",
                                                to_copy, remain - to_copy);

                for (i = 0; i < to_copy/4; i++)
                {       //faster copy
                        *(u32*)(txbuf8+i*12) = *(u32*)(vmem8+i*12);
                        *(u32*)(txbuf8+4+i*12) = *(u32*)(vmem8+4+i*12);
                        *(u32*)(txbuf8+8+i*12) = *(u32*)(vmem8+8+i*12);
                }
                for(i = to_copy/4*4; i < to_copy; i++)
                {
                        txbuf8[i*3] = vmem8[i*3];
                        txbuf8[i*3+1] = vmem8[i*3+1];
                        txbuf8[i*3+2] = vmem8[i*3+2];
                }
                vmem8 = vmem8 + to_copy*3;
                ret = par->fbtftops.write(par, par->txbuf.buf,
                                                startbyte_size + to_copy * 3);
                if (ret < 0)
                        return ret;
                remain -= to_copy;
        }

        return ret;
}
EXPORT_SYMBOL(fbtft_write_vmem24_bus8);

static struct fbtft_display display = {
        .regwidth = 8,
        .width = WIDTH,
        .height = HEIGHT,
        .bpp = BPP,
        .init_sequence = default_init_sequence,
        .fbtftops = {
                .set_addr_win = set_addr_win,
                .set_var = set_var,
                .write_vmem = fbtft_write_vmem24_bus8,
        },
};

FBTFT_REGISTER_DRIVER(DRVNAME, "ilitek,ili9488", &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:ili9488");
MODULE_ALIAS("platform:ili9488");

MODULE_DESCRIPTION("FB driver for the ili9488 LCD Controller");
MODULE_AUTHOR("Noralf Tronnes");
MODULE_LICENSE("GPL");
