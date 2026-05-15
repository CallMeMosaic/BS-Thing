FROM ubuntu:latest

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    gdb \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Kopiere den gesamten src-Ordner (inkl. Unterverzeichnis) in /app
WORKDIR /app
COPY . .
RUN mkdir -p /app/build

# Kompiliere mit dem korrekten Pfad
RUN gcc ./src/server.c -o ./src/server

# Führe die Anwendung aus
CMD ["./src/server"]