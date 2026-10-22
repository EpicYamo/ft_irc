*This project has been created as part of the 42 curriculum by aaycan, itaskira.*

# ft_irc

## Description

ft_irc is an IRC (Internet Relay Chat) server written in C++98. Clients such
as irssi connect to it over TCP, register with a password, nickname and
username, and then talk to each other in private messages or in channels.

The server runs in a single process with a single `poll()` loop. All sockets
are non-blocking; nothing ever waits on one client, so a slow or broken
client cannot freeze the others.

Main features:

- Registration: `PASS`, `NICK`, `USER`, `CAP` (ignored capability
  negotiation), welcome replies `001`-`005`.
- Messaging: `PRIVMSG` and `NOTICE` to users and channels, several targets
  separated by commas.
- Channels: `JOIN`, `PART`, `NAMES`, `TOPIC`, `KICK`, `INVITE`, `QUIT`.
- Channel modes (`MODE`): `i` (invite only), `t` (topic restricted to
  operators), `k` (key), `o` (operator), `l` (user limit). User mode `i`
  (invisible).
- Queries used by clients: `WHO`, `WHOIS`, `PING` / `PONG`, ban list query
  (always empty).
- Robustness: partial messages are buffered until `\r\n` (or `\n`) arrives,
  line and buffer limits, send queue limit, flood control, registration and
  ping timeouts, survival of `EMFILE` and out-of-memory situations.

Reference client: **irssi**.

### Bonus

- **File transfer:** DCC. The server relays the `DCC SEND` offer unchanged;
  the file itself travels directly between the two clients.
- **Bot:** `ircbot`, a separate program that joins one channel, greets
  everyone who joins and answers `!help`.

## Instructions

### Build

```sh
make            # builds ./ircserv
make bonus      # builds ./ircbot
make clean      # removes object files
make fclean     # removes object files and binaries
make re         # rebuilds everything
```

Requires a C++ compiler supporting `-std=c++98` (built with
`-Wall -Wextra -Werror`).

### Run the server

```sh
./ircserv <port> <password>
./ircserv 6667 pass
```

The port must be between 1 and 65535; the password must not be empty or
contain spaces.

### Connect with irssi

```sh
irssi -c localhost -p 6667 -w pass -n arda
```

Then for example: `/join #test`, `/msg ali hello`, `/topic #test hi`,
`/mode #test +k secret`, `/kick #test ali`.

### Connect with nc

```sh
nc -C localhost 6667
PASS pass
NICK arda
USER arda 0 * :Arda
JOIN #test
PRIVMSG #test :hello
```

### Bot

```sh
./ircbot 6667 pass '#test'
```

The channel name must be quoted because `#` starts a comment in the shell.

### File transfer (irssi)

```
/dcc send <nick> <file>     # sender
/dcc get <nick>             # receiver
```

When the two clients are on different machines, connect to the server with
its LAN address (not `localhost`), or set `/set dcc_own_ip <your LAN ip>`,
otherwise irssi offers `127.0.0.1` and the receiver cannot connect.

## Resources

- RFC 1459 - Internet Relay Chat Protocol:
  https://datatracker.ietf.org/doc/html/rfc1459
- RFC 2810-2813 - IRC architecture, channel management, client protocol,
  server protocol: https://datatracker.ietf.org/doc/html/rfc2812
- Modern IRC Client Protocol: https://modern.ircdocs.horse/
- DCC specification: https://modern.ircdocs.horse/dcc.html
- CTCP: https://modern.ircdocs.horse/ctcp.html
- irssi source code (channel sync, DCC):
  https://github.com/irssi/irssi
- Linux man pages: `poll(2)`, `socket(2)`, `bind(2)`, `listen(2)`,
  `accept(2)`, `recv(2)`, `send(2)`, `fcntl(2)`, `signal(7)`

### How AI was used

- Explaining concepts (sockets, `poll()`, non-blocking I/O, IRC numerics,
  C++98 features) and finding the relevant RFC sections.
- Discussing design decisions.
- Writing test scenarios and analysing server logs.
