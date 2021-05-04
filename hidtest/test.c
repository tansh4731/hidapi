/*******************************************************
 Windows HID simplification

 Alan Ott
 Signal 11 Software

 8/22/2009

 Copyright 2009

 This contents of this file may be used by anyone
 for any reason without any conditions and may be
 used as a starting point for your own applications
 which use HIDAPI.
********************************************************/

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	int res;
	unsigned char buf[256];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	struct hid_device_info *devs, *cur_dev;

	printf("hidapi test/example tool. Compiled with hidapi version %s, runtime version %s.\n", HID_API_VERSION_STR, hid_version_str());
	if (hid_version()->major == HID_API_VERSION_MAJOR && hid_version()->minor == HID_API_VERSION_MINOR && hid_version()->patch == HID_API_VERSION_PATCH) {
		printf("Compile-time version matches runtime version of hidapi.\n\n");
	}
	else {
		printf("Compile-time version is different than runtime version of hidapi.\n]n");
	}

	if (hid_init())
		return -1;

	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;


	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	if (argc > 1) {
		handle = hid_open_path(argv[1]);
	}
	else {
		handle = hid_open(0x222a, 0x0001, NULL);
	}

	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}

	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed String 1: %ls\n", wstr);

	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 0);

#if 1
#define CMD_MAX_NUM 20
	unsigned char cmd_buf[CMD_MAX_NUM][256];
	for (int ii = 0; ii < CMD_MAX_NUM; ii++) {
		memset(cmd_buf[ii], 0, sizeof(cmd_buf[ii]));
	}

	int cmd_index = 0;
	int cmd_byte = 0;

	/*
	cmd_buf[cmd_index][cmd_byte++] = 0x03;
	cmd_buf[cmd_index][cmd_byte++] = 0xF1;
	cmd_buf[cmd_index][cmd_byte++] = 0x00;
	cmd_index++;
	cmd_byte = 0;

	cmd_buf[cmd_index][cmd_byte++] = 0x03;
	cmd_buf[cmd_index][cmd_byte++] = 0xA3;
	cmd_buf[cmd_index][cmd_byte++] = 0x01;
	cmd_buf[cmd_index][cmd_byte++] = 0x03;
	cmd_buf[cmd_index][cmd_byte++] = 0x42;
	cmd_index++;
	cmd_byte = 0;

	cmd_buf[cmd_index][cmd_byte++] = 0x03;
	cmd_buf[cmd_index][cmd_byte++] = 0xA3;
	cmd_buf[cmd_index][cmd_byte++] = 0x01;
	cmd_buf[cmd_index][cmd_byte++] = 0x20;
	cmd_buf[cmd_index][cmd_byte++] = 0x61;
	cmd_index++;
	cmd_byte = 0;

	cmd_buf[cmd_index][cmd_byte++] = 0x03;
	cmd_buf[cmd_index][cmd_byte++] = 0xA3;
	cmd_buf[cmd_index][cmd_byte++] = 0x01;
	cmd_buf[cmd_index][cmd_byte++] = 0x0A;
	cmd_buf[cmd_index][cmd_byte++] = 0x20;
	cmd_index++;
	cmd_byte = 0;

	cmd_buf[cmd_index][cmd_byte++] = 0x03;
	cmd_buf[cmd_index][cmd_byte++] = 0xA3;
	cmd_buf[cmd_index][cmd_byte++] = 0x01;
	cmd_buf[cmd_index][cmd_byte++] = 0x08;
	cmd_buf[cmd_index][cmd_byte++] = 0x40;
	cmd_index++;
	cmd_byte = 0;

	cmd_buf[cmd_index][cmd_byte++] = 0x03;
	cmd_buf[cmd_index][cmd_byte++] = 0xA3;
	cmd_buf[cmd_index][cmd_byte++] = 0x02;
	cmd_buf[cmd_index][cmd_byte++] = 0x00;
	cmd_buf[cmd_index][cmd_byte++] = 0xF2;
	cmd_index++;
	cmd_byte = 0;

	cmd_buf[cmd_index][cmd_byte++] = 0x03;
	cmd_buf[cmd_index][cmd_byte++] = 0xF1;
	cmd_buf[cmd_index][cmd_byte++] = 0x00;
	cmd_index++;
	*/
	// Set_EngineerMode_Enabled
	cmd_byte = 0;
	cmd_buf[cmd_index][cmd_byte++] = 0x03;
	cmd_buf[cmd_index][cmd_byte++] = 0xF1;
	cmd_buf[cmd_index][cmd_byte++] = 0x01;
	cmd_index++;

	for (int ii = 0; ii < cmd_index; ii++) {
		Sleep(50);
		res = hid_set_output_report(handle, cmd_buf[ii], 0x40);
		if (res < 0) {
			printf("Unable to hid_set_output_report.\n");
		}
		else {
			printf("hid_set_output_report done: res = %d\n", res);
		}
	}
#endif

	// Read requested state. hid_read() has been set to be
	// non-blocking by the call to hid_set_nonblocking() above.
	// This loop demonstrates the non-blocking nature of hid_read().
	res = 0;
	while (TRUE) {
		res = hid_read(handle, buf, sizeof(buf));
		if (res == 0)
			printf("waiting...\n");
		else if (res < 0)
			printf("Unable to read()\n");
		else {
			printf("Data read:\n   ");
			// Print out the returned buffer.
			for (i = 0; i < res; i++)
				printf("%02hhx ", buf[i]);
			printf("\n");
		}
	}

	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef WIN32
	system("pause");
#endif

	return 0;
}
