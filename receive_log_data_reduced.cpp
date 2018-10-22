/* 
 *  Sketch for receiving sensor readings sent from Arduino to RPi
 *  Code for the RPI (receiver)
 *  Code written by ghjalmar
 * 			 - 
 *  based on example code by TMRh20 and Maniacbug (authors of nRF24 lib)
 *  
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 

// Set up and configure the nRF24L01 communication module
using namespace std;
// RPI Pin configuration
RF24 radio(22,0);

// Assign a unique identifier for this node, 0 or 1
bool radioNumber = 1;

// Radio pipe addresses for the 2 nodes to communicate.
const uint8_t pipes[][6] = {"1Node","2Node"};

// Define the callback function for the sqlite_exec function
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

// Defining an callback function for reading values from the DB
// NOT CURRENTLY USED
static int callback2(void *data, int argc, char **argv, char **azColName){
   // Taken from Tutorials point guide, prints out the results from the query
   printf("argv %s = %s\n", azColName[argc-1], argv[argc-1] ? argv[argc-1] : "NULL");
   //db_counter = (int)*argv[argc-1] - '0';
   //printf ("db_counter: %d \n", db_counter);
   return 0;
}

int log_to_db(float *next_data, string *column) {
	// THE MAIN FUNCTION USED FOR LOGGING IN THE DATABASE
	// TAKES AS INPUT POINTER TO THE STRING DEFINING WHAT FIELD IT IS 
	// SUPPOSED TO LOG AND POINTER TO THE VALUE TO LOG 
   // Properties for the SQLite database connection
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   std::string sql;

   /* Open database */
   rc = sqlite3_open("/home/pi/DataBase1.db", &db);
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }
	
   
   // Create the SQL statement for insertion
   sql = "INSERT INTO table3("+ *column + ") VALUES ("+ std::to_string(*next_data)+ ")"; //   + std::to_string(db_counter) + ","  + ", datetime('now'))"
   const char * c = sql.c_str();
   // Execute the SQL statement 
   rc = sqlite3_exec(db, c, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Records created successfully\n");
      sqlite3_close(db);
      return 1;
   }
   sqlite3_close(db);
   return 0;
}

int check_data(float *read_data){
	// Function that reads the received data and determines what field it is
	// Returns 1 if the readings are successfull, 0 if not 
	std::string column;
	if((*read_data == -10000) || (*read_data == -10001) || (*read_data == -10002) || (*read_data == -10003)){
		// the received package was an indicator message
		return 1;
	} else{
		// not able to determine what field the received data is
		return 0;
	}
}

int check_column(float *indicator, std::string *column){
	// From temp_val, determine what field was in the previous package
	if(*indicator == -10000) {
		// temperature
		*column = "temp";
	} else if(*indicator == -10001){
		// pressure
		*column = "pressure";
	} else if(*indicator == -10002){
		// height
		*column = "height";
	} else if(*indicator == -10003){
		// humidity
		*column = "humidity";
	} else{
		// not able to determine what field the received data is
		// something wrong
		printf("Should be able to determine field but wasn't \n");
		return 0;
	}
	return 1;
}

int main(int argc, char** argv){
	// Configuring the communications module
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
	// Variables used in the while loop
	int counter_temp = 100;
	float indicator = 0;
	std::string column;
	// forever loop the listening
	while (1)
	{
		//  Receive each packet, dump it out, and send it back
		// see if there is data ready
		
		if ( radio.available())
		{
			// Dump the payloads until we've gotten everything
			float read_data;

			// Fetch the payload, and see if this was the last one.
			while(radio.available()){
				radio.read( &read_data, sizeof(float) );
			}
			radio.stopListening(); // data package received
			radio.write( &read_data, sizeof(float));
			// Spew it
			printf("Got payload(%d) %f...\n",sizeof(float), read_data);
			
			// log to db if possible
			int status = check_data(&read_data);
			printf("check_data resulted in status = %d \n", status);
			if (status == 1){
				// The received package was an indicator
				indicator = read_data;
				counter_temp = 0;				
			} else if((status == 0) && (counter_temp == 1)){
				// Previous package was an indicator and the current package
				// should be a value to log
				if(check_column(&indicator, &column) == 1){
					// Call log_to_db
					printf("calling log_to_db with data (%d) %f...\n",sizeof(float), read_data);
					log_to_db(&read_data, &column);
				}
			} else{
				printf("Did not receive an indicator and/or counter_temp not 0 \n");
				printf("Read data: %f status : %d  counter temp : %d \n", read_data, status, counter_temp);
				}
			// Signal for next package
			counter_temp = counter_temp + 1;
			// Now, resume listening so we catch the next packets.
			
			radio.startListening();
			
			delay(925); //Delay after payload responded to, minimize RPi CPU time
			
		}
		


	} // forever loop

	return 0;
}
