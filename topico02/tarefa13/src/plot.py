import pandas as pd
import matplotlib.pyplot as plt

# Carrega os dados do CSV
df = pd.read_csv("resultados_afinidade.csv")

# Converte os dados para tipos apropriados
df["Threads"] = df["Threads"].astype(int)
df["Tempo(s)"] = df["Tempo(s)"].astype(float)

# Lista de políticas de OMP_PROC_BIND únicas
bind_policies = df["OMP_PROC_BIND"].unique()

# Cria o gráfico
plt.figure(figsize=(10, 6))

for bind in bind_policies:
    subset = df[df["OMP_PROC_BIND"] == bind]

    # Agrupa por número de threads e calcula média dos tempos (para diferentes OMP_PLACES)
    avg_times = subset.groupby("Threads")["Tempo(s)"].mean()

    plt.plot(avg_times.index, avg_times.values, marker='o', label=f'OMP_PROC_BIND={bind}')

# Personalização do gráfico
plt.title("Tempo de Execução vs. Número de Threads\n(Diferentes OMP_PROC_BIND)")
plt.xlabel("Número de Threads")
plt.ylabel("Tempo de Execução (s)")
plt.grid(True)
plt.legend()
plt.tight_layout()

# Salva e mostra o gráfico
plt.savefig("grafico_omp_proc_bind.png")
