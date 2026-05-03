# Especificação de Requisitos

## Projeto 1 – Calculadora Modular
- **RF01:** O sistema deve permitir ao usuário digitar uma operação no formato `operando1 operador operando2` (ex: `5 + 3`).
- **RF02:** Deve suportar os operadores: `+`, `-`, `*`, `/`, `^` (potência).
- **RF03:** Deve exibir o resultado formatado com duas casas decimais.
- **RF04:** Deve registrar cada operação bem-sucedida em um arquivo `historico.txt` com data/hora.
- **RNF01:** A biblioteca de operações deve ser compilada separadamente como uma biblioteca estática.
- **RNF02:** O programa deve funcionar em Linux com GCC padrão.

## Projeto 2 – Visualizador Gráfico de Dados
- **RF05:** Deve ler um arquivo CSV com pares (rótulo, valor) e plotar um gráfico de barras.
- **RF06:** Deve permitir zoom (`+`/`-`) e restaurar zoom padrão (`r`).
- **RF07:** Deve permitir selecionar uma barra com clique do mouse, exibindo o valor no console.
- **RNF03:** Deve utilizar OpenGL/GLUT e ser compatível com freeglut3.

## Projeto 3 – Estação Meteorológica Serial
- **RF08:** Deve ler continuamente uma porta serial (física ou virtual) e exibir linhas recebidas no console.
- **RF09:** Deve armazenar cada linha recebida com timestamp em arquivo `datalog.csv`.
- **RNF04:** Deve configurar a porta serial para 9600 baud, 8N1, modo raw.

## Projeto 4 – Servidor Multithread de Monitoramento
- **RF10:** Deve aceitar conexões TCP na porta 8080 e responder aos comandos `GET` (última medição) e `LOG` (histórico completo).
- **RF11:** Deve atender múltiplos clientes simultaneamente usando threads.
- **RNF05:** O acesso ao buffer de dados deve ser protegido por mutex.

## Projeto 5 – Dashboard IoT Integrador
- **RF12:** Deve conectar-se ao servidor (Projeto 4) e receber dados periodicamente.
- **RF13:** Deve exibir um gráfico de linha duplo (temperatura e umidade) que se atualiza em tempo real.
- **RF14:** Deve permitir pausar a atualização (`p`), limpar o gráfico (`r`) e ajustar zoom (`+`/`-`).
- **RNF06:** A comunicação com o servidor deve ocorrer em thread separada da interface gráfica.
