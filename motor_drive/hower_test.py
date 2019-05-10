import RPi.GPIO as GPIO
import time
import MotorDrive

if __name__ == "__main__":
	motor_drive = MotorDrive.MotorDrive(20)
	while( (motor_drive.current_time - motor_drive.init_time) < 5 ):
		motor_drive.rotation_vel_test()
		print(motor_drive.rotation_vel)
		
	motor_drive._pwm.stop()
	
