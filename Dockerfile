FROM alpine:latest AS builder

RUN apk add --no-cache \
    build-base \
    cmake \
    libpcap-dev \
    gtest-dev

WORKDIR /src
COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build --parallel


FROM alpine:latest AS runner

RUN apk add --no-cache \
    libpcap \
    libstdc++ \
    gtest 

COPY --from=builder /src/build/sniffer              /usr/local/bin/sniffer
COPY --from=builder /src/build/tests/unit_tests     /usr/local/bin/unit_tests
COPY --from=builder /src/build/tests/scenario_01_pipeline /usr/local/bin/scenario_01_pipeline
COPY --from=builder /src/build/tests/scenario_02_malformed /usr/local/bin/scenario_02_malformed
COPY --from=builder /src/build/tests/scenario_03_load /usr/local/bin/scenario_03_load

CMD ["unit_tests"]