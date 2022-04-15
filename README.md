## RC-Project

Project for the Networking and Communication subject.

## Pre Requirements

With the purpose to run this project it is required to have some tools. It is needed a C compiler as `gcc` (or others) and a OS as `gnu/linux`.
In the case we are using a Lubuntu distribution.


## Obtain the main code

In orther to obtain the sorce code, open your terminal and clone this project.

```bash
git clone https://github.com/JoaoESmoreira/RC-Project
```


## Compile

Now you have access to the repository, you can complile the code. There are two options: to compile without print debug / to compile with print debug.

```bash
make
```

```bash
make debug
```


## Run

To run the server you just need to run:
```bash
./prog 
```
In other terminal, make use of `netcat` to simulate the admin usage.
```bash
nc -u localhost 9876
```

## Runing server

After you run the server and netcat, it is time for the admin to send some instructions to the server.

### Options
#### Add a new user
```bash
ADD_USER {username} {password} {the sockets we have access} {budget}
```

#### List all users
```bash
LIST
```

#### Delete a user
```bash
DEL {username}
```

#### Refresh the time
```bash
REFRESH {new time}
```

#### To log out
```bash
QUIT
```

#### To log out
```bash
QUIT
```

#### To torn of the server
```bash
QUIT SERVER
```


## Collaborators

João - joaomoreira@student.dei.uc.pt https://github.com/JoaoESmoreira

Rafael - rafaelg@student.dei.uc.pt  https://github.com/rafag00
