# Multi-stage Docker build: compile with development headers, then ship only
# the TIVIDY executable and the runtime libraries it needs.

FROM debian:bookworm-slim AS build

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        make \
        libncurses-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN make clean && make

FROM debian:bookworm-slim AS runtime

ENV DEBIAN_FRONTEND=noninteractive \
    TERM=xterm-256color

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libncurses6 \
        libtinfo6 \
        libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=build /app/main /usr/local/bin/main
COPY --from=build /app/examples ./examples
COPY --from=build /app/workflows ./workflows

# ncurses needs an interactive terminal. Start with:
# docker run --rm -it tividy
ENTRYPOINT ["/usr/local/bin/main"]
