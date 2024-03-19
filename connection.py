import time
import serial
import os
import tkinter as tk
from tkinter import messagebox


class Gui:
    def __init__(self,ser):
        self.ser = ser
        self.root = tk.Tk()
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.root.mainloop()
    def on_closing(self):
        if messagebox.askyesno("Quit", message="Do you want to quit program?"):
            self.ser.close()
            self.root.destroy()           


os.system("./connect.sh")
ser = serial.Serial(port='/dev/rfcomm0', baudrate=9600)
time.sleep(2)

while True:
    value = ser.readlines()
    valueInString = str(value, 'UTF-8')
    print(valueInString)

print(ser.name)
ser.write(b'hello')
ser.close()



