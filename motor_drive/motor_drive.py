#the ctr_pwm outputs of four motor on BCM are :2 14 13 16
#the A B phase of motor on BCM are :3,4  15,18  19,26	20,21
#the input of hower on BCM are :   


import RPi.GPIO as GPIO
import time
import ROS


GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

#set the ctr_pwm output on BCM mode
#The first is the output PWM ,the second and third is the output A and B
#front_left
GPIO.setup( 2 , GPIO.OUT) #PWM
GPIO.setup( 3 , GPIO.OUT) #A
GPIO.setup( 4 , GPIO.OUT) #B
#front_right
GPIO.setup( 14 , GPIO.OUT)#pwm
GPIO.setup( 15 , GPIO.OUT)#a
GPIO.setup( 18 , GPIO.OUT)#b
#behind_left
GPIO.setup( 13 , GPIO.OUT)#pwm
GPIO.setup( 19 , GPIO.OUT)#a
GPIO.setup( 26 , GPIO.OUT)#b
#behind_right
GPIO.setup( 16 , GPIO.OUT)#pwm
GPIO.setup( 20 , GPIO.OUT)#a
GPIO.setup( 21 , GPIO.OUT)#b

#four wheels inputs of hower
#front_left
GPIO.setup( 17 , GPIO.IN , pull_up_down = GPIO.PUD_UP) 
#front_right
GPIO.setup( 23 , GPIO.IN , pull_up_down = GPIO.PUD_UP)
#behind_left
GPIO.setup( 6 ,  GPIO.IN , pull_up_down = GPIO.PUD_UP)
#behind_right
GPIO.setup( 12 , GPIO.IN , pull_up_down = GPIO.PUD_UP)

#the init paulse num of the wheels
counter_front_left = 0 
counter_front_right = 0
counter_behind_left = 0
counter_behind_right = 0

global M_time_init = time.time()

def wheel_front_left_callback(17):
	global counter_front_left
	if GPIO.event_detected(17): 
		counter_front_left+=1

def wheel_front_right_callback(23):
	global counter_front_right
	if GPIO.event_detected(23):
		counter_front_right+=1

def wheel_behind_left_callback(6):
	global counter_behind_left
	if GPIO.event_detected(6):
		counter_behind_left+=1

def wheel_behind_right_callback(12):
	global counter_behind_left
	if GPIO.event_detected(12):
		counter_behind_right+=1

while True:
	GPIO.add_event_detect( 17 , GPIO.RISING , callback = wheel_front_left_callback)
	GPIO.add_event_detect( 23 , GPIO.RISING , callback = wheel_front_right_callback)
	GPIO.add_event_detect(  6 , GPIO.RISING , callback = wheel_behind_left_callback)
	GPIO.add_event_detect( 12 , GPIO.RISING , callback = wheel_behind_right_callback)
	if ( (time.time() - M_time_init) = 1):
		



