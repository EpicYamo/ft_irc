# ft_irc

An IRC server written in **C++98**, from the sockets up: one `poll()` loop,
non-blocking I/O, channels and modes, private messages, DCC file-transfer
relaying, and a small companion bot.

> Part of the **42 school curriculum**. Built by **Arda Aycan** and
> **itaskira**.

Clients such as irssi or HexChat connect over TCP, register with a password,
nickname and username, and then talk to each other in private messages or in
channels. Nothing in the server ever blocks on a single client, so one slow
connection cannot freeze the rest.

```
irssi ─┐
       ├── TCP ──► ircserv (single process, poll())  ──► channels, clients
nc    ─┘
```

## Features

### Registration and session

- `PASS`, `NICK`, `USER`, and `CAP` (capability negotiation is accepted and
  handled with `CAP LS` / `CAP END`; no IRCv3 capability is enabled).
- Welcome burst: `001` through `005`, including `NICKLEN`, `CHANNELLEN`,
  `CHANLIMIT` and `TARGMAX`.
- `PING` / `PONG` keepalive, both directions.
- Duplicate nickname detection, invalid nickname rejection, and a 60 second
  registration timeout.

### Messaging

- `PRIVMSG` and `NOTICE` to users and channels, with up to 20 comma-separated
  targets per line.
- Correct numeric replies for missing nicknames, missing targets and unknown
  channels.
- `WHO` and `WHOIS`, honouring user mode `i` (invisible).

### Channels

- `JOIN`, `PART`, `NAMES`, `TOPIC`, `KICK`, `INVITE`, `QUIT`.
- Channel modes: `i` (invite only), `t` (only operators change the topic),
  `k` (key), `o` (operator), `l` (user limit).
- Automatic operator status for the first member, operator-only actions
  enforced everywhere, topic and key persistence for the lifetime of the
  channel.
- Empty channels are destroyed when the last member leaves.

### Robustness

- Partial commands are buffered until `\r\n` (or `\n`) arrives; a single line
  is capped at 510 bytes.
- Every client has a bounded send queue (256 KiB); a client that never reads
  is disconnected instead of growing the server's memory.
- Flood control: too many commands inside a 10 second window disconnects the
  client with `Excess Flood`.
- Timeouts for registration, ping replies and closing sockets.
- `EMFILE` survival: a spare descriptor is kept open so the accept loop can
  close the listening socket, shed load and reopen instead of dying.
- `SIGINT` / `SIGTERM` shut the server down cleanly.

### Bonus

- **DCC file transfer.** The server relays `DCC SEND` offers between clients;
  the file itself travels directly between them.
- **`ircbot`.** A small standalone client that joins one channel, greets
  everyone who joins and answers `!help`.

## Supported commands

| Command | What it does |
| --- | --- |
| `CAP` | capability negotiation (`LS`, `REQ`, `END`, `LIST`) |
| `PASS` | server password, checked during registration |
| `NICK` | set or change the nickname |
| `USER` | set username and real name |
| `PING` / `PONG` | keepalive in both directions |
| `PRIVMSG` | message a user or channel |
| `NOTICE` | like `PRIVMSG`, without automatic replies |
| `JOIN` | join one or more channels, with keys |
| `PART` | leave channels with an optional reason |
| `NAMES` | list members, or every visible channel |
| `TOPIC` | read or set a channel topic |
| `MODE` | channel and user modes |
| `KICK` | remove a member (operators only) |
| `INVITE` | invite a user to a channel |
| `WHO` / `WHOIS` | client queries |
| `QUIT` | disconnect with a reason |

## Architecture

| File | Responsibility |
| --- | --- |
| `srcs/main.cpp` | argument validation, signal setup, server lifetime |
| `srcs/Server.cpp` | `poll()` loop, accept/read/write, dispatch, timeouts, flood control |
| `srcs/Client.cpp` | per-connection state: input buffer, send queue, registration |
| `srcs/Channel.cpp` | members, operators, topic, modes, invite list |
| `srcs/Parser.cpp` | IRC line parsing: prefix, command, params |
| `srcs/commands/` | one file per command |
| `includes/Replies.hpp` | numeric replies in one place |
| `bonus/` | the standalone `ircbot` |

The loop is deliberately simple:

1. `poll()` on the listening socket and every client for 1 second.
2. New connection → accept, set non-blocking, register the client.
3. Readable → append to the client's input buffer and extract complete lines.
4. Each line is parsed and dispatched through a command table.
5. Writable and something queued → send as much as the socket accepts.
6. Every pass checks registration, ping and closing timeouts.

Because the send queue is per client, a command that broadcasts to a channel
never blocks the sender; the recipients are drained on later iterations.

## Limits

These are the values the server reports in `005` and enforces internally:

| Limit | Value |
| --- | --- |
| Line length | 510 bytes |
| Send queue per client | 256 KiB |
| Nickname length | 30 |
| Channel name length | 50 |
| Channels per client | 20 |
| Targets per `PRIVMSG` / `NOTICE` | 20 |
| Mode parameters per command | 3 |
| Registration timeout | 60 s |
| Ping interval / timeout | 120 s / 120 s |
| Flood window | 10 s |
| `poll()` timeout | 1 s |

## Build

```bash
make         # ./ircserv
make bonus   # ./ircbot
make re      # clean rebuild
make clean
make fclean
```

Requirements: a C++ compiler with `-std=c++98` support. The build uses
`-Wall -Wextra -Werror`.

## Run

```bash
./ircserv <port> <password>
./ircserv 6667 pass
```

The port must be between 1 and 65535; the password must not be empty or
contain spaces.

### irssi

```bash
irssi -c localhost -p 6667 -w pass -n arda
/join #test
/msg ali hello
/topic #test hi
/mode #test +k secret
/kick #test ali
```

### netcat

```bash
nc -C localhost 6667
PASS pass
NICK arda
USER arda 0 * :Arda
JOIN #test
PRIVMSG #test :hello
```

### Bot

```bash
./ircbot 6667 pass '#test'
```

The channel name is quoted because `#` starts a shell comment.

### DCC

```
/dcc send <nick> <file>     # sender
/dcc get <nick>             # receiver
```

For clients on different machines, connect to the server through its LAN
address (not `localhost`) or set `/set dcc_own_ip <your LAN ip>` in irssi;
otherwise the offer contains `127.0.0.1` and the transfer cannot connect.

## Known limitations

- **Plain TCP only.** No TLS and no server-to-server links.
- **No services.** There is no NickServ/ChanServ; operators are granted by the
  server, not registered accounts.
- **No `LIST`.** `NAMES` without arguments lists visible channels, but the
  `LIST` command itself is not implemented.
- **IRCv3 is not implemented.** `CAP` is accepted so clients do not hang, but
  no capability is negotiated.
- **DCC is relayed, not served.** The server never touches file data.
- **Casemapping is ASCII-only.** `005` advertises `rfc1459`, but only
  `A-Z`/`a-z` are folded; the special `{}|` mappings are not.

## Resources

- RFC 1459 — Internet Relay Chat Protocol:
  https://datatracker.ietf.org/doc/html/rfc1459
- RFC 2810–2813 — architecture, channel management, client and server
  protocol: https://datatracker.ietf.org/doc/html/rfc2812
- Modern IRC Client Protocol: https://modern.ircdocs.horse/
- DCC specification: https://modern.ircdocs.horse/dcc.html
- CTCP: https://modern.ircdocs.horse/ctcp.html
- irssi source (channel sync, DCC): https://github.com/irssi/irssi
- `poll(2)`, `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`, `recv(2)`,
  `send(2)`, `fcntl(2)`, `signal(7)`

## AI usage

AI was used as a study aid, not as a code generator:

- explaining sockets, `poll()`, non-blocking I/O, IRC numerics and C++98
  constraints, and pointing at the relevant RFC sections,
- discussing design decisions,
- writing test scenarios and reading server logs.

## Authors

- **Arda Aycan** ([@EpicYamo](https://github.com/EpicYamo))
- **itaskira**

Built as part of the 42 school curriculum.
