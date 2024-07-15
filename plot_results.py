import pandas as pd
import matplotlib.pyplot as plt

# Cargar los resultados del archivo CSV
df = pd.read_csv('results.csv')

# Graficar
fig, ax = plt.subplots(figsize=(12, 8))

for key, grp in df.groupby(['distribution']):
    ax.plot(grp['num_points'], grp['quadtree_time'], label=f'QuadTree - {key}', marker='o')
    ax.plot(grp['num_points'], grp['pointquadtree_time'], label=f'PointQuadTree - {key}', marker='x')

ax.set_xlabel('Número de Puntos')
ax.set_ylabel('Tiempo de Consulta (segundos)')
ax.set_xscale('log')  # Configurar escala logarítmica para el eje X
ax.set_yscale('log')  # Configurar escala logarítmica para el eje Y
ax.set_title('Comparación del Tiempo de Consulta entre QuadTree y PointQuadTree')
ax.legend()
ax.grid(True, which="both", ls="--")
plt.show()
