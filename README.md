# C-NQueens
The n-queens problem reduces to finding n unique integers between 1 and n (a permutation) such that no two numbers in the list differ by the distance between them in the list (the diagonal property).
This program will generate all solutions to the n-queens problem and returns the number of solutions found.
The application is multithreaded to take advantage of multi-core processors, and ensures that synchronization between the different threads is handled accordingly to achieve maximum parallelism.   

server.c and client.c attempts to solve the problem by distributing the workload over several servers. Each server will try to solve a portion of the problem and report back to the client while maintaining maximum parallelism.


