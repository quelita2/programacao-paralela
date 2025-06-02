import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

csv_path = "resultados_afinidade.csv"
output_path = "../assets/grafico_afinidade.png"

os.makedirs(os.path.dirname(output_path), exist_ok=True)

df = pd.read_csv(csv_path)

# Converte para string (caso o CSV salve valores booleanos como True/False)
df["OMP_PROC_BIND"] = df["OMP_PROC_BIND"].astype(str)
df["OMP_PLACES"] = df["OMP_PLACES"].astype(str)

sns.set(style="whitegrid")

g = sns.relplot(
    data=df,
    x="Threads",
    y="Tempo(s)",
    hue="OMP_PLACES",
    col="OMP_PROC_BIND",
    kind="line",
    marker="o",
    col_wrap=3,
    facet_kws={'sharey': False, 'sharex': True}
)

g.set_titles("OMP_PROC_BIND = {col_name}")
g.set_axis_labels("Número de Threads", "Tempo de Execução (s)")
g.fig.suptitle("Comparação de Afinidades de Threads com OpenMP", y=1.03, fontsize=16)

plt.tight_layout()
plt.savefig(output_path)
print(f"Gráfico salvo em: {output_path}")
