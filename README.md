<a name="top"></a>

<div align="center">
  <img src="https://www.dieter-schwarz-stiftung.de/files/Projects/Project%20logos/Logo_42HN-min.jpg" alt="Logo"/>
</div>

<br>
<div align="center">

### 🛠 C++ (Object Oriented Programming)

</div>

# IRC

## 1. Introduction

This project creates a IRC server, using C++98. IRC (*Internet Realy Chat*) is a classic, text-based, real-time chat system. It is developed to work with the *Irssi* IRC client and manages the communication between all its instances connected to the same network. The communication protocol is based on predefined commands that allow users to send and receive messages as plain ASCII text.

The primary job of an IRC server is to listen for connections from clients, manage channels, and relay messages between the connected clients.

The program is called as follows:

<div align="center">

```$ ./IRC <port_number> <password>```
</div>


<div align="right">
  <a href="#top">⬆️ Return to top</a>
</div>

<br>


## 2. Server

The *Server*, is the central hub in the client-server model that powers the classic, text-based chat system. Its primary functionality is to accept connections from IRC clients, manage and coordinate all chat activity within its domain, and then efficiently relay messages between users, both in public discussion forums called *channels* and in *private* one-on-one conversations.



<div align="right">
  <a href="#top">⬆️ Return to top</a>
</div>

<br>


## 3. Client

As 