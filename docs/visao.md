# Visão do Projeto - Suíte de Monitoramento e Visualização (SMV)

## 1. Objetivo
Desenvolver um conjunto modular de aplicações em linguagem C que realizam coleta de dados de sensores, armazenamento, comunicação em rede e visualização gráfica em tempo real, demonstrando competências em programação de sistemas, concorrência e integração hardware-software.

## 2. Público-alvo
- Estudantes e professores da disciplina de Linguagem C e Sistemas Embarcados.
- Recrutadores e avaliadores técnicos (portfólio).
- Hobbystas de IoT e automação.

## 3. Principais funcionalidades
- **Calculadora Modular:** operações matemáticas básicas com histórico em arquivo, servindo como exemplo de bibliotecas estáticas.
- **Visualizador de Dados:** gráfico de barras interativo a partir de arquivo CSV, demonstrando uso de OpenGL e captura de eventos.
- **Estação Meteorológica Serial:** leitura de dados simulados via porta serial com registro temporal.
- **Servidor Multithread de Monitoramento:** distribuição dos dados a múltiplos clientes TCP com controle de concorrência.
- **Dashboard IoT Integrador:** gráfico de linha em tempo real que conecta a estação serial ao visualizador via rede.

## 4. Riscos identificados
- Dependência de bibliotecas externas (freeglut, pthreads, socat) que podem não estar presentes no ambiente do usuário.
- Código com pouca cobertura de testes automatizados, podendo conter erros em cenários de borda.
- Ausência de interface amigável para configuração (tudo via terminal/argumentos), limitando o público leigo.
- A simulação de porta serial depende de `socat`, que pode não funcionar em todos os sistemas.
