# RC-Project


The main objective of the project is build a socket transaction system for the Networking and Communication subject.


## Pre Requirements

With the purpose to run this project it is required to have some tools. It is needed a C compiler as `gcc` (or others) and a OS as `gnu/linux`.
In the case we are using a Lubuntu distribution.
We are assume netcat is installed by default in the OS. Otherwise, you will have to install it.


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

After that, two scripts will be created: `stock_server` and `operations_terminal`.


# Run

To run the server you just need to run:
```bash
./stock_server {STOCK PORT} {CONFIG PORT} {config file}
```
In other terminal, make use of `netcat` to simulate the admin usage.
```bash
nc -u localhost 9876
```
Finally, for user usage:
```bash
./operations_terminal {IP ADDRESS of server} {STOCK PORT}
```

## Runing server

First of all, the admin must login with their credentials. To do this, just press enter and the server will send you some mesages with the respective instructions.

After you run the server and netcat, it is time for the admin to send some instructions to the server.

# Admin interaction

### Operations
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

#### To turn off the server
```bash
QUIT_SERVER
```

# User interaction

When you log in as a user, the server will ask to put your credentials. Only it's possible if previouslythe admin added you.

The operations are selected by a number. The server will send you all information about what you need to do.

You can:
#### Subcrive a market to get all informations by multicast
#### Buy auctions
#### Sell auctions
#### Turn on/off the feed of market
#### Check your wallet
#### Log out

## Collaborators

João - joaomoreira@student.dei.uc.pt https://github.com/JoaoESmoreira


Rafael - rafaelg@student.dei.uc.pt  https://github.com/rafag00
