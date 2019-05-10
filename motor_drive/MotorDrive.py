import RPi.GPIO as GPIO
import time

class MotorDrive:
        def __init__(self,duty_ratio):
        
                self.rotation_vel = 0
                self.hower_count = 0
                self.time_scale = 0.0
                self.duty_ratio = duty_ratio
                GPIO.setmode(GPIO.BCM)
                GPIO.setwarnings(False)
                GPIO.setup( 2 , GPIO.OUT) #PWM
                GPIO.setup( 3 , GPIO.OUT) #IN1
                GPIO.setup( 4 , GPIO.OUT) #IN2
                GPIO.setup( 17 , GPIO.IN )#Hower_phase_A
                GPIO.setup( 27 , GPIO.IN )#Hower_phase_B

                self._pwm = GPIO.PWM(2,100)
                self._pwm.start(duty_ratio)
                GPIO.output(3,GPIO.HIGH)
                GPIO.output(4,GPIO.LOW)
                self.current_time = time.time()
                self.last_time = self.current_time
                self.init_time = time.time()
                #GPIO.add_event_detect(17,GPIO.RISING , callback = self.rotation_vel_test)#detect the positive or negtive of the rotation
'''
        def rotation_vel_test(self):        
                self.hower_count = self.hower_count + 1
                self.current_time = time.time()
                self.time_scale = self.current_time - self.last_time
                if (self.time_scale >= 0.1):
                        self.last_time = self.current_time
                        self.rotation_vel = self.hower_count / ( 13 * self.time_scale)
                        self.hower_count = 0
'''
