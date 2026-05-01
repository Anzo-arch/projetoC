# 🚀 Projetos em Linguagem C – Disciplina ADS

[![Linguagem: C](https://img.shields.io/badge/Linguagem-C-00599C?style=flat&logo=c)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Licença: MIT](https://img.shields.io/badge/Licen%C3%A7a-MIT-green.svg)](https://opensource.org/licenses/MIT)
[![Plataforma: Linux](https://img.shields.io/badge/Plataforma-Linux-blue?style=flat&logo=linux)](https://kernel.org)

Repositório criado para consolidar, na prática, os principais conceitos da disciplina de **Linguagem C** — de bibliotecas estáticas até comunicação serial, threads e dashboards gráficos em tempo real.

## 📚 Projetos

| # | Projeto | Conceitos Trabalhados |
|---|---------|------------------------|
| 1 | **Calculadora Modular** | Bibliotecas estáticas, headers, E/S de console e arquivos |
| 2 | **Visualizador Gráfico de Dados** | OpenGL/GLUT, gráficos 2D, captura de eventos (teclado/mouse) |
| 3 | **Estação Meteorológica Serial** | Comunicação serial (UART), `termios`, simulação com `socat` |
| 4 | **Servidor Multithread de Monitoramento** | Sockets TCP, threads (`pthread`), mutexes, servidor concorrente |
| 5 | **Dashboard IoT Integrador** | Une todos os conceitos: serial → servidor → dashboard OpenGL em tempo real |

## ⚙️ Dependências (Ubuntu / Debian)

```bash
sudo apt update
sudo apt install -y build-essential freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev socat
