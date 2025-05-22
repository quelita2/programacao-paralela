import pandas as pd
import matplotlib.pyplot as plt

def plot_escalabilidade(nome_arquivo, titulo):
    df = pd.read_csv(nome_arquivo)
    plt.plot(df["Threads"], df["Time(s)"], marker="o")
    plt.xlabel("Threads")
    plt.ylabel("Tempo (s)")
    plt.title(titulo)
    plt.grid()
    plt.savefig(titulo.replace(" ", "_") + ".png")
    print(f"Gráfico salvo como {titulo.replace(' ', '_') + '.png'}")

plot_escalabilidade("escalabilidade_forte.csv", "Escalabilidade Forte")
plot_escalabilidade("escalabilidade_fraca.csv", "Escalabilidade Fraca")
