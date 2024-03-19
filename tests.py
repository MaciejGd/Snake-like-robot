import tkinter as tk
from tkinter import messagebox

#list of baudrates
baud_rates=[110, 300, 600, 1200, 2400, 4800, 9600,14400,19200,38400,57600,115200,128000,256000]

baud_rate_val = 9600
is_connected = False

def update_textbox(textbox, data):
    textbox.config(state=tk.NORMAL) # unlock the textbox
    textbox.insert(tk.END, "\n"+str(data)) # add newline and append the data
    textbox.config(state=tk.DISABLED) # lock back the textbox to readonly
    textbox.see(tk.END) 

class Gui:
    def __init__(self):
        self.root = tk.Tk()
        self.root.geometry('800x600')
        self.root.title("Snake control app")
        self.root.configure(bg="black")

        self.menubar = tk.Menu(self.root)
        self.menubar.configure(bg="black", fg="white")
        self.console_menu = tk.Menu(self.menubar, tearoff=0)
        self.console_menu.add_command(label="Clear", command=self.clear)
        self.menubar.add_cascade(menu=self.console_menu, label="Output")
        
        #selecting baudrate you want
        self.variable = tk.StringVar(self.root)
        self.variable.set(baud_rates[6])
        self.baud_menu = tk.OptionMenu(self.root, self.variable, *baud_rates, command=self.ok)
        self.baud_menu.pack()

        self.root.config(menu=self.menubar)

        self.start_button = tk.Button(self.root, text="START", command=lambda: self.show_message("Snake start request send"))
        self.start_button.configure(bg="#00ffff", fg="blue")
        self.start_button.place(x=100, y=100)

        self.stop_button = tk.Button(self.root, text="Stop", command=lambda: self.show_message("Snake stop request send"))
        self.stop_button.configure(bg="#00ffff", fg="blue")
        self.stop_button.place(x=200, y=100)

        self.consoleBox = tk.Text(self.root, height=10, width=760, bg="white")
        self.consoleBox.place(x=5, y=400)
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
        messagebox.showinfo(message=prompt)
        update_textbox(self.consoleBox, prompt)
        update_textbox(self.consoleBox, "Waiting for MCU response...")

    def clear(self):
        self.consoleBox.config(state=tk.NORMAL)
        self.consoleBox.delete("1.0",tk.END)
        self.consoleBox.config(state=tk.DISABLED)
    
    def establish_connection(self):
        self.baud_menu.configure(state="disabled")
Gui()
