#!/usr/lib/python3.5
# Code to update the dns of the homepage, to be run regularly using CRON
# Tried doing this first with urllip.request.openurl but then found the requests module
import datetime
import requests
# Ping the url with the api commands
dns_url = "https://api.1984.is/1.0/freedns/?apikey=3bA0fX5uU1oT4tN9dQ0eW5zU3hL3uS6sC3cD9fT9nB0jG8pD9xS7pP5gG4bE7aJ&domain=ghjalmar.is&ip="
contents = requests.get(dns_url)
# read() returns bytes, need to cast them to a string so it's possible to write it to the log file
#contents = contents.decode("utf-8")
#print(contents.text)

# Get the time
now = datetime.datetime.today()
#print(now)

combined_string = str(now) + "|" + contents.text + "\n"

# Open a text file to write a log
path = "/home/pi/Documents/weather-station-website/update_dns_log.txt"
f = open(path, 'a')
f.write(combined_string)
f.close()