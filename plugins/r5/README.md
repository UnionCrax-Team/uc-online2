# r5 plugin — Windrose

Gets a Windrose client onto a **dedicated server via direct connection**.

## What it does

One hook. `UR5CaHttpClient::Init` sets the coop gateway host for every request;
this rewrites it to a host you control, which answers the auth route with a
synthetic `IsOk`. Nothing is ever sent to the publisher.

That is the whole plugin.

## Why it is needed

Windrose authorises the client through its coop backend before any online path
opens — joining included:

```
POST https://r5coopapigateway-<region>-release.windrose.support
     /api/v1/Auth/AuthenticateClientBySteam
424 {"errorcode":102,"errordesc":"Ticket for other app"}
```

The gateway asks **Steam** to validate the ticket against Windrose's real AppId,
which a ticket minted under a spoofed AppId can never satisfy. That check is
server-side and unforgeable — so this does not try to pass it. It sends the
request somewhere else.

## Setup

```ini
[Settings]
AppId=480
ogAppId=3041230
PluginsFolder=plugins

[R5]
CaHttpInitRVA=0x729DC60
GatewayHostOverride=uco2.iforgor.cc
```

`GatewayHostOverride` must:
- be a **hostname only** — the client composes `https:// + host + :443 + /api/v1 + <route>` around it
- be **shorter** than `r5coopapigateway-kr-release.windrose.support` (44 chars), since the rewrite is in place
- have a **valid certificate** — the client still speaks ordinary HTTPS to it

A Cloudflare Worker suits this well: no TLS to terminate, no socket redirect, no
DNS hooking. It needs to answer `/api/v1/Auth/AuthenticateClient*` with
`IsOk/AccountId/DeviceId/AccessToken/AccessTokenExpiryTime/CoopApiEndpoints/CoturnEndpoints`.

## Joining a dedicated server

Use **direct connection**. On the server (`ServerDescription.json`, or the env
of the Docker image):

```
UseDirectConnection      = true
P2pProxyAddress          = 0.0.0.0     # NOT 127.0.0.1 in a container --
DirectConnectionProxyAddress = 0.0.0.0 # Docker's published port cannot reach loopback
DirectConnectionServerPort = 7777      # TCP *and* UDP
```

Direct connection needs **no Steam ticket and no auth** — the official
`DedicatedServer.md` documents no such step, and the client satisfies coop
verification locally with the literal `BLSessionId '<DirectConnection>'`,
short-circuiting the CmService/ICE path entirely. That is why joining works when
hosting did not.

If the server rejects you, read its log: it prints both password hashes on
`OnAccountBLConnected`, and they are a plain MD5 of the UTF-8 password.

## Re-deriving the RVA on a new build

`CaHttpInitRVA` is a raw address into one exact executable. To find it again:

1. search the exe for the ANSI literal `UR5CaHttpClient::Init` (UE logs `__FUNCTION__`)
2. take the function that references it, via a `lea` xref resolved through `.pdata`
3. ignore the two `::<lambda_N>::operator ()` variants — the real one's literal
   has no suffix

It has been **1534 bytes in every build seen so far** (old Steam build, current
build, and the dedicated server), which is a useful confirmation.
