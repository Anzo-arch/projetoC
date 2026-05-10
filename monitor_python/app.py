import tkinter as tk
from tkinter import ttk, messagebox
import sqlite3
import csv
import os
from datetime import datetime
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# ---------- BANCO DE DADOS ----------
def criar_banco():
    conn = sqlite3.connect("monitor.db")
    cursor = conn.cursor()
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS medicoes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT,
            temperatura REAL,
            umidade REAL
        )
    """)
    conn.commit()
    return conn

def importar_csv(conn, arquivo_csv):
    if not os.path.exists(arquivo_csv):
        print(f"Arquivo não encontrado: {arquivo_csv}")
        return -1
    try:
        with open(arquivo_csv, "r") as f:
            leitor = csv.reader(f)
            linhas_importadas = 0
            for linha in leitor:
                if len(linha) >= 2:
                    data_hora = linha[0]
                    dados = linha[1]
                    temp = 0.0
                    umid = 0.0
                    valido = False
                    # Extrai temperatura se existir
                    if "T:" in dados:
                        partes = dados.split(",")
                        for parte in partes:
                            if parte.strip().startswith("T:"):
                                temp = float(parte.strip().split(":")[1])
                                valido = True
                    # Extrai umidade se existir
                    if "U:" in dados:
                        partes = dados.split(",")
                        for parte in partes:
                            if parte.strip().startswith("U:"):
                                umid = float(parte.strip().split(":")[1])
                                valido = True
                    if valido:
                        conn.execute("INSERT OR IGNORE INTO medicoes (timestamp, temperatura, umidade) VALUES (?, ?, ?)",
                                     (data_hora, temp, umid))
                        linhas_importadas += 1
        conn.commit()
        print(f"Total importado: {linhas_importadas}")
        return linhas_importadas
    except Exception as e:
        print(f"Erro ao importar CSV: {e}")
        return -2

def consultar_dados(conn):
    cursor = conn.cursor()
    cursor.execute("SELECT timestamp, temperatura, umidade FROM medicoes ORDER BY timestamp DESC LIMIT 20")
    return cursor.fetchall()

# ---------- INTERFACE GRÁFICA ----------
class Dashboard:
    def __init__(self, root, conn):
        self.conn = conn
        self.root = root
        root.title("Dashboard Ambiental RAD")
        root.geometry("800x500")

        self.tree = ttk.Treeview(root, columns=("Data/Hora", "Temp (°C)", "Umidade (%)"), show="headings")
        self.tree.heading("Data/Hora", text="Data/Hora")
        self.tree.heading("Temp (°C)", text="Temp (°C)")
        self.tree.heading("Umidade (%)", text="Umidade (%)")
        self.tree.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        self.status = tk.Label(root, text="Pronto", bd=1, relief=tk.SUNKEN, anchor=tk.W)
        self.status.pack(fill=tk.X, side=tk.BOTTOM)

        frame = tk.Frame(root)
        frame.pack(pady=5)
        tk.Button(frame, text="Importar CSV", command=self.importar_click).pack(side=tk.LEFT, padx=5)
        tk.Button(frame, text="Gráfico", command=self.mostrar_grafico).pack(side=tk.LEFT, padx=5)

        self.atualizar_tabela()

    def atualizar_tabela(self):
        for row in self.tree.get_children():
            self.tree.delete(row)
        dados = consultar_dados(self.conn)
        for d in dados:
            self.tree.insert("", tk.END, values=d)
        self.status.config(text=f"{len(dados)} registros carregados")

    def importar_click(self):
        caminho = "/home/anderson/projetos/dashboard_iot/datalog.csv"
        print(f"Tentando importar de: {caminho}")
        resultado = importar_csv(self.conn, caminho)
        if resultado == -1:
            messagebox.showwarning("Erro", f"Arquivo não encontrado:\n{caminho}")
        elif resultado == -2:
            messagebox.showerror("Erro", "Falha ao processar o arquivo CSV.")
        elif resultado == 0:
            messagebox.showinfo("Aviso", "Nenhum dado novo importado (formato inválido ou já existente).")
        else:
            self.atualizar_tabela()
            messagebox.showinfo("Sucesso", f"{resultado} novas medições importadas.")

    def mostrar_grafico(self):
        dados = consultar_dados(self.conn)
        if not dados:
            messagebox.showwarning("Sem dados", "Nenhum dado para exibir. Importe o CSV primeiro.")
            return
        tempos = [d[0][-8:] for d in reversed(dados)]
        temps = [d[1] for d in reversed(dados)]
        umids = [d[2] for d in reversed(dados)]

        fig, ax = plt.subplots(figsize=(6, 3))
        ax.plot(tempos, temps, 'r-o', label="Temperatura")
        ax.plot(tempos, umids, 'b-o', label="Umidade")
        ax.legend()
        ax.set_xlabel("Hora")
        ax.set_title("Últimas medições")
        plt.xticks(rotation=45)

        janela_grafico = tk.Toplevel(self.root)
        janela_grafico.title("Gráfico")
        canvas = FigureCanvasTkAgg(fig, master=janela_grafico)
        canvas.draw()
        canvas.get_tk_widget().pack()

# ---------- MAIN ----------
if __name__ == "__main__":
    conn = criar_banco()
    root = tk.Tk()
    app = Dashboard(root, conn)
    root.mainloop()
    conn.close()
