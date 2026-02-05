# PPP0 authorize demo (Orange Pi / Armbian)

C++ demo/test application that performs the equivalent of:

```bash
curl http://ttft.uxp.ru/api/pump/authorize \
  -H "Content-Type: application/json" \
  -d '{"CardUid":3000,"PumpControllerUid":1}'
```

…but **forces the request to go out via `ppp0`** even when the default route is `eth0`.

It does that by binding the request to an interface using **libcurl `CURLOPT_INTERFACE`**.

## Build

Install deps:

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libcurl4-openssl-dev
```

Build:

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

## Run

Default parameters:

```bash
sudo ./build/ppp0_authorize_demo
```

Override interface / values:

```bash
sudo ./build/ppp0_authorize_demo --ppp-if ppp0 --card 3000 --pump 1
```

Override URL:

```bash
sudo ./build/ppp0_authorize_demo --url http://ttft.uxp.ru/api/pump/authorize
```

## Verify it uses ppp0

Watch traffic:

```bash
sudo tcpdump -ni ppp0 host ttft.uxp.ru
```

Then run the demo in another terminal.

## Notes

- If you get permission errors when binding to interface, run the program with `sudo`.
- If your PPP interface name differs, pass `--ppp-if <name>`.
