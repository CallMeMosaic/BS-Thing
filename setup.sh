#!/bin/bash

echo "Setting up RAG System..."

# Verzeichnisse erstellen (falls nicht vorhanden)
mkdir -p data/rag-llm data/documents data/db

# Berechtigungen setzen (unter Windows oft nicht nötig, aber für WSL2/Docker)
chmod -R 755 data/ || echo "Berechtigungen konnten nicht gesetzt werden (ignoriere unter Windows)"

# Alte Container/Dienste stoppen
echo "Stoppe alte Container/Dienste..."
docker compose down --remove-orphans

# Ports freigeben (Windows-Alternative zu `pkill`)
echo "Freigabe von Ports..."
taskkill //F //IM "ollama.exe" l || echo "Kein Ollama-Prozess gefunden (ignoriere)"
taskkill //F //IM "docker*" || echo "Keine Docker-Prozesse gefunden (ignoriere)"

# Docker-Services bauen und starten
echo "Baue und starte Docker-Services..."
docker compose up --build -d

# LLM-Modell herunterladen (falls llm-Container existiert)
echo "Lade Qwen3:0.6B-Modell..."
docker exec -it rag-llm ollama pull qwen3:0.6b

echo "RAG System ist bereit!"
read -p "Drücke Enter, um fortzufahren..."