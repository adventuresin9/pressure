/*****************************************************/
/* by adventuresin9                                  */
/*                                                   */
/* This is a simple program to read the temperature  */
/* and pressure from the LSP25H chip found on the    */
/* Raspberry Pi Sense Hat.                           */
/*                                                   */
/* It will mount the i²c device address, initialize  */
/* the chip, read the data, then shutdown and close. */
/*                                                   */
/* Look up the datasheet on the LSP25H for more      */
/* details on the settings and the math needed to    */
/* get readable values out of it.                    */
/*****************************************************/

#include <u.h>
#include <libc.h>



int
mountchip(void)
{

	/* default location for lps25h is 5c */

	int fd = -1;

	if(bind("#J5c", "/dev", MBEFORE) < 0)
		sysfatal("no J device");

	fd = open("/dev/i2c.5c.data", ORDWR);
	if(fd < 0)
		sysfatal("no open dev");

	return(fd);
}



void
initchip(int fd)
{

	/* 0x20 is the register to initialize the chip */
	/* 0x90 turns on chip and set refresh to 25Hz  */
	/* using pread and pwrite with an offset of 0  */
	/* gives better results for poking registers   */

	uchar buf[2];

	buf[0] = 0x20;
	buf[1] = 0x30;

	if((pwrite(fd, buf, 2, 0)) < 0)
		sysfatal("initchip failed");
}



int
getpress(int fd)
{
	uchar reg[1];
	uchar presoxl[1], presol[1], presoh[1];
	int press;

	reg[0] = 0x28;
	pwrite(fd, reg, 1, 0);
	pread(fd, presoxl, 1, 0);

	reg[0] = 0x29;
	pwrite(fd, reg, 1, 0);
	pread(fd, presol, 1, 0);

	reg[0] = 0x2A;
	pwrite(fd, reg, 1, 0);
	pread(fd, presoh, 1, 0);

	press = presoxl[0] + (presol[0] << 8) + (presoh[0] << 16);
	press = press / 4096;

	return(press);
}



float
gettemp(int fd)
{
	uchar reg[1];
	uchar tempol[1], tempoh[1];
	int temp;
	float dectemp;

	reg[0] = 0x2B;
	pwrite(fd, reg, 1, 0);
	pread(fd, tempol, 1, 0);

	reg[0] = 0x2C;
	pwrite(fd, reg, 1, 0);
	pread(fd, tempoh, 1, 0);


	temp = tempol[0] + (tempoh[0] << 8);
	if (temp > 32767) temp -= 65536;
	temp = 425 + (temp / 48);

	dectemp = temp;
	dectemp /= 10;

	return(dectemp);
}



void
closechip(int fd)
{
	uchar buf[2];

	buf[0] = 0x20;
	buf[1] = 0x00;

	if((pwrite(fd, buf, 2, 0) < 0))
		sysfatal("closechip write failed");

	close(fd);

	unmount("#J5c", "/dev");
}
	

void
main(int, char*)
{
	int fd;

	fd = mountchip();

	initchip(fd);

	/* These 2 readings and the sleep are to flush   */
	/* old values out of the chip and get fresh ones */

	if(getpress(fd) < 0)
		sysfatal("bad return on pressure");

	if(gettemp(fd) >= 150)
		sysfatal("bad return on temperature");

	sleep(100);


	/* Print fresh temp and pressure values */

	print("Temperature is %.1f C\n", gettemp(fd));
	print("Pressure is %d hPa\n", getpress(fd));

	closechip(fd);

	exits(nil);
}

