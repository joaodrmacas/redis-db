# Redis-DB


![Javatpoint](https://upload.wikimedia.org/wikipedia/en/thumb/6/6b/Redis_Logo.svg/1200px-Redis_Logo.svg.png)


# Usage

## Start server

Server doesn't require any parameters for now and it will run by default on **port '1234'**.

    make run_server

## Client

Redis client also connects by default to port '1234' and all the commands are case insesitive used via command line as shown below.

### Get command


get command searches for the value pair for the provided key and requires 1 arguments necessarily \<key\>. It returns the \<value\> pair for that key.

    make run_client get key

### Set command

set command saves a key value pair of any kind to the database and requires 2 arguments necessarily \<key\> \<value\>.

    make run_client set key value

### Del command

del command removes a key value pair from the database and requires 1 argument necessarily \<key>.

    make run_client del key

### Keys command

keys command lists all the current keys stored in the database at the moment. It doesn't receive any arguments.

    make run_client keys


# Goal

This redis like database is a simplified version of the original **redis-db** with the goal of having an in memory database for rapid access on stored data.

**This project is still in development, therefore it doesn't have all the functionalities I want.**

---

### Project Structures

This project was built in order to learn **network programming** and **data structures** concepts mainly **event based looping**, **protocols**, **timers**, **non blocking IO** that are used in real projects.

I tried to apply those concepts during this project, mainly by following [this book](https://build-your-own.org/redis/).

### Server Structure

The server has a non-blocking loop event using poll function, which allows the server to not wait until one client is done to start processing anothers request. This concept is crucial in databases since in a real world scenario clients cant be stuck waiting until every request before theirs have been processed.

All the connection are TCP-based using the following protocol for requests:

  
    | nstr | len | str1 | len | str2 | ... | len | strn |

and the following for responses in which res is a 32-bit status code

    | res | data... |
    

### Database Structure

All the data structures used to build the database are Intrusive, in order to make generic collection (without applying a type to it).

### Hashtables

To store all of the data in a key value pair format we use **Chaining Hashtables**. In this case we use 2 hashtables with **dynamic resizing**, meaning whenever the main table reaches the maximum capacity, we create a new one with double its size and progressively move the stored data in the old one to the new one.
This method allow the database to still be operable without having to wait until all the keys are move to the new one to be able to answer requests back. As drawback, this is more RAM-consuming as we need to store 2 different databases and could be prejudiced for servers run in low RAM capacity machines.  