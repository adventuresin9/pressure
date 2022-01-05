# pressure

For Plan 9 on the Raspberry Pi with a Sense Hat

or 9 Front if you have i2c support added in

This is a simple program to access the LPS25H chip on the Sense Hat
to get the current temperature in C, and air pressure in hPa.

It takes no arguments and simply prints the values with some text and
readable formatting to standard out.

Just compile and link using 

5c lps25h.c
5l lps25h.5
and that will default to give a 5.out executable.

7c and 7l for arm64 on 9Front.

See the datasheet for more info on the chip
https://www.st.com/en/mems-and-sensors/lps25h.html

