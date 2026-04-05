# kuan - Multi-Threaded HTTP Server in C

A lightweight HTTP/1.1 server built from scratch in C using POSIX sockets and a custom thread-pool architecture. No frameworks, no libraries. Just raw sockets.

Built to understand what web servers actually do under the hood.

---

## How It Works

### Thread Pool
On startup, `kuan` spawns a fixed pool of 5 worker threads. Instead of creating a new thread per request (expensive), incoming connections are pushed into a shared queue. Worker threads wait on a condition variable and wake up the moment a connection is available.

This avoids the overhead of thread creation on every request while keeping concurrency under control.

```
Main Thread                Worker Threads (x5)
    |                            |
accept() connection         wait on cond_var
    |                            |
enqueue(fd)  ──signal──>   dequeue(fd)
    |                            |
back to accept()           handle_connection()
```

### Synchronization
The shared queue is protected by a mutex + condition variable pair:
- `mtx_lock` / `mtx_unlock` - ensures only one thread touches the queue at a time
- `cnd_signal` - wakes exactly one waiting worker when a new connection arrives
- `cnd_wait` - puts idle workers to sleep without burning CPU cycles

### Request Parsing
HTTP requests are parsed manually. The `parse_request()` function walks the raw bytes character by character, extracting the method (`GET`, `POST`, etc.) and the URL path. I kept it simple for learning purpose.

### Routing
Routes are matched with `strcmp` against the parsed path and mapped to static HTML files in `public/`:

| Route | File |
|-------|------|
| `/` | `public/index.html` |
| `/about` | `public/about.html` |
| `/contact` | `public/contact.html` |

---

## Running It

```bash
# Compile
gcc -o kuan main.c -lpthread

# Run
./kuan

# Server starts at
http://localhost:8181
```

---

## Why I Built This

I was curious how HTTP actually works under the hood, like what is really happening when a request hits a server. So I built one from scratch in C to figure it out. In the process I ended up learning about threads, mutexes, and condition variables too

---

## Known Limitations

This is a learning project, not production software:

- Fixed thread pool size (no dynamic scaling)
- No support for `POST` bodies or query strings
- Buffer sizes are hardcoded (`MAX_DATA_SIZE 1000`)
