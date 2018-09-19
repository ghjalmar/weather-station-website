# test script for reading and plotting from database

import sqlite3
import matplotlib.pyplot as p
import matplotlib.dates as d

conn = sqlite3.connect('/home/pi/TestDB.db')

print("Opened database")

temp = []
date = []
# id = []

records = conn.execute('SELECT temp, dt as "[timestamp]", ID from tafla1 where temp not null order by dt')

for row in records:
	temp.append(row[0]) 
	date.append(row[1])
	#id.append(row[2])
# https://stackoverflow.com/questions/18535662/matplotlib-how-to-convert-dates-from-sqlite-to-matplotlib-format
#print(type(d.datetime.strptime(date[0], '%Y-%m-%d %H:%M:%S')))
print("Number of temp records: ", len(temp))
print("Number of date records: ", len(date))
print("Operation done successfully")
conn.close()

p.plot(temp)
p.show()
