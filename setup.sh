#!/bin/bash

echo "Setting up RAG System..."

# Create data directories
mkdir -p data/llm data/documents

# Set permissions
chmod -R 755 data/

# Alte Container stoppen
docker compose down --remove-orphans


# Port freigeben falls nötig
#sudo pkill -f rag-llm
sudo pkill -f rag-server
docker rm -f rag-server

# Build and start services
docker compose up --build -d

#docker exec -it rag-llm ollama pull qwen3:0.6b

docker build -t rag-server RAG-Server/.
docker run rag-server

echo "RAG System is ready!"
read pause
