# Dockerfile for ChromaDB
FROM gcc:latest

# Install Chroma
RUN pip install --no-cache-dir chromadb[server]

# Expose ChromaDB port
EXPOSE 8000

# Run ChromaDB
CMD ["chroma", "run", "--host", "0.0.0.0", "--port", "8000"]

