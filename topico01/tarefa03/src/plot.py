import matplotlib.pyplot as plt
import pandas as pd

# Carregar os dados do CSV
data = pd.read_csv("data.csv")

# Criar o gráfico
plt.figure(figsize=(8, 6))
plt.plot(data["Iterações"], data["Dígitos Corretos"], marker='o', linestyle='-', color='b', label="Dígitos Corretos")
#plt.xscale("log")  # Escala logarítmica para as iterações
plt.xlabel("Número de Iterações")
plt.ylabel("Dígitos Corretos de π")
plt.title("Convergência da Aproximação de π")
plt.legend()
plt.grid(True, which="both", linestyle="--", linewidth=0.5)
plt.savefig("../assets/grafico_pi.png")