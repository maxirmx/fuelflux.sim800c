# PPP0 authorize demo (Orange Pi / Armbian) — fixed JSON body lifetime

## Build

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libcurl4-openssl-dev

mkdir -p build
cd build
cmake ..
cmake --build . -j
```

## Run (numeric JSON, default)

```bash
sudo ./build/sim800c
```

## Run (send UIDs as strings)

Some backends expect `"3000"` instead of `3000`. Use:

```bash
sudo ./build/sim800c --card-string --pump-string
```

## Verify it uses ppp0

```bash
sudo tcpdump -ni ppp0 host ttft.uxp.ru
```
