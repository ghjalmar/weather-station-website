import sqlite3
# Connect to the database
conn = sqlite3.connect('/home/pi/DataBase1.db')
c = conn.cursor()
c.execute('''CREATE TABLE table3 (ID INTEGER PRIMARY KEY, temp REAL, pressure REAL, height REAL, humidity REAL, dt DATETIME DEFAULT (datetime('now', 'localtime')));''')
conn.commit()
conn.close()
# 