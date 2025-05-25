import pandas as pd
import matplotlib.pyplot as plt
import os

output_dir = "../assets"
os.makedirs(output_dir, exist_ok=True)

def plot_escalabilidade(nome_arquivo, titulo):
    df = pd.read_csv(nome_arquivo)
    plt.plot(df["Threads"], df["Time(s)"], marker="o")
    plt.xlabel("Threads")
    plt.ylabel("Tempo (s)")
    plt.title(titulo)
    plt.grid()
    plt.savefig(os.path.join(output_dir, f"grafico_{titulo.replace(' ', '_').lower()}.png"))
    print(f"Gráfico salvo como {titulo.replace(' ', '_') + '.png'}")

plot_escalabilidade("escalabilidade_forte.csv", "Escalabilidade Forte")
plot_escalabilidade("escalabilidade_fraca.csv", "Escalabilidade Fraca")
