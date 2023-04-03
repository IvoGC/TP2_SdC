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
librequests.getPrices.argtypes = (ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_float)
librequests.getPrices.restype = Prices

def request():
	res = requests.get("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=ars,usd,eur")
	if res:
		data = res.json()
		return data
	else:
		print("api error")


def getPrices(toConvert, priceUsd, priceEur, priceArs):
	return librequests.getPrices(toConvert, priceUsd, priceEur, priceArs)


def update():
	global toConvert
	try:
		tC = float(toConvert.get())
		data = request()
		priceUsd = data['bitcoin']['usd']
		priceEur = data['bitcoin']['eur']
		priceArs = data['bitcoin']['ars']

		results = getPrices(tC, priceUsd, priceEur, priceArs)

		label_price.config(text=f"Cotización actual: {round(priceUsd, 3)} USD")
		label_usd.config(text=f"Conversión dolares: {round(results.usd, 3)} USD")
		label_euro.config(text=f"Conversión euros: {round(results.euros, 3)} EUR")
		label_peso.config(text=f"Conversión pesos: {round(results.pesos, 3)} ARS")
	except ValueError:
		print("Conversion error")


root = tk.Tk()
root.title("Calculadora de cripto monedas")
root.geometry('400x600')

bitcoinImg = tk.PhotoImage(file='./Img/bitcoin.png')
label_bitcoinImg = tk.Label(root, image=bitcoinImg)
label_bitcoinImg.place(x=250, y=20)

toConvert = tk.StringVar()
toConvert.set('0')

label = tk.Label(root, text="Ingrese la cantidad a convertir", padx=30, pady=5, width=20)
label.place(x=20, y=30)

entry = tk.Entry(root, textvariable=toConvert)
entry.config(width=10)
entry.place(x=50, y=64)

button = tk.Button(root, text="Convertir", command=update)
button.place(x=150,y=60)

usdImg = tk.PhotoImage(file='./Img/usd.png')
label_usdImg = tk.Label(root, image=usdImg)
label_usdImg.place(x=0, y=130)

label_usd = tk.Label(root, text=f"Conversión dolares: {0.0} USD")
label_usd.place(x=100, y=170)

euroImg = tk.PhotoImage(file='./Img/euro.png')
label_euroImg = tk.Label(root, image=euroImg)
label_euroImg.place(x=0, y=280)

label_euro = tk.Label(root, text=f"Conversión euros: {0.0} EUR")
label_euro.place(x=100, y=320)

pesoImg = tk.PhotoImage(file='./Img/peso.png')
label_pesoImg = tk.Label(root, image=pesoImg)
label_pesoImg.place(x=0, y=430)

label_peso = tk.Label(root, text=f"Conversión pesos: {0.0} ARS")
label_peso.place(x=100, y=470)

label_price = tk.Label(root, text=f"Cotización actual: {0.0} USD")
label_price.place(x=100, y=550)

root.mainloop()

