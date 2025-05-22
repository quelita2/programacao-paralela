import numpy as np
import matplotlib.pyplot as plt

def plot_data(filename, title):
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
    plt.savefig(f"grafico_{title.replace(' ', '_').lower()}.png")

plot_data("velocidade_parado.dat", "Campo de Velocidade - Fluido Parado")
plot_data("velocidade_perturbado.dat", "Campo de Velocidade - Após Perturbação")
