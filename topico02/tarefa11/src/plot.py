import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import os

output_dir = "../assets"
os.makedirs(output_dir, exist_ok=True)

df = pd.read_csv("resultados.csv")

# Gráfico: Velocidade do fluido parado e perturbado
def grafico_velocidade(filename, title):
    data = np.loadtxt(filename)
    x, y, u = data[:,0], data[:,1], data[:,2]
    N = int(np.sqrt(len(u)))
    u = u.reshape((N, N))

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    X, Y = np.meshgrid(np.arange(N), np.arange(N))
    ax.plot_surface(X, Y, u, cmap='viridis')
    ax.set_title(title)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Velocidade')
    plt.savefig(os.path.join(output_dir, f"grafico_{title.replace(' ', '_').lower()}.png"))

grafico_velocidade("velocidade_parado.dat", "Campo de Velocidade - Fluido Parado")
grafico_velocidade("velocidade_perturbado.dat", "Campo de Velocidade - Após Perturbação")

# Gráfico: Tempo vs Chunk Size (por schedule, collapse, estado_inicial)
sns.set(style="whitegrid")
for estado in df['estado_inicial'].unique():
    for schedule in df['schedule'].unique():
        plt.figure(figsize=(10, 6))
        for collapse in sorted(df['collapse'].unique()):
            subset = df[(df['estado_inicial'] == estado) &
                        (df['schedule'] == schedule) &
                        (df['collapse'] == collapse)]
            plt.plot(subset['chunk_size'], subset['tempo'], marker='o', label=f"collapse={collapse}")
        plt.title(f"Tempo vs Chunk Size ({schedule}, {estado})")
        plt.xlabel("Chunk Size")
        plt.ylabel("Tempo (s)")
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        filename = f"tempo_chunk_{schedule}_{estado}.png"
        plt.savefig(os.path.join(output_dir, filename))
        plt.close()

# Gráfico: Tempo médio por Schedule e Estado Inicial
plt.figure(figsize=(8, 6))
sns.barplot(data=df, x="schedule", y="tempo", hue="estado_inicial", errorbar="ci")
plt.title("Tempo médio por Schedule e Estado Inicial")
plt.xlabel("Schedule")
plt.ylabel("Tempo Médio (s)")
plt.tight_layout()
plt.savefig(os.path.join(output_dir, "tempo_medio_por_schedule.png"))
plt.close()
