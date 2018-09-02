# test script for reading and plotting from database

import sqlite3
from matplotlib.pyplot import *

conn = sqlite3.connect('/home/pi/test1.db')

print("Opened database")

humidity = []
date = []

records = conn.execute("SELECT humidity, date from tafla1 where humidity >=0 and date >= 0")

for row in records:
	humidity.append(row[0]) 
	date.append(row[1]) 

print("Number of humidity records: ", len(humidity))
print("Number of date records: ", len(date))
print("Operation done successfully")
conn.close()

plot(date, humidity)
