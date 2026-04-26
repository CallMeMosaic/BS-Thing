# Basis-Image für C-Entwicklung
FROM MinGw:latest

# Arbeitsverzeichnis setzen
WORKDIR /app/RAG-Server

# Systemabhängigkeiten installieren
RUN apt-get update && apt-get install -y \
    gcc \
    libcurl4-openssl-dev \
    libsqlite3-dev \
    make \
    && rm -rf /var/lib/apt/lists/*

# Projekt in den Container kopieren
COPY . /app

# Kompiliere das C-Programm
RUN gcc RAG-Server/src/main.c -o RAG-Server/src/main

# Optional: Führe das Programm aus
CMD ["./src/main"]