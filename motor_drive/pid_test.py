# coding=utf-8
import PID
import MotorDrive
import time
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import spline
import RPi.GPIO as GPIO

global motor_drive

def rotation_vel_test(channel):        
        motor_drive.hower_count = motor_drive.hower_count + 1
        motor_drive.current_time = time.time()
        motor_drive.time_scale = motor_drive.current_time - motor_drive.last_time
        if (motor_drive.time_scale >= 0.1):
                motor_drive.last_time = motor_drive.current_time
                motor_drive.rotation_vel = motor_drive.hower_count / ( 13 * 18.8 *motor_drive.time_scale)
                motor_drive.hower_count = 0

                        
if __name__ == "__main__":
        P = 0.01
        I = 0.01
        D = 0 
        InputDutyRatio = 40 #from 0 to 100
        feedback_list = []
        time_list = []
        setpoint_list = []
        pid = PID.PID(P, I, D)

        motor_drive = MotorDrive.MotorDrive(InputDutyRatio)
        channel = 17
        GPIO.add_event_detect(channel,GPIO.RISING,callback = rotation_vel_test)#detect the positive or negtive of the rotation
        while((motor_drive.current_time - motor_drive.init_time) <= 4):
                motor_drive.current_time = time.time()
                #if (GPIO.event_detected(17)):
                        #motor_drive.rotation_vel_test()
                
                pid.test_pid(InputDutyRatio  , motor_drive.rotation_vel )
                print(motor_drive.rotation_vel)
                feedback_list.append(motor_drive.rotation_vel )
                setpoint_list.append(pid.SetPoint)
                time_list.append(motor_drive.current_time - motor_drive.init_time)


        motor_drive._pwm.stop()
        time_sm = np.array(time_list)
        time_smooth = np.linspace(time_sm.min(), time_sm.max(), 300)
        feedback_smooth = spline(time_list, feedback_list, time_smooth)
        plt.figure(0)
        plt.plot(time_smooth, feedback_smooth)
        plt.plot(time_list, setpoint_list)
        plt.xlim((0, 4))
        plt.ylim((min(feedback_list)-0.5, max(feedback_list)+0.5))
        plt.xlabel('time (s)')
        plt.ylabel('PID (PV)')
        plt.title('TEST PID')

        

        plt.grid(True)
        plt.show()
        #print(feedback_list)

        
