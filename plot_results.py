import pandas as pd
import matplotlib.pyplot as plt
import os

# Ruta del archivo CSV
csv_file = 'results.csv'

# Verificar si el archivo existe
if not os.path.isfile(csv_file):
    print(f"Error: El archivo {os.path.abspath(csv_file)} no existe.")
else:
    try:
        # Leer los datos del archivo CSV
        df = pd.read_csv(csv_file)

        # Crear una figura y un eje para la gráfica
        fig, ax = plt.subplots(figsize=(12, 8))

        # Graficar los datos agrupados por el tipo de distribución
        for key, grp in df.groupby(['distribution']):
            ax.plot(grp['num_points'], grp['quadtree_time'], label=f'QuadTree - {key}', marker='o')
            ax.plot(grp['num_points'], grp['pointquadtree_time'], label=f'PointQuadTree - {key}', marker='x')

        # Configurar etiquetas de los ejes
        ax.set_xlabel('Número de Puntos')
        ax.set_ylabel('Tiempo de Consulta (segundos)')

        # Configurar escalas logarítmicas
        ax.set_xscale('log')
        ax.set_yscale('log')

        # Título de la gráfica
        ax.set_title('Comparación del Tiempo de Consulta entre QuadTree y PointQuadTree')

        # Mostrar leyenda y cuadrícula
        ax.legend()
        ax.grid(True, which="both", ls="--")

        # Mostrar la gráfica
        plt.show()
    except Exception as e:
        print(f"Error al leer el archivo CSV: {e}")
