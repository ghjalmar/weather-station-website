/* 
 *  Sketch for receiving sensor readings sent from Arduino to RPi
 *  Code for the RPI (receiver)
 *  Based on example code by TMRh20 and Maniacbug
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>

using namespace std;
// RPI Pin configuration
RF24 radio(22,0);

// Assign a unique identifier for this node, 0 or 1 
bool radioNumber = 1;

// Radio pipe addresses for the 2 nodes to communicate.
const uint8_t pipes[][6] = {"1Node","2Node"};


int main(int argc, char** argv){
  // Configuring for a receiver (RPI)
  // Setup and configure rf radio
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
  
  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.

    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
	
	radio.startListening();
	
	// forever loop the listening
	while (1)
	{
		
		//  Receive each packet, dump it out, and send it back
			
		// if there is data ready
		if ( radio.available() )
		{
			// Dump the payloads until we've gotten everything
			float read_data;

			// Fetch the payload, and see if this was the last one.
			while(radio.available()){
				radio.read( &read_data, sizeof(float) );
			}
			radio.stopListening();
			
			radio.write( &read_data, sizeof(float));

			// Now, resume listening so we catch the next packets.
			radio.startListening();

			// Spew it
			printf("Got payload(%d) %f...\n",sizeof(float), read_data);
			
			delay(925); //Delay after payload responded to, minimize RPi CPU time
			
		}
		


	} // forever loop

  return 0;
}
