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
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 

// Set up and configure the nRF24L01 communication module
using namespace std;
// RPI Pin configuration
RF24 radio(22,0);

// Assign a unique identifier for this node, 0 or 1
bool radioNumber = 1;
bool global_bool = 1;
// Radio pipe addresses for the 2 nodes to communicate.
const uint8_t pipes[][6] = {"1Node","2Node"};

// Define a global int that keep tracks of number of logs to the DB and 
// is the primary key in the database
//int db_counter = 0;

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
static int callback2(void *data, int argc, char **argv, char **azColName){
   // Taken from Tutorials point guide
   printf("argv %s = %s\n", azColName[argc-1], argv[argc-1] ? argv[argc-1] : "NULL");
   //db_counter = (int)*argv[argc-1] - '0';
   //printf ("db_counter: %d \n", db_counter);
   return 0;
}

int log_to_db(float *next_data, string *column) {
   // Properties for the SQLite database connection
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   //int curr_key[2];
   std::string sql;

   /* Open database */
   rc = sqlite3_open("/home/pi/TestDB.db", &db);
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }
	
   
   // Extract current unique key
   sql = "select max(id) from tafla1";
   // cast the std::string to const char* for the SQL function
   const char * c = sql.c_str();
   rc = sqlite3_exec(db, c, callback2, 0, &zErrMsg);
   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      sqlite3_close(db);
      return 0;
   } else {
      fprintf(stdout, "Unique key read successfully\n");
   }
   // Increment the unique key
   //db_counter++;
   // Create the SQL statement for insertion
   sql = "INSERT INTO tafla1("+ *column + ") VALUES ("+ std::to_string(*next_data)+ ")"; //   + std::to_string(db_counter) + ","  + ", datetime('now'))"
   c = sql.c_str();
   // Execute the SQL statement 
   rc = sqlite3_exec(db, c, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Records created successfully\n");
      return 1;
   }
   sqlite3_close(db);
   return 0;
}

int check_data(float *read_data){
	// Function that reads the sent data and determines what field it is
	// Returns 1 if the readings are successfull, 0 if not 
	std::string column;
	if(*read_data == -10000) {
		// temp
		column = "temp";
	} else if(*read_data == -10001){
		// pressure
		column = "pressure";
	} else if(*read_data == -10002){
		// height
		column = "height";
	} else if(*read_data == -10003){
		// humidity
		column = "humidity";
	} else{
		// listen for next package
		return 0;
	}
	// listen for next package
	bool nested_bool = true;
	float next_data;
	radio.startListening();
	while (nested_bool ){
		if ( radio.available() )	{
			while(radio.available()){
				radio.read( &next_data, sizeof(float) );
			}
			nested_bool = false;
			radio.stopListening(); // data package received	
			radio.write( &next_data, sizeof(float));
			printf("Got next_data payload(%d) %f...\n",sizeof(float), next_data);
		}
	}
	// next_data should be positive, return 0 if not
	if (next_data < 0){
		return 0;
	} else{
		// Call log_to_db
		printf("calling log_to_db with next_data (%d) %f...\n",sizeof(float), next_data);
		int status = log_to_db(&next_data, &column);
		if (status == 1){
			return 1;
		} else{
			return 0;}
	}
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

	// forever loop the listening
	while (1)
	{
		//  Receive each packet, dump it out, and send it back
			
		// see if there is data ready
		if ( radio.available() && global_bool)
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
			global_bool = false;
			int status = check_data(&read_data);
			if (status == 1){
				printf("Check data function logged data \n");
			} else{
				printf("Check data function failed \n");
				//radio.startListening();
				}
			
			// Now, resume listening so we catch the next packets.
			global_bool = true;
			radio.startListening();
			
			delay(925); //Delay after payload responded to, minimize RPi CPU time
			
		}
		


	} // forever loop

	return 0;
}
