# LittleVec

*[Read this README in Russian](readme_ru.md)*

**LittleVec** is a lightweight and memory-efficient vector database.  
It allows you to quickly search for the nearest vectors and store extra information (payload) for each object.

It works as a plugin for [RocksServer](https://github.com/valmat/RocksServer).  
**Install the plugin, start RocksServer — and you can use it right away!**

---

## Advantages

- Minimalistic API: only the necessary functions, nothing extra
- Efficient in memory and speed
- Easy to integrate
- Supports several independent vector databases
- Works even on very low-power servers and VPS

---

## Quick Start

1. Install RocksServer and the LittleVec plugin (see the installation guide).
2. Start RocksServer.
3. Use the LittleVec HTTP API to work with vectors and search.

---

# API

## Working with Databases

> All database operations use endpoints `/vecdb/...`

All endpoints in this section return response code 200 and the body:
```json
{"success": true}
```
on success.

### 1. Create a Database

**POST** `/vecdb/create`

```json
{
    "name": "<database_name (string)>",
    "dim": <vector_dimension (int)>,
    "dist": "<distance_metric (optional, see below)>"
}
```

- **name** (string, required) — unique database name
- **dim** (int, required) — vector dimension
- **dist** (string, optional) — metric: `"qcos"` (default), `"cos"`, `"dot_prod"`, `"l1"`, `"l2"`

**Example request**:
```json
{
    "name": "my_vectors",
    "dim": 128,
    "dist": "cos"
}
```

#### Possible errors:
- 400: Invalid parameters (`{"error": "Description of problem"}`)
- 409: Database already exists (`{"error": "Database already exists"}`)

---

### 2. Delete a Database

**POST** `/vecdb/delete`

```json
{
    "name": "<database_name>"
}
```

---

### 3. Update Database Parameters

**POST** `/vecdb/update`

```json
{
    "name": "<database_name>",
    "dist": "<new_distance_metric>"
}
```
> *Only the distance metric changes.*

---

## Vector Operations

> All vector operations use endpoints `/vectors/...`  
> In every request, you must include the `db_name` field — the name of the database.

---

### 4. Add / Update Vectors

**POST** `/vectors/set`

```json
{
    "db_name": "my_vectors",
    "data": [
        {
            "id": "123",
            "vector": [0.1, 0.2, 0.3, ...],
            "payload": <any JSON, optional>
        },
        ...
    ]
}
```
- **id** (string) — unique vector identifier
- **vector** (array of floats) — length must match dim
- **payload** — any serializable object (optional)

On success, returns code 200 and body:
```json
{"success": true}
```

---

### 5. Delete Vectors

**DELETE** `/vectors/delete`

```json
{
    "db_name": "my_vectors",
    "data": [
        { "id": "123" },
        ...
    ]
}
```

On success, returns code 200 and body:
```json
{"success": true}
```

---

## Search for Nearest Vectors

### 6. Search by One Vector

**POST** `/vectors/search`

```json
{
    "db_name": "my_vectors",
    "vector": [0.1, 0.2, 0.3, ...],
    "top_k": <number of nearest vectors to return, optional>,
    "dist": "<metric, optional>"  // overrides database dist
}
```

**Response:**
```json
{
    "nearest": [
        { "id": "123", "distance": 0.123, "payload": ... },
        ...
    ]
}
```

---

### 7. Batch Search by Array of Vectors

**POST** `/vectors/batch_search`

```json
{
    "db_name": "my_vectors",
    "data": [
        {
            "vector": [0.1, 0.2, 0.3, ...],
            "extra": <any JSON, optional>
        },
        ...
    ],
    "top_k": <number of nearest vectors to return, optional>,
    "dist": "<metric, optional>"
}
```

**Response:**
```json
{
    "results": [
        {
            "nearest": [
                { "id": "123", "distance": 0.123, "payload": ... },
                ...
            ],
            "extra": ... // if extra was sent
        },
        ...
    ]
}
```

---

# Error Format

All errors return the correct HTTP status (400, 404, 409, 500, etc.).  
The response body always contains:
```json
{
  "error": "Error description"
}
```

**Common errors:**
- 400: Invalid request / validation error (for example, vector size does not match)
- 404: Database or record not found
- 409: Trying to create a database or record that already exists
- 500: Internal server error

---

# Features

- Built on [RocksDB](https://rocksdb.org/) — a very [efficient database](https://github.com/facebook/rocksdb) with iterators and high SSD performance.

- The main goal of LittleVec is to be a memory-efficient vector database. Low RAM usage: **15–20 MB** even when storing millions of vectors and gigabytes of data.

- On one million vectors of size 50, a search takes about 60 ms.

- No sharding or replication: this solution is for running on a single server, but in this case it works efficiently.

## Additional Info

- Each request must specify the database name, so you can use several databases at the same time.

- The payload size limit depends on RocksDB and the `max_body_size` parameter in RocksServer.
  
- In every search request, you can set the distance metric manually. You can also change the default metric by updating the database parameters.

---

## License

LittleVec is distributed under the [MIT License](LICENSE).
