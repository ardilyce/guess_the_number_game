# CS204 Homework 6 - Guess the Number Game using Threads

## Overview

This project is a multithreaded C++ program that simulates a "Guess the Number" game. The game involves multiple players, each represented by a separate thread, who try to guess a target number set by the host (another thread). The game is played over several rounds, and the program ensures fair play by synchronizing the start time for all players.

## Features

- Multithreaded gameplay with one thread for each player and one for the host.
- Players guess the target number randomly within a specified range.
- The game manages synchronization and avoids conflicts using mutexes.
- Detailed output showing the progress and results of each round.

## Requirements

- C++11 or later
- A C++ compiler (e.g., g++, clang++)
- Standard library support for threads and mutexes
