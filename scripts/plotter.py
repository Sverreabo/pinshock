import matplotlib.pyplot as plt
from csv_handler import read_data

def running_avg(liste, n):
    ny_liste = []

    for i in range(n, len(y_verdier) - n):
        verdier = y_verdier[i-n:i+n]
        ny_liste.append(sum(verdier) / len(verdier))
    
    return ny_liste
    
    
x_verdier, y_verdier = read_data("data.csv")

index_min = min(range(len(y_verdier)), key=y_verdier.__getitem__)
print(x_verdier[index_min], ":", y_verdier[index_min])


N = 0
if N > 0:
    nye_yverdier = running_avg(y_verdier, N)

    plt.title("Running AVG")
    plt.plot(x_verdier[N:-N], nye_yverdier)

else:
    plt.plot(x_verdier, y_verdier)

plt.show()
