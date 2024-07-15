import tkinter as tk
from tkinter import messagebox
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import random

# Clase Point para representar puntos con coordenadas x e y
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

# Clase Rectangle para representar los límites del QuadTree
class Rectangle:
    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

    def contains(self, point):
        return (self.x - self.width <= point.x <= self.x + self.width and
                self.y - self.height <= point.y <= self.y + self.height)

    def intersects(self, range):
        return not (range.x - range.width > self.x + self.width or
                    range.x + range.width < self.x - self.width or
                    range.y - range.height > self.y + self.height or
                    range.y + range.height < self.y - self.height)

# Clase PointQuadTree para representar el QuadTree
class PointQuadTree:
    def __init__(self, boundary):
        self.boundary = boundary
        self.point = None
        self.divided = False

    def subdivide(self):
        x, y, w, h = self.boundary.x, self.boundary.y, self.boundary.width, self.boundary.height
        self.nw = PointQuadTree(Rectangle(x - w / 2, y + h / 2, w / 2, h / 2))
        self.ne = PointQuadTree(Rectangle(x + w / 2, y + h / 2, w / 2, h / 2))
        self.sw = PointQuadTree(Rectangle(x - w / 2, y - h / 2, w / 2, h / 2))
        self.se = PointQuadTree(Rectangle(x + w / 2, y - h / 2, w / 2, h / 2))
        self.divided = True

    def insert(self, point):
        if not self.boundary.contains(point):
            return False

        if self.point is None:
            self.point = point
            return True

        if not self.divided:
            self.subdivide()

        if self.nw.insert(point) or self.ne.insert(point) or self.sw.insert(point) or self.se.insert(point):
            return True

    def query(self, range, found):
        if not self.boundary.intersects(range):
            return

        if self.point and range.contains(self.point):
            found.append(self.point)

        if self.divided:
            self.nw.query(range, found)
            self.ne.query(range, found)
            self.sw.query(range, found)
            self.se.query(range, found)

    def remove(self, point):
        if not self.boundary.contains(point):
            return False

        if self.point and self.point == point:
            self.point = None
            return True

        if self.divided:
            if self.nw.remove(point) or self.ne.remove(point) or self.sw.remove(point) or self.se.remove(point):
                return True

        return False

# Función para dibujar el QuadTree y los puntos
def draw_quadtree(ax, qt):
    if qt is None:
        return

    rect = patches.Rectangle((qt.boundary.x - qt.boundary.width, qt.boundary.y - qt.boundary.height),
                             qt.boundary.width * 2, qt.boundary.height * 2,
                             linewidth=1, edgecolor='r', facecolor='none')
    ax.add_patch(rect)

    if qt.point:
        ax.plot(qt.point.x, qt.point.y, 'bo')

    if qt.divided:
        draw_quadtree(ax, qt.nw)
        draw_quadtree(ax, qt.ne)
        draw_quadtree(ax, qt.sw)
        draw_quadtree(ax, qt.se)

# Funciones de inserción, búsqueda y eliminación
def insert_point():
    try:
        x, y = float(entry_x.get()), float(entry_y.get())
        point = Point(x, y)
        if qt.insert(point):
            points.append(point)
            update_canvas()
        else:
            messagebox.showwarning("Advertencia", "Punto fuera de los límites.")
    except ValueError:
        messagebox.showerror("Error", "Coordenadas no válidas.")

def search_points():
    try:
        x, y, w, h = float(entry_x.get()), float(entry_y.get()), float(entry_w.get()), float(entry_h.get())
        range = Rectangle(x, y, w, h)
        found = []
        qt.query(range, found)
        messagebox.showinfo("Puntos encontrados", f"Se encontraron {len(found)} puntos.")
    except ValueError:
        messagebox.showerror("Error", "Coordenadas o dimensiones no válidas.")

def remove_point():
    try:
        x, y = float(entry_x.get()), float(entry_y.get())
        point = Point(x, y)
        if qt.remove(point):
            points.remove(point)
            update_canvas()
        else:
            messagebox.showwarning("Advertencia", "Punto no encontrado.")
    except ValueError:
        messagebox.showerror("Error", "Coordenadas no válidas.")

# Función para actualizar el canvas
def update_canvas():
    ax.clear()
    draw_quadtree(ax, qt)
    canvas.draw()

# Inicialización de la aplicación Tkinter
root = tk.Tk()
root.title("QuadTree Visualization")

frame = tk.Frame(root)
frame.pack(side=tk.LEFT, padx=10, pady=10)

tk.Label(frame, text="X:").grid(row=0, column=0)
entry_x = tk.Entry(frame)
entry_x.grid(row=0, column=1)

tk.Label(frame, text="Y:").grid(row=1, column=0)
entry_y = tk.Entry(frame)
entry_y.grid(row=1, column=1)

tk.Label(frame, text="Width (for search):").grid(row=2, column=0)
entry_w = tk.Entry(frame)
entry_w.grid(row=2, column=1)

tk.Label(frame, text="Height (for search):").grid(row=3, column=0)
entry_h = tk.Entry(frame)
entry_h.grid(row=3, column=1)

btn_insert = tk.Button(frame, text="Insert", command=insert_point)
btn_insert.grid(row=4, column=0, columnspan=2, pady=5)

btn_search = tk.Button(frame, text="Search", command=search_points)
btn_search.grid(row=5, column=0, columnspan=2, pady=5)

btn_remove = tk.Button(frame, text="Remove", command=remove_point)
btn_remove.grid(row=6, column=0, columnspan=2, pady=5)

# Inicialización de matplotlib
fig, ax = plt.subplots(figsize=(8, 8))
canvas = FigureCanvasTkAgg(fig, master=root)
canvas.get_tk_widget().pack(side=tk.RIGHT, fill=tk.BOTH, expand=1)

# Creación del QuadTree y puntos iniciales
boundary = Rectangle(0, 0, 200, 200)
qt = PointQuadTree(boundary)
points = []

# Actualizar el canvas inicialmente
update_canvas()

root.mainloop()
