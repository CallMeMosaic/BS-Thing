FROM gcc:latest
WORKDIR /app

# Kopiere den gesamten src-Ordner (inkl. Unterverzeichnis) in /app
COPY src/ ./src/

# Kompiliere mit dem korrekten Pfad
RUN gcc ./src/main.c -o ./src/myapp

# Führe die Anwendung aus
CMD ["./src/myapp"]