import tkinter as tk
import requests
import ctypes

class Prices(ctypes.Structure):
    _fields_ = [
        ('usd', ctypes.c_float),
        ('euros', ctypes.c_float),
        ('pesos', ctypes.c_float),
    ]

librequests = ctypes.CDLL("./lib/librequests.so")
librequests.getPrices.restype = Prices

def getPrices():
	return librequests.getPrices()


def update():
	
	results = getPrices()

	label_usd.config(text=f"Conversión dolares: {results.usd} USD")
	label_euro.config(text=f"Conversión euros: {results.euros} EUR")
	label_peso.config(text=f"Conversión pesos: {results.pesos} ARS")

	root.after(1,update)


root = tk.Tk()
root.title("Calculadora de cripto monedas")
root.geometry('400x600')


bitcoinImg = tk.PhotoImage(file='./Img/bitcoin.png')
label_bitcoinImg = tk.Label(root, image=bitcoinImg)
label_bitcoinImg.place(x=150, y=20)

usdImg = tk.PhotoImage(file='./Img/usd.png')
label_usdImg = tk.Label(root, image=usdImg)
label_usdImg.place(x=0, y=150)

label_usd = tk.Label(root, text=f"Conversión dolares: {0} USD")
label_usd.place(x=100, y=180)

euroImg = tk.PhotoImage(file='./Img/euro.png')
label_euroImg = tk.Label(root, image=euroImg)
label_euroImg.place(x=0, y=300)

label_euro = tk.Label(root, text=f"Conversión euros: {0} USD")
label_euro.place(x=100, y=330)

pesoImg = tk.PhotoImage(file='./Img/peso.png')
label_pesoImg = tk.Label(root, image=pesoImg)
label_pesoImg.place(x=0, y=450)

label_peso = tk.Label(root, text=f"Conversión pesos: {0} USD")
label_peso.place(x=100, y=480)


root.after(1,update)
root.mainloop()

