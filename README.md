# Data_Link_Layer_Protocols_Simulation
Data link layer protocols simulation between peers that are connected with a noisy channel, where the transmission is not error-free, packets may get corrupted,  duplicated, delayed, or lost, and the buffers are of limited sizes.

The system network topology consists of three pairs of nodes [node0, node1],[node2, node3], and [node4, node5], and one coordinator that is connected to all the pairs.

![image](https://user-images.githubusercontent.com/49316071/150110545-cbda8e4c-e0df-4096-9829-7a7f63d92a21.png)

In this project, each pair of nodes communicate and exchange messages using the Selective Repeat algorithm with noisy channel and window of size M, using Byte Stuffing as a framing algorithm, CRC checksum as an error detection algorithm, and hamming code algorithm for single-bit errors correction.
