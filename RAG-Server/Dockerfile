#builder
FROM ubuntu:22.04 AS builder

WORKDIR /app

# Install Build-Tools
RUN apt-get update && \
    apt-get install -y --no-install-recommends build-essential && \
    rm -rf /var/lib/apt/lists/*

# copy project folder
COPY . .

# compile /app/src/server
RUN mkdir -p /app/build && \
    gcc ./src/server.c ./src/server_socket.c ./src/client_handler.c ./src/api.c ./src/request_parser.c -I./include -o ./src/server

# runtime
FROM ubuntu:22.04

WORKDIR /app

# copy /app/src/server (Builder) to /app/src/server (Runtime)
COPY --from=builder /app/src/server ./src/server

# set rigths
RUN chmod +x ./src/server

# run
CMD ["./src/server"]