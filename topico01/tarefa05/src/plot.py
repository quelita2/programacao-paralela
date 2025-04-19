import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Leitura dos dados salvos
df = pd.read_csv("resultados.csv")

# Plot
plt.figure(figsize=(10, 6))
sns.lineplot(data=df, x="n", y="Tempo(s)", hue="Threads", marker="o", palette="tab10")

plt.xscale("log")
plt.yscale("log")
plt.xlabel("Valor de n")
plt.ylabel("Tempo (segundos)")
plt.title("Versão Paralela - Tempo vs N")
plt.grid(True, which="both", ls="--", lw=0.5)
plt.tight_layout()
plt.savefig("grafico2_paralelo_threads.png")
plt.show()
