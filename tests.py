import os
import tkinter as tk
from tkinter import messagebox
import serial

#list of baudrates
baud_rates=[110, 300, 600, 1200, 2400, 4800, 9600,14400,19200,38400,57600,115200,128000,256000]

baud_rate_val = 9600
is_connected = False
com = "/dev/rfcomm0"

def update_textbox(textbox, data):
    textbox.config(state=tk.NORMAL) # unlock the textbox
    textbox.insert(tk.END, "\n"+str(data)) # add newline and append the data
    textbox.config(state=tk.DISABLED) # lock back the textbox to readonly
    textbox.see(tk.END) 

def send_data(serial_port, data_code):
    serial_port.write(data_code)

class Gui:
    def __init__(self):
        self.root = tk.Tk()
        self.root.geometry('800x500')
        self.root.title("Snake control app")
        self.root.configure(bg="black")

        self.menubar = tk.Menu(self.root)
        self.menubar.configure(bg="black", fg="white")
        self.console_menu = tk.Menu(self.menubar, tearoff=0)
        self.console_menu.add_command(label="Clear", command=self.clear)
        self.menubar.add_cascade(menu=self.console_menu, label="Output")
        self.root.config(menu=self.menubar)

        #line between connection and movement
        self.canvas = tk.Canvas(self.root, width=2, height=195)
        self.canvas.place(x=399, y=10)
        
        #MOVEMENT WINDOW
        #frame for stop start buttons
        self.move_label = tk.Label(self.root, text="MOVEMENT CONTROL", font=12)
        self.move_label.place(x=510, y=10)
        self.move_label.configure(bg="black", fg="white")

        self.start_button = tk.Button(self.root, text="START", font=16,command=lambda: self.show_message("Snake start request send"))
        self.start_button.configure(bg="#0C7D09", fg="#16FE10", width=6,height=3)
        self.start_button.bind("<Button-3>", lambda event: self.right_click("Button which tells snake robot to start moving"))
        self.start_button.place(x=650, y=100)

        self.stop_button = tk.Button(self.root, text="STOP", font=16,command=lambda: self.show_message("Snake stop request send"))
        self.stop_button.configure(bg="#BF0F0F", fg="#F71923", width=6, height=3)
        self.stop_button.bind("<Button-3>", lambda event: self.right_click("Button which tells snake robot to stop moving"))
        self.stop_button.place(x=550, y=100)
        
        #self.com_box = tk.Text(self.root, height=1, width=15, bg="white")
        self.stretch_button = tk.Button(self.root, text="STRETCH", font=16, command=lambda: self.show_message("All servos set to 0 dergees"))
        self.stretch_button.configure(bg="#00ffff", fg="blue", width=6, height=3)
        self.stretch_button.bind("<Button-3>", lambda event: self.right_click("Set all servos in robot to zero degrees"))
        self.stretch_button.place(x=450, y=100)

        #things responsible for settings before establishing connection with module
        #CONNECTION SETTINGS WINDOW
        self.conn_label = tk.Label(self.root, text="CONNECTION SETTINGS", font=12)
        self.conn_label.configure(bg="black",fg="white")
        self.conn_label.place(x=100, y=10)


        self.baud_label = tk.Label(self.root, text="Baudrate")
        self.baud_label.configure(bg="black",fg="white")
        self.baud_label.place(x=250, y=53)

        self.variable = tk.StringVar(self.root)
        self.variable.set(baud_rates[6])
        self.baud_menu = tk.OptionMenu(self.root, self.variable, *baud_rates, command=self.ok)
        self.baud_menu.config(width=5, height=1)
        self.baud_menu.place(x=250, y=78)

        self.baud_label = tk.Label(self.root, text="Serial Port")
        self.baud_label.configure(bg="black",fg="white")
        self.baud_label.place(x=50, y=53)

        self.com_box = tk.Text(self.root, height=1, width=15, bg="white")
        self.com_box.place(x=50, y=81)
        self.com_box.insert(tk.END, com) 
        self.com_box.bind("<Return>", self.change_com)

        self.connect_button = tk.Button(self.root, width=30, text="Connect", font=16, command=self.establish_connection)
        self.connect_button.place(x=34, y=135)

        self.disconnect_button = tk.Button(self.root, width=30, text="Disconnect", font=16, command=self.lose_connection)
        self.disconnect_button.place(x=34, y=175)
        #END OF CONNECTION SETTINGS WINDOW

        self.consoleBox = tk.Text(self.root, height=15, width=95, bg="white")
        self.consoleBox.place(x=15, y=220)
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.root.mainloop()

    def ok(self, selected_arg):
        if not is_connected:
            global baud_rate_val
            update_textbox(self.consoleBox, "Value of baudrate changed from "+ str(baud_rate_val) + " to " + str(selected_arg))
            baud_rate_val = self.variable.get()

    def on_closing(self):
        if messagebox.askyesno("Quit", message="Do you want to quit program?"):
            self.root.destroy()

    def show_message(self, prompt):
        if is_connected:
            update_textbox(self.consoleBox, prompt + "\nWaiting for MCU response...")
        else:
            update_textbox(self.consoleBox, "Connection has not been set")

    def right_click(self, msg):
        messagebox.showinfo(message=msg)

    def clear(self):
        self.consoleBox.config(state=tk.NORMAL)
        self.consoleBox.delete("1.0",tk.END)
        self.consoleBox.config(state=tk.DISABLED)
    
    def establish_connection(self):
        global is_connected
        if not is_connected:
            self.baud_menu.configure(state="disabled")
            self.com_box.configure(state="disabled")
            update_textbox(self.consoleBox, "Trying to establish connection with snake's head")
            is_connected = True
        #another things to do when establishing connection
            
    def lose_connection(self):
        global is_connected
        is_connected = False
        self.baud_menu.configure(state="active")
        self.com_box.configure(state="normal")
        update_textbox(self.consoleBox, "Successfully disconected from external device")

    def change_com(self, event):
        global com
        if os.path.exists(self.com_box.get("1.0",tk.END).strip()):
            com = self.com_box.get("1.0", tk.END).strip()
            update_textbox(self.consoleBox, "Path successfully changed")
            print(com)
        else:
            update_textbox(self.consoleBox, "There is no such path in the system, specify correct path")
            self.com_box.delete("1.0", tk.END)
        return "break"
my_gui = Gui()