# Tividy
## Wonder Team

```
Miss. Keagan van Biljon - u23594412
Mr. Matthew Malan - u25009801
Mr. Lucian van der Merwe - u25054262
Miss. Hayley Nel - u25101821
Mr. Ruben van der Vyver - u25007786
```
TIVIDY is a generic workflow management system used to define, execute and monitor workflows. It manages work items, participants, dependencies, routing, approvals, failures and progress. Workflow definitions describe how a process should operate, while running instances track the progress and data of a specific process. TIVIDY should remain generic and general, without becoming too focused on a specific field/organisation. 

# TIVIDY

## Dependencies

**Local build**
- C++11 compatible compiler (G++)
- GNU Make
- ncurses development library

Install on Ubuntu/Debian:

```bash
sudo apt update
sudo apt install build-essential make libncurses-dev
```

**Docker**
- Docker Engine or Docker Desktop

## Build

### Using Make

Build from the project root:

```bash
make
```

Run the default demo:

```bash
./main
```

Run with a workflow file:

```bash
./main test.txt
```

Clean build files:

```bash
make clean
```

### Using Docker

Build the image:

```bash
docker build -t tividy .
```

Run the default demo:

```bash
docker run --rm -it tividy
```

Run with a custom workflow file:

```bash
docker run --rm -it tividy /app/workflows/workflow_file.txt
```

### Links
 - [Demo Document](https://docs.google.com/document/d/1Tvk2_Lwu3TnETVxUGc6hZ45iJdSNOIl9Yx-YCw3b26I/edit?usp=sharing)
 - [Class Diagram](https://online.visual-paradigm.com/share.jsp?id=343138303733392d39#diagram:workspace=katyghkb&proj=0&id=9)
 - [Initial Notes](https://docs.google.com/document/d/1qfJP5-RVT95t6Qr2V-mfdque3NM8BMf-QKCEgJ3NrJ0/edit?usp=sharing)
