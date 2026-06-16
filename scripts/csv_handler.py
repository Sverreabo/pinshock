import csv

def write_data(x_verdier, y_verdier, filnavn="data.csv"):

    with open(filnavn, "w", encoding="utf-8") as f:
        skriver = csv.writer(f, delimiter=";", )
        skriver.writerows(zip(x_verdier, y_verdier))

def read_data(filnavn="data.csv"):
    x_verdier = []
    y_verdier = []

    with open(filnavn, "r", encoding="utf-8") as f:
        leser = csv.reader(f, delimiter=";")
        for rad in leser:
            x_verdier.append(float(rad[0]))
            y_verdier.append(float(rad[1]))

    return x_verdier, y_verdier
