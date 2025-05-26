import pandas as pd
import matplotlib.pyplot as plt

# Carrega os dados do CSV
df = pd.read_csv("resultados.csv")

# Remove linhas com tempo vazio (caso algum teste falhou ou não foi preenchido)
df = df.dropna(subset=["tempo"])

# Converte colunas para os tipos corretos
df["processos"] = df["processos"].astype(int)
df["M"] = df["M"].astype(int)
df["tempo"] = df["tempo"].astype(float)

# Plot 1: Tempo vs Tamanho da Matriz para cada número de processos
plt.figure(figsize=(10, 6))
for proc in sorted(df["processos"].unique()):
    subset = df[df["processos"] == proc]
    plt.plot(subset["M"], subset["tempo"], marker='o', label=f"{proc} processos")

plt.title("Tempo de Execução vs Tamanho da Matriz")
plt.xlabel("Tamanho da matriz (M=N)")
plt.ylabel("Tempo (s)")
plt.grid(True)
plt.tight_layout()
plt.savefig("tempo_vs_tamanho.png")

# Plot 2: Tempo vs Número de Processos para cada tamanho de matriz
# plt.figure(figsize=(10, 6))
# for tam in sorted(df["M"].unique()):
#     subset = df[df["M"] == tam]
#     plt.plot(subset["processos"], subset["tempo"], marker='o', label=f"{tam}x{tam}")

# plt.title("Tempo de Execução vs Número de Processos")
# plt.xlabel("Número de Processos")
# plt.ylabel("Tempo (s)")
# plt.grid(True)
# plt.tight_layout()
# plt.savefig("tempo_vs_processos.png")
