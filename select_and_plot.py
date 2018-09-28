# test script for reading and plotting from database

import sqlite3
import matplotlib.pyplot as p
import matplotlib.dates as dt
import datetime 
conn = sqlite3.connect('/home/pi/TestDB.db')

print("Opened database")

temp = []
date = []
# id = []

#records = conn.execute('SELECT temp, dt as "[timestamp]", ID from tafla1 where temp not null order by dt')
records = conn.execute('Select temp, dt as "[timestamp]", ID from tafla1 where ((dt between "2018-09-19 00:00:00" and datetime("now","localtime")) and (temp not null))');

for row in records:
	temp.append(row[0]) 
	date.append(row[1])
	#id.append(row[2])

x = [datetime.datetime.strptime(d, '%Y-%m-%d %H:%M:%S') for d in date]
#dates = [dt.date2num(d) for d in date]
# https://stackoverflow.com/questions/18535662/matplotlib-how-to-convert-dates-from-sqlite-to-matplotlib-format
#print(type(d.datetime.strptime(date[0], '%Y-%m-%d %H:%M:%S')))
print("Number of temp records: ", len(temp))
print("Number of date records: ", len(date))
print("Operation done successfully")
conn.close()


p.plot(x,temp)
p.show()
