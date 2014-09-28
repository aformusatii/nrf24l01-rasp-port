/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "RF24.h"

RF24 radio;
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
void loop();

int main(int argc, char** argv)
{

  printf("Start NRF24L01P test...");

  radio.begin();
  radio.setRetries(15,15);
  radio.setPayloadSize(8);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(120);

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  
  radio.startListening();

  radio.printDetails();

  //while (1) {
	  loop();
  //}

  return 0;
}

void loop() {
	radio.stopListening();
	
	uint8_t data[] = {111, 101};
	bool ok = radio.write(data, 2);

    if (ok)
      printf("ok...\n\r");
    else
      printf("failed.\n\r");
}
