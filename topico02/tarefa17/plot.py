import pandas as pd
import matplotlib.pyplot as plt

# Carregar dados
df = pd.read_csv('resultados.csv')

# Plot: tempo vs. número de processos, para cada tamanho de matriz
plt.figure(figsize=(10, 6))
for m in sorted(df['M'].unique()):
    dados = df[df['M'] == m]
    plt.plot(dados['processos'], dados['tempo'], marker='o', label=f'M={m}')

plt.xlabel('Número de Processos')
plt.ylabel('Tempo de Execução (s)')
plt.title('Tempo de Execução vs. Nº de Processos (Distribuição por Colunas)')
plt.legend(title='Tamanho da Matriz')
plt.grid(True)
plt.tight_layout()
plt.savefig('grafico_tempos.png')
