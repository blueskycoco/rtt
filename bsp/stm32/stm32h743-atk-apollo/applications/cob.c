#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "md5.h"

int read_file(const uint8_t *file, uint8_t *buf)
{
	FILE *fp=fopen(file,"rb");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	fseek(fp,0L,SEEK_SET);

	fread(buf,flen,1,fp);
	fclose(fp);
	printf("read %s , %d bytes\n", file, flen);
	return flen;
}

int write_file(const uint8_t *file, uint8_t *buf, int len)
{
	FILE *fp=fopen(file,"wb");

	fwrite(buf,len,1,fp);
	fclose(fp);
	printf("write %s , %d bytes\n", file, len);
	return len;
}

int main(int argc, void *argv[])
{
	int i,j;
	uint32_t ofs = 0;
	uint8_t decrypt[16] = {0};    
	MD5_CTX md5;

	/* h7_len1(4) md5_h7(16) h7_fw(x) radar_len(4) md5_radar(16) radar(y) */
	if (argc != 4) {
		printf("usage: ./cob ota.img app.bin radar.bin\n");
		return 0;
	}
	uint8_t *ota_img = (uint8_t *)calloc(1, 1024 * 1024);
	uint8_t *app_img = (uint8_t *)calloc(1, 624 * 1024);
	uint8_t *radar_img = (uint8_t *)calloc(1, 400 * 1024);
	int len_app = read_file(argv[2], app_img);
	int len_radar = read_file(argv[3], radar_img);

	ota_img[0] = (len_app >> 24) & 0xff;
	ota_img[1] = (len_app >> 16) & 0xff;
	ota_img[2] = (len_app >>  8) & 0xff;
	ota_img[3] = (len_app >>  0) & 0xff;
	for (i = 0; i < len_app; i++)
		ota_img[i + 20] = app_img[i];
	MD5Init(&md5);	
	MD5Update(&md5, app_img, len_app);
	MD5Final(&md5, ota_img + 4);
	printf("app md5-len-sum: ");
	for(i = 0;i < 20; i++)
		printf("%02x ", ota_img[4 + i]);
	printf("\r\n");

	ofs = len_app + 20;
	ota_img[ofs + 0] = (len_radar >> 24) & 0xff;
	ota_img[ofs + 1] = (len_radar >> 16) & 0xff;
	ota_img[ofs + 2] = (len_radar >>  8) & 0xff;
	ota_img[ofs + 3] = (len_radar >>  0) & 0xff;
	for (i = 0; i < len_radar; i++)
		ota_img[i + 20 + ofs] = radar_img[i];
	MD5Init(&md5);	
	MD5Update(&md5, radar_img, len_radar);
	MD5Final(&md5, ota_img + 4 + ofs);
	printf("radar md5-len-sum: ");
	for(i = 0;i < 20; i++)
		printf("%02x ", ota_img[4 + i + ofs]);
	printf("\r\n");
	
	write_file(argv[1], ota_img, len_app + len_radar + 40);
	free(ota_img);
	free(app_img);
	free(radar_img);
	return 0;
}
