#!/usr/lib/python3.5
#!/bin/env python
# test script for reading and plotting from database

import sqlite3
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as p
import matplotlib.dates as dt
import datetime 
conn = sqlite3.connect('/home/pi/DataBase1.db')

#print("Opened database")

temp = []
humidity = []
pressure = []
t_date = []
h_date = []
p_date = []
# id = []

#records = conn.execute('SELECT temp, dt as "[timestamp]", ID from tafla1 where temp not null order by dt') 
temp_records = conn.execute('Select temp, dt as "[timestamp]", ID from table1 where ((dt between datetime("now", "-2 days") and datetime("now")) and (temp not null))');
humidity_records = conn.execute('Select humidity, dt as "[timestamp]", ID from table1 where ((dt between datetime("now", "-2 days") and datetime("now")) and (humidity not null))');
pressure_records = conn.execute('Select pressure, dt as "[timestamp]", ID from table1 where ((dt between datetime("now", "-2 days") and datetime("now")) and (pressure not null))');
# Store the values to lists
for row in temp_records:
	temp.append(row[0])
	t_date.append(row[1])
for row in humidity_records:
	humidity.append(row[0])
	h_date.append(row[1])
for row in pressure_records:
	pressure.append(row[0])
	p_date.append(row[1])

x_t = [datetime.datetime.strptime(d, '%Y-%m-%d %H:%M:%S') for d in t_date]
x_h = [datetime.datetime.strptime(d, '%Y-%m-%d %H:%M:%S') for d in h_date]
x_p = [datetime.datetime.strptime(d, '%Y-%m-%d %H:%M:%S') for d in p_date]

#dates = [dt.date2num(d) for d in date]
# https://stackoverflow.com/questions/18535662/matplotlib-how-to-convert-dates-from-sqlite-to-matplotlib-format
#print(type(d.datetime.strptime(date[0], '%Y-%m-%d %H:%M:%S')))
#print("Number of temp records: ", len(temp))
#print("Number of date records: ", len(t_date))
#print("Operation done successfully")
conn.close()

fig, ax = p.subplots()
ax.plot(x_t,temp)
p.title('Temperature')
ax.set_ylabel('[°C]')
#p.show()
p.savefig('/home/pi/Documents/weather-station-website/temp_plot.png')

fig, ax1 = p.subplots()

color = 'tab:red'
ax1.set_ylabel('Humidity [%]', color=color)
ax1.plot(x_h, humidity, color=color)
ax1.tick_params(axis='y', labelcolor=color)

ax2 = ax1.twinx()

color = 'tab:blue'
ax2.set_ylabel('Pressure [kPa]', color=color)
ax2.plot(x_p, pressure, color=color)
ax2.tick_params(axis='y', labelcolor=color)
p.title("Humidity and pressure")
fig.tight_layout()
p.savefig('/home/pi/Documents/weather-station-website/humidity_pressure_plot.png')
#p.show()