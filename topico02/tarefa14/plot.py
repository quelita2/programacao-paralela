import pandas as pd
import matplotlib.pyplot as plt

# Lê o arquivo CSV
df = pd.read_csv("tempo_mensagens.csv")

# Plot com escala log-log
plt.figure(figsize=(8,6))
plt.plot(df["Tamanho (bytes)"], df["Tempo medio (ms)"], marker='o', linestyle='-')
plt.xscale('log')
plt.yscale('log')
plt.xlabel("Tamanho da Mensagem (bytes)")
plt.ylabel("Tempo Médio por Troca (ms)")
plt.title("Tempo de Comunicação MPI vs Tamanho da Mensagem")
plt.grid(True, which="both", ls="--", linewidth=0.5)
plt.tight_layout()
plt.savefig("grafico_mpi.png", dpi=300)
plt.show()
