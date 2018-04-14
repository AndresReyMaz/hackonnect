# -*- coding: utf-8 -*-
#serial communication for arduino
import serial 
#delay utility
import time
#used for sending http requests with data read from the arduino
import requests
#used for parsing json response
import json

ser = serial.Serial('/dev/cu.usbmodem1421',9600)
print("Service making request...")
size = 3
arr = [10,22,31]
url =  "http://ec2-18-188-150-180.us-east-2.compute.amazonaws.com:8080/init/" + str(size)
response = requests.get(url)

print("ARR:")
print(response.text)

data = json.loads(response.text)
arr = data['randoms']

print("Now wait for serial")

while ser.in_waiting == 0:
    pass

string = ser.readline().rstrip()
print("Fist:")
print(string)



for i in arr:
    ser.write(str(i))
    string = ser.readline().rstrip()
    print(str(i)+":"+string)


print("starting infinite loop")
while 1:
    while ser.in_waiting == 0:
        pass
    string = ser.readline().rstrip()
    index = 0
    count = 0
    for i in arr:
        if i == int(string):
            index = count
        count+=1

    url = "http://ec2-18-188-150-180.us-east-2.compute.amazonaws.com:8080/" + string
    response = requests.get(url)
    data = json.loads(response.text)
    arr[index] = int(data['id'])
    ser.write(str(arr[index]))
    string = ser.readline().rstrip()
    print(arr)






