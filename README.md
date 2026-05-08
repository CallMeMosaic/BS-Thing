# BS-Thing

# RAG Server

Ein modularer Retrieval-Augmented Generation (RAG) Server mit FastAPI und ChromaDB und Ollama.
Über die Endpoint können HTTP-Request an den Server über den Port 8080 gesendet werden.
/Query führt eine Ähnlichkeits suche in der ChromaDB aus und wandelt das Ergebnis mit Hilfe von Ollama in einen Text um.

## Installation

1. Repository klonen
2. Docker (Desktop) starten
3. setup.sh ausführen und ggf. fehlgeschlagene oder nicht gestartete Container neustarten.


## Vorraussetzung
GitHub Projekt ist in CLion
CLion compiliert im Docker

## Verwendung
### API Endpoints

- `GET /` - Server Status
- `POST /query` - RAG Query ausführen
- `POST /documents` - Dokumente hinzufügen
- `POST /upload-file` - Dokumente zu einem Prompt hinzufügen
- `GET /collections/list` - Collections auflisten
- `DELETE /collections/{name}` - Collection löschen
- `GET /health` - Gesundheitsstatus