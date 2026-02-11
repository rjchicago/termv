# --- Build stage ---
FROM alpine:latest AS builder
ARG VERSION=dev
RUN apk add --no-cache gcc musl-dev ncurses-dev ncurses-static make
COPY . /termv
WORKDIR /termv
RUN make clean && make VERSION=${VERSION}

# --- Runtime stage ---
FROM alpine:latest
RUN apk add --no-cache ncurses ncurses-terminfo-base
COPY --from=builder /termv/termv /usr/local/bin/termv
ENV TERM=xterm-256color
CMD ["termv"]
